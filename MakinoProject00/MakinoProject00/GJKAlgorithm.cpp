#include "GJKAlgorithm.h"
#include "UtilityForComplexCalculations.h"
#include "UtilityForString.h"

// Minkowski difference information
struct MinkowskiPoint {
    Vector3f v;         // Minkowski difference vector
    Vector3f supportA;  // Support vector of A
    Vector3f supportB;  // Support vector of B

    // Constructor
    MinkowskiPoint() {}

    // Move constructor and move operator
    MinkowskiPoint(MinkowskiPoint&& move) = default;
    MinkowskiPoint& operator=(MinkowskiPoint&& move) = default;
};

// Face information
struct FaceIndex {
    size_t index[3]; // Indices array

    // Constructor
    FaceIndex(size_t a, size_t b, size_t c) { index[0] = a; index[1] = b; index[2] = c; }
};

// Edge information
struct EdgeIndex {
    size_t index[2]; // Indices array

    // Constructor
    EdgeIndex(size_t a, size_t b) { index[0] = a; index[1] = b; }
    
    // Comparison operator
    bool operator==(const EdgeIndex& other) const {
        return index[0] == other.index[0] && index[1] == other.index[1];
    }
};

// Normal information
struct NormalInfo {
    Vector3f normal; // Normalized normal vector
    float distance;  // Distance with face to origin

    // Constructor
    NormalInfo(const Vector3f& normal, float distance) : normal(normal), distance(distance) {}
};

// Simplex information
struct Simplex {
public:
    // Constructor
    Simplex() : m_size(0) {}

    // Add vertex
    void AddVertex(MinkowskiPoint* vert) {
        assert(m_size < 4);
        m_vertices[m_size] = std::move(*vert);
        m_size++;
    }

    // Resize size of the array of vertices
    void Resize(UINT size) {
        assert(size <= 4);
        m_size = size;
    }

    // Overwrite vertex
    void OverwriteVertex(UINT index, MinkowskiPoint* vert) {
        m_vertices[index] = std::move(*vert);
    }

    // Get the vertex of the array
    MinkowskiPoint* GetVertex(UINT index) { return &m_vertices[index]; }

    // Get the size of the array
    UINT GetSize() { return m_size; }

    // First iterator of vertex array
    auto begin() { return m_vertices.begin(); }
    // Tail iterator of vertex array
    auto end() { return m_vertices.end(); }
private:
    // Vertices array
    std::array<MinkowskiPoint, 4> m_vertices;
    // Current size of the vertices array
    UINT m_size;
};

// Calculate support vector
void CalculateSupport(const Vector3f& dirA, MinkowskiPoint* point,
    const ACCollider3D* colliderA, const Vector3f& posA, const Matrix3x3f& matA, const Matrix3x3f& invMatA,
    const ACCollider3D* colliderB, const Vector3f& posB, const Matrix3x3f& matB, const Matrix3x3f& invMatB);

// What to do with a simplex?
// #NOTE : Only DoSimplex4 determines that it is in contact (returns 1)
// @return 1 if the origin is contained, 0 if the algorithm is continued, -1 if the algorithm exits assuming no intersection
int DoSimplex(Simplex* simplex, Vector3f* nextDir);
// What to do with a edge?
int DoSimplex2(Simplex* simplex, Vector3f* nextDir);
// What to do with a triangle?
int DoSimplex3(Simplex* simplex, Vector3f* nextDir);
// What to do with a tetrahedron?
int DoSimplex4(Simplex* simplex, Vector3f* nextDir);

// Calculate a normal vectors of a faces
// @return Index of the face closest to the origin
size_t CalculateFaceNormal(const std::vector<MinkowskiPoint>& polytope, const std::vector<FaceIndex>& faces, const Vector3f& center, std::vector<NormalInfo>* normals);
// If it's a non-duplicate edge, add it
void AddNonDuplicationEdge(std::vector<EdgeIndex>* uniqueEdges, size_t a, size_t b);

// If the GJK algorithm repeats this number of times, it will exit
constexpr int GJK_ITERATION_MAX = 1000;

// GJK(Gilbert-Johnson-Keerthi) collision algorithm
bool Mkpe::CollisionAlgorithm::GJKAlgorithm(Dbvt::BVOverlapPair* pair) {
    // Get the two wrappers
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();

    // Get the colliders of the two wrappers
    const ACCollider3D* colliderA = wrapperA->GetCollider().Get();
    const ACCollider3D* colliderB = wrapperB->GetCollider().Get();

    // Get coordinates and convert them to a local coordinate space with the origin at the midpoint of the two colliders
    const Vector3f& worldPosA = wrapperA->GetPos();
    const Vector3f& worldPosB = wrapperB->GetPos();
    Vector3f midPoint = (worldPosA + worldPosB) * Utl::Inv::_2;
    Vector3f localPosA = worldPosA - midPoint;
    Vector3f localPosB = worldPosB - midPoint;

    // Calculate rotation and inverse rotation matrices
    // #NOTE : Use rotation matrices rather than quaternions for computational efficiency
    Matrix3x3f matA = wrapperA->GetRotation().GetMatrix();
    Matrix3x3f matB = wrapperB->GetRotation().GetMatrix();
    Matrix3x3f invMatA = matA.Transpose();
    Matrix3x3f invMatB = matB.Transpose();

    // Variable declarations
    Vector3f dir = Vector3f(1.0f, 0.0f, 0.0f); // Search direction
    Simplex simplex; // Simplex

    // Calculate first support vector
    MinkowskiPoint support;
    CalculateSupport(dir, &support,
        colliderA, localPosA, matA, invMatA,
        colliderB, localPosB, matB, invMatB);

    // Add a new vertex to the simplex
    simplex.AddVertex(&support);

    // Determine next search direction
    dir = -simplex.GetVertex(0)->v;

    // Start iteration
    int iterate1 = 0;
    for (; iterate1 < GJK_ITERATION_MAX; ++iterate1) {
        // Calculate support vector
        CalculateSupport(dir, &support,
            colliderA, localPosA, matA, invMatA,
            colliderB, localPosB, matB, invMatB);

        // If it is not moving in the direction of origin, it is not in contact.
        float dot = support.v.Dot(dir);
        if (dot <= 0.0f) {
            return false;
        }

        // Add a new vertex to the simplex
        simplex.AddVertex(&support);

        // Simplex judgmenet
        int flag = DoSimplex(&simplex, &dir);
        // If contact is made, the search is end
        if (flag == 1) {
            break;
        }
        // If there is no contact, it's ended
        else if (flag == -1) {
            return false;
        }

        // If the length of the direction vector is 0, there is no contact
        if (dir.LengthSq() <= FLT_EPSILON) {
            return false;
        }
    }

    // If it were the iteration limit
    if (iterate1 >= GJK_ITERATION_MAX - 1) {
        // Output warning message
        std::wstring debugMessage = L"WARNING：Can significantly affect performance.\n"
              L"The iteration limit was exceeded in the GJK algorithm collision detection for "
            + std::wstring(Utl::Str::string2WString(typeid(*colliderA->GetGameObject()).name()))
            + L" and " + std::wstring(Utl::Str::string2WString(typeid(*colliderB->GetGameObject()).name()))
            + L". Something is wrong with the program.";
        OutputDebugString(debugMessage.c_str());

        // Exit
        return false;
    }

    // #NOTE : It's a collision when it comes to this point.

    // If the colliders overlap each other, contact points are not added
    if (pair->CanOverlapAfterApproval()) {
        return false;
    }

    // Collision information creation by EPA (Expanding Polytope Algorithm) algorithm
    // #NOTE : Only DoSimplex4 returns the result that it is in contact, which guarantees that the simplex is tetrahedral.
    constexpr size_t INIT_CAPACITY = 10; // A value that will end up with a polyhedron with roughly this many vertices

    // Transfer all vertices to a super polyhedron
    std::vector<MinkowskiPoint> polytope(std::make_move_iterator(simplex.begin()), std::make_move_iterator(simplex.end()));
    polytope.reserve(INIT_CAPACITY);
    // Composition of the first face
    std::vector<FaceIndex> faces = {
        FaceIndex(0, 1, 2),
        FaceIndex(0, 3, 1),
        FaceIndex(0, 2, 3),
        FaceIndex(1, 3, 2),
    };
    faces.reserve(INIT_CAPACITY);

    // Calculate the coordinates of the center of a super polyhedron
    // #NOTE : It is important that it is a point inside the super polyhedron, although it will not be a central coordinate later on.
    Vector3f center = Vector3f::Zero();
    for (auto& it : polytope) {
        center += it.v;
    }
    center /= (float)polytope.size();

    // Normals array
    std::vector<NormalInfo> normals;
    normals.reserve(INIT_CAPACITY);
    // Calculate normal informations and get the index of the faces closest to the origin
    size_t nearFaceIndex = CalculateFaceNormal(polytope, faces, center, &normals);

    // Start EPA Algorithm
    NormalInfo nearNormal(normals[nearFaceIndex]); // Normal closest to the origin
    int iterate2 = 0;
    for (; iterate2 < GJK_ITERATION_MAX; ++iterate2) {
        // Calculate support vector
        CalculateSupport(nearNormal.normal, &support,
            colliderA, localPosA, matA, invMatA,
            colliderB, localPosB, matB, invMatB);

        // Calculate projection to the normal direction
        float distance = nearNormal.normal.Dot(support.v);

        // If there are more vertices in the normal direction
        constexpr float SIMILAR_DISTANCE = 0.002f;
        if (distance - nearNormal.distance > SIMILAR_DISTANCE) {
            std::vector<EdgeIndex> uniqueEdges;

            // Check that all normals are looking in the same direction as the support vector
            for (size_t i = 0; i < normals.size(); ++i) {
                // If a normal is looking in the same direction as the support vector
                if (normals[i].normal.Dot(support.v) > normals[i].normal.Dot(polytope[faces[i].index[0]].v)) {
                    size_t* faceIndex = faces[i].index;

                    // If it's a non-duplicate edge, add it
                    AddNonDuplicationEdge(&uniqueEdges, faceIndex[0], faceIndex[1]);
                    AddNonDuplicationEdge(&uniqueEdges, faceIndex[1], faceIndex[2]);
                    AddNonDuplicationEdge(&uniqueEdges, faceIndex[2], faceIndex[0]);

                    // Erase face and normal
                    faces[i] = faces.back();
                    normals[i] = normals.back();
                    faces.pop_back();
                    normals.pop_back();

                    // Decrement index
                    i--;
                }
            }

            // Form new faces with unique edges
            std::vector<FaceIndex> newFaces;
            size_t newVertex = polytope.size();
            for (auto& it : uniqueEdges) {
                newFaces.push_back(FaceIndex(it.index[0], it.index[1], newVertex));
            }

            // Add new vertex
            polytope.push_back(std::move(support));

            // New normals array
            std::vector<NormalInfo> newNormals;
            newNormals.reserve(newFaces.size());
            // Calculate the normal information of the new face and get the index of the face closest to the origin
            size_t newNearFaceIndex = CalculateFaceNormal(polytope, newFaces, center, &newNormals);

            // Re-search for the side closest to the origin, as some vertices may be missing
            float oldNearDistance = FLT_MAX;
            size_t normalSize = normals.size();
            for (size_t i = 0; i < normalSize; ++i) {
                // If it is the closest face, it is hold
                if (normals[i].distance < oldNearDistance) {
                    oldNearDistance = normals[i].distance;
                    nearFaceIndex = i;
                }
            }

            // If the new face is closer, update the closest face
            if (newNormals[newNearFaceIndex].distance < oldNearDistance) {
                nearFaceIndex = newNearFaceIndex + normalSize;
            }

            // Add new faces and new normals
            faces.insert(faces.end(), newFaces.begin(), newFaces.end());
            normals.insert(normals.end(), newNormals.begin(), newNormals.end());

            // Update the normals of the closest face
            nearNormal = normals[nearFaceIndex];
        }
        // If the point closest to the origin has already been calculated, exit
        else {
            break;
        }
    }

    // If it were the iteration limit
    if (iterate2 >= GJK_ITERATION_MAX - 1) {
        // Output warning message
        std::wstring debugMessage = L"WARNING：Can significantly affect performance.\n→"
              L"The iteration limit was exceeded in the creation of collision information by the EPA algorithm for "
            + std::wstring(Utl::Str::string2WString(typeid(*colliderA->GetGameObject()).name()))
            + L" and " + std::wstring(Utl::Str::string2WString(typeid(*colliderB->GetGameObject()).name()))
            + L". Something is wrong with the program.";
        OutputDebugString(debugMessage.c_str());

        // Exit
        return false;
    }

    // Calculate contact point
    Barycentric3f barycentric;
    Utl::Calc::ClosestTriangle2Point(Vector3f::Zero(),
        polytope[faces[nearFaceIndex].index[0]].v,
        polytope[faces[nearFaceIndex].index[1]].v,
        polytope[faces[nearFaceIndex].index[2]].v,
        &barycentric);

    // Calculate contact point on the collider B
    Vector3f contactB =
        polytope[faces[nearFaceIndex].index[0]].supportB * barycentric.u() +
        polytope[faces[nearFaceIndex].index[1]].supportB * barycentric.v() +
        polytope[faces[nearFaceIndex].index[2]].supportB * barycentric.w();
    // Move the contact point into world coordinate space
    contactB += midPoint;

    // Add a contact point
    pair->contactData.AddContactPoint(-nearNormal.normal, contactB, -nearNormal.distance);

    // Return result
    return true;
}

// Calculate support vector
void CalculateSupport(const Vector3f& dirA, MinkowskiPoint* point,
    const ACCollider3D* colliderA, const Vector3f& posA, const Matrix3x3f& matA, const Matrix3x3f& invMatA, 
    const ACCollider3D* colliderB, const Vector3f& posB, const Matrix3x3f& matB, const Matrix3x3f& invMatB) {
    // Calculate the local direction vectors of each collider
    Vector3f localDirA = matA * dirA;
    Vector3f localDirB = matB * (-dirA);

    // Calculate support vectors in local coordinate space
    Vector3f localSupportA = colliderA->GetLocalSupportVector(localDirA);
    Vector3f localSupportB = colliderB->GetLocalSupportVector(localDirB);

    // Convert support vectors to world coordinate system
    point->supportA = invMatA * localSupportA + posA;
    point->supportB = invMatB * localSupportB + posB;
    
    // Calculate Minkowski difference vector
    point->v = point->supportA - point->supportB;
}

// What to do with a simplex?
int DoSimplex(Simplex* simplex, Vector3f* nextDir) {
    UINT size = simplex->GetSize();
    // If the number of the vertices is 2
    if (size == 2) {
        return DoSimplex2(simplex, nextDir);
    }
    // If the number of the vertices is 3
    else if (size == 3) {
        return DoSimplex3(simplex, nextDir);
    }
    // If the number of the vertices is 4
    else {
        return DoSimplex4(simplex, nextDir);
    }
}

// What to do with a edge?
int DoSimplex2(Simplex* simplex, Vector3f* nextDir) {
    // Latest vertex
    MinkowskiPoint* A = simplex->GetVertex(1);
    // Other vertex
    MinkowskiPoint* B = simplex->GetVertex(0);

    // Calculate a vector from the latest vertex to another vertex
    Vector3f AB = B->v - A->v;
    // Create a vector from the latest vertex to the origin
    Vector3f AO = -A->v; /* == 0 - A */

    // Project AO to AB
    float ab_ao = AB.Dot(AO);

    /*
     * Since the search was done from B to A, there is no origin in the Voronoi area of B.
     * Therefore, if AB and AO are in the same direction, the origin exists in the Voronoi region of edge AB.
     * If not, then the origin exists in the Voronoi region of A.
     */ 

    // If AB and AO are in the same direction, the origin exists in the Voronoi region of edge AB
    if (ab_ao > 0.0f) {
        // In this case, AB x AO x AB is the next search direction
        *nextDir = AB.Cross(AO).Cross(AB);
    }
    // The origin exists in the Voronoi region of A
    else {
        // Reduce the number of dimensions of a simplex to have only A vertex
        simplex->OverwriteVertex(0, A);
        simplex->Resize(1);
        // Next search direction is a vector from A to O
        *nextDir = AO;
    }

    // Continue
    return 0;
}

// What to do with a triangle?
int DoSimplex3(Simplex* simplex, Vector3f* nextDir) {
    // Latest vertex
    MinkowskiPoint* A = simplex->GetVertex(2);
    // Other vertices
    MinkowskiPoint* B = simplex->GetVertex(1);
    MinkowskiPoint* C = simplex->GetVertex(0);

    // Calculate a vector from A to the other vertex, then calculate the triangle's normal
    Vector3f AB = B->v - A->v;
    Vector3f AC = C->v - A->v;
    Vector3f N_ABC = AB.Cross(AC);

    // If the triangles have no area, then they don't intersect
    float areaSq = N_ABC.LengthSq();
    if (areaSq <= FLT_EPSILON) {
        return -1; // Exit algorithm
    }

    // Calculate a vector from A to the origin
    Vector3f AO = -A->v;

    // ABC x AC
    // Calculate the outward vector vertical to AC by the cross product of the triangle's normal and AC
    Vector3f VerticalAC = N_ABC.Cross(AC);
    // If VerticalAC・AO is positive, then the origin exists outside of AC
    float dot = VerticalAC.Dot(AO);
    if (dot >= -FLT_EPSILON) {
        // AC
        // If AC・AO is positive、then the origin exists in the Voronoi region of AC
        dot = AC.Dot(AO);
        if (dot >= -FLT_EPSILON) {
            // Reduce the number of dimensions of the simplex and make it a CA edge
            simplex->OverwriteVertex(1, A);
            simplex->Resize(2);
            // In this case, AC x AO x AC is the next search direction
            *nextDir = AC.Cross(AO).Cross(AC);
        }
        // Since this is not the case, the origin exists on the vertex A side
        else {
            // AB
            // The origin exists in the Voronoi region of AB edge
            dot = AB.Dot(AO);
            if (dot >= -FLT_EPSILON) {
                // Reduce the number of dimensions of the simplex and make it a BA edge
                simplex->OverwriteVertex(0, B);
                simplex->OverwriteVertex(1, A);
                simplex->Resize(2);
                // In this case, AB x AO x AB is the next search direction
                *nextDir = AB.Cross(AO).Cross(AB);
            }
            // The origin exists in the Voronoi region of A
            else {
                // Reduce the number of dimensions of the simplex and make vertex A the only vertex
                simplex->OverwriteVertex(0, A);
                simplex->Resize(1);
                // Next search direction is a vector from A to O
                *nextDir = AO;
            }
        }
    }
    // the origin exists inside of AC
    else {
        // AB×ABC
        // If VerticalAB・AO is positive, then the origin exists outside of AB
        Vector3f VerticalAB = AB.Cross(N_ABC);
        dot = VerticalAB.Dot(AO);
        if (dot >= -FLT_EPSILON) {
            // AB
            // The origin exists in the Voronoi region of AB edge
            dot = AB.Dot(AO);
            if (dot >= -FLT_EPSILON) {
                // Reduce the number of dimensions of the simplex and make it a BA edge
                simplex->OverwriteVertex(0, B);
                simplex->OverwriteVertex(1, A);
                simplex->Resize(2);
                // In this case, AB x AO x AB is the next search direction
                *nextDir = AB.Cross(AO).Cross(AB);
            }
            // The origin exists in the Voronoi region of A
            else {
                // Reduce the number of dimensions of the simplex and make vertex A the only vertex
                simplex->OverwriteVertex(0, A);
                simplex->Resize(1);
                // Next search direction is a vector from A to O
                *nextDir = AO;
            }
        }
        // The origin exists inside the ABC triangle.
        else {
            // ABC normal
            // If N_ABC・AO is positive, then the origin exists on the upper side of the triangle
            dot = N_ABC.Dot(AO);
            if (dot >= -FLT_EPSILON) {
                // Next search direction is a ABC normal direction
                *nextDir = N_ABC;
            }
            // the origin exists on the lower side of the triangle
            else {
                // Next search direction is the inverse of a ABC normal direction
                *nextDir = -N_ABC;

                // Reverse the orientation of the triangle
                MinkowskiPoint temp = std::move(*C);
                simplex->OverwriteVertex(0, B);
                simplex->OverwriteVertex(1, &temp);
            }
        }
    }

    // Continue
    return 0;
}

// What to do with a tetrahedron?
int DoSimplex4(Simplex* simplex, Vector3f* nextDir) {
    // Latest vertex
    MinkowskiPoint* A = simplex->GetVertex(3);
    // Other vertices
    MinkowskiPoint* B = simplex->GetVertex(2);
    MinkowskiPoint* C = simplex->GetVertex(1);
    MinkowskiPoint* D = simplex->GetVertex(0);

    // If the distance from triangle BCD to A is almost zero, it is not a tetrahedron because it has no volume. No intersections can be found
    Vector3f closestPoint = Utl::Calc::ClosestTriangle2Point(A->v, B->v, C->v, D->v);
    float lenSq = (A->v - closestPoint).LengthSq();
    if (lenSq <= FLT_EPSILON) {
        return -1; // Exit algorithm
    }

    // Judge whether the origin exists on the triangle that the tetrahedron has.
    // Does the origin exist on ABC?
    closestPoint = Utl::Calc::ClosestTriangle2Point(Vector3f::Zero(), A->v, B->v, C->v);
    lenSq = closestPoint.LengthSq(); /* == (0 - closestPoint).squaredNorm() */
    if (lenSq <= FLT_EPSILON) {
        return 1; // Contain the origin in the simplex
    }
    // Does the origin exist on ACD?
    closestPoint = Utl::Calc::ClosestTriangle2Point(Vector3f::Zero(), A->v, C->v, D->v);
    lenSq = closestPoint.LengthSq(); /* == (0 - closestPoint).squaredNorm() */
    if (lenSq <= FLT_EPSILON) {
        return 1; // Contain the origin in the simplex
    }
    // Does the origin exist on ABD?
    closestPoint = Utl::Calc::ClosestTriangle2Point(Vector3f::Zero(), A->v, B->v, D->v);
    lenSq = closestPoint.LengthSq(); /* == (0 - closestPoint).squaredNorm() */
    if (lenSq <= FLT_EPSILON) {
        return 1; // Contain the origin in the simplex
    }
    // Does the origin exist on BCD?
    closestPoint = Utl::Calc::ClosestTriangle2Point(Vector3f::Zero(), B->v, C->v, D->v);
    lenSq = closestPoint.LengthSq(); /* == (0 - closestPoint).squaredNorm() */
    if (lenSq <= FLT_EPSILON) {
        return 1; // Contain the origin in the simplex
    }

    // Calculate each vector
    Vector3f AO = -A->v; /* == 0 - A */
    Vector3f AB = B->v - A->v;
    Vector3f AC = C->v - A->v;
    Vector3f AD = D->v - A->v;
    // Calculate each normal vector
    Vector3f N_ABC = AB.Cross(AC);
    Vector3f N_ACD = AC.Cross(AD);
    Vector3f N_ADB = AD.Cross(AB);

    // For each triangle, get the directions of the fourth vertex
    int ABC_Dir_D = Utl::Sign(N_ABC.Dot(AD));
    int ACD_Dir_B = Utl::Sign(N_ACD.Dot(AB));
    int ADB_Dir_C = Utl::Sign(N_ADB.Dot(AC));

    // For each triangle, get the directions of the origin
    int ABC_Dir_O = Utl::Sign(N_ABC.Dot(AO));
    int ACD_Dir_O = Utl::Sign(N_ACD.Dot(AO));
    int ADB_Dir_O = Utl::Sign(N_ADB.Dot(AO));

    // Does the origin exist inside each triangle?
    bool isInsideABC = (ABC_Dir_D == ABC_Dir_O);
    bool isInsideACD = (ACD_Dir_B == ACD_Dir_O);
    bool isInsideADB = (ADB_Dir_C == ADB_Dir_O);

    // The origin exists inside the tetrahedron.
    if (isInsideABC && isInsideACD && isInsideADB) {
        return 1; // Contain the origin in the simplex
    }
    // Since D is the farthest from the origin among all the vertices of the tetrahedron, advance to the ABC triangle case
    else if (false == isInsideABC) {
        simplex->OverwriteVertex(0, C);
        simplex->OverwriteVertex(1, B);
        simplex->OverwriteVertex(2, A);
        simplex->Resize(3);
    }
    // Since B is the farthest from the origin among all the vertices of the tetrahedron, advance to the ACD triangle case
    else if (false == isInsideACD) {
        /* simplex->OverwriteVertex(0, D); */
        /* simplex->OverwriteVertex(1, C); */
        simplex->OverwriteVertex(2, A);
        simplex->Resize(3);
    }
    // Since C is the farthest from the origin among all the vertices of the tetrahedron, advance to the ADB triangle case
    else {
        simplex->OverwriteVertex(1, D);
        simplex->OverwriteVertex(0, B);
        simplex->OverwriteVertex(2, A);
        simplex->Resize(3);
    }

    // Add triangle case
    return DoSimplex3(simplex, nextDir);
}

// Calculate a normal vectors of a faces
size_t CalculateFaceNormal(const std::vector<MinkowskiPoint>& polytope, const std::vector<FaceIndex>& faces, const Vector3f& center, std::vector<NormalInfo>* normals) {
    size_t nearFaceIndex = 0;       // Index of the face closest to the origin
    float nearDistance = FLT_MAX;   // Distance to the origin of the face closest to the origin

    // Processing for all faces
    size_t size = faces.size();
    for (size_t i = 0; i < size; ++i) {
        // Get all vertices of a face (triangle)
        const Vector3f& A = polytope[faces[i].index[0]].v;
        const Vector3f& B = polytope[faces[i].index[1]].v;
        const Vector3f& C = polytope[faces[i].index[2]].v;

        // Calculate a normal and distance to the origin
        Vector3f normal = ((B - A).Cross(C - A)).GetNormalize();
        float distance = normal.Dot(A);

        // If the normals are reversed, correct them
        // #NOTE : This algorithm cannot handle concave shapes.
        if (normal.Dot(A - center) < 0) {
            normal *= -1.0f;
            distance *= -1.0f;
        }

        // Add the normal information to the array
        normals->emplace_back(NormalInfo(normal, distance));

        // Retain if closest to origin
        if (distance < nearDistance) {
            nearFaceIndex = i;
            nearDistance = distance;
        }
    }

    // Return the index of the closest face
    return nearFaceIndex;
}

// If it's a non-duplicate edge, add it
void AddNonDuplicationEdge(std::vector<EdgeIndex>* uniqueEdges, size_t a, size_t b) {
    // Search for a edge in the opposite direction
    auto result = std::find(
        uniqueEdges->begin(),
        uniqueEdges->end(),
        EdgeIndex(b, a));

    // If a edge is found that is facing the opposite direction, delete it.
    if (result != uniqueEdges->end()) {
        *result = uniqueEdges->back();
        uniqueEdges->pop_back();
    }
    // If not found, add as unique edge
    else {
        uniqueEdges->emplace_back(EdgeIndex(a, b));
    }
}
