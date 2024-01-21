#include "CollisionAlgorithmBoxBox.h"
#include "BoxCollider3D.h"
#include "UtilityForComplexCalculations.h"

// Class for finding a intersection between OBBs
class CFindOBBIntersection {
public:
    using FindIntersectionFuncPtr = void(CFindOBBIntersection::*)(Mkpe::Dbvt::BVOverlapPair*,
        const Vector3f&, float,
        Vector3f*, Vector3f*,
        const std::vector<int>&, const std::vector<int>&);

public:
    // Constructor
    CFindOBBIntersection() {
        // 4 : 4
        m_intersectFunc[3][3] = &CFindOBBIntersection::Find4_4;
        // 4 : 2
        m_intersectFunc[3][1] = &CFindOBBIntersection::Find4_2;
        m_intersectFunc[1][3] = &CFindOBBIntersection::Find4_2;
        // 4 : 1
        m_intersectFunc[3][0] = &CFindOBBIntersection::Find4_1;
        m_intersectFunc[0][3] = &CFindOBBIntersection::Find4_1;
        // 2 : 2
        m_intersectFunc[1][1] = &CFindOBBIntersection::Find2_2;
        // 2 : 1
        m_intersectFunc[1][0] = &CFindOBBIntersection::Find2_1;
        m_intersectFunc[0][1] = &CFindOBBIntersection::Find2_1;
        // 1 : 1
        m_intersectFunc[0][0] = &CFindOBBIntersection::Find1_1;
        // 3 vertices have no function
        m_intersectFunc[2][0] = nullptr;
        m_intersectFunc[2][1] = nullptr;
        m_intersectFunc[2][2] = nullptr;
        m_intersectFunc[2][3] = nullptr;
        m_intersectFunc[0][2] = nullptr;
        m_intersectFunc[1][2] = nullptr;
        m_intersectFunc[3][2] = nullptr;
    }

    // Find a intersection
    void FindIntersection(Mkpe::Dbvt::BVOverlapPair* pair,
        const Vector3f& normalB, float depth,
        Vector3f* vertexArrayA, Vector3f* vertexArrayB,
        const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB) {
        // Call function
        (this->*m_intersectFunc[closestIndexA.size() - 1][closestIndexB.size() - 1])(
            pair, normalB, depth, vertexArrayA, vertexArrayB, closestIndexA, closestIndexB
        );
    }

private:
    // Find a intersection at 4 : 4
    void Find4_4(Mkpe::Dbvt::BVOverlapPair* pair,
        const Vector3f& normalB, float depth,
        Vector3f* vertexArrayA, Vector3f* vertexArrayB,
        const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB);

    // Find a intersection at 4 : 2
    void Find4_2(Mkpe::Dbvt::BVOverlapPair* pair,
        const Vector3f& normalB, float depth,
        Vector3f* vertexArrayA, Vector3f* vertexArrayB,
        const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB);

    // Find a intersection at 4 : 1
    void Find4_1(Mkpe::Dbvt::BVOverlapPair* pair,
        const Vector3f& normalB, float depth,
        Vector3f* vertexArrayA, Vector3f* vertexArrayB,
        const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB);

    // Find a intersection at 2 : 2
    void Find2_2(Mkpe::Dbvt::BVOverlapPair* pair,
        const Vector3f& normalB, float depth,
        Vector3f* vertexArrayA, Vector3f* vertexArrayB,
        const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB);

    // Find a intersection at 2 : 1
    void Find2_1(Mkpe::Dbvt::BVOverlapPair* pair,
        const Vector3f& normalB, float depth,
        Vector3f* vertexArrayA, Vector3f* vertexArrayB,
        const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB);

    // Find a intersection at 1 : 1
    void Find1_1(Mkpe::Dbvt::BVOverlapPair* pair,
        const Vector3f& normalB, float depth,
        Vector3f* vertexArrayA, Vector3f* vertexArrayB,
        const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB);

private:
    // Stores pointer to intersection calculation function
    // #NOTE : Not necessary when the vertex is 3, but we will leave it as it is since there is no need for unnecessary calculations
    FindIntersectionFuncPtr m_intersectFunc[4][4];
};

// Are there nearly identical points in the coordinate array?
bool IsSamePointArray2Point(const std::vector<Vector3f>& pointArray, const Vector3f& point);

// Box and box collision algorithm
bool Mkpe::CollisionAlgorithm::BoxBox(Dbvt::BVOverlapPair* pair) {
    static CFindOBBIntersection calclate; // Class for calculation

    // Get the two wrappers
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();

    // Cast the colliders of the two wrappers to box colliders
    const CBoxCollider3D* boxA = static_cast<const CBoxCollider3D*>(wrapperA->GetCollider().Get());
    const CBoxCollider3D* boxB = static_cast<const CBoxCollider3D*>(wrapperB->GetCollider().Get());

    // Get positions of the two boxes
    const Vector3f& p1 = wrapperA->GetPos();
    const Vector3f& p2 = wrapperB->GetPos();

    // Get quaternions of the two boxes
    const Quaternionf& rotationA = wrapperA->GetRotation();
    const Quaternionf& rotationB = wrapperB->GetRotation();

    // Get rotation matrices of the two boxes
    Matrix3x3f matA = rotationA.GetMatrix();
    Matrix3x3f matB = rotationB.GetMatrix();

    // Calculate a distance vector between the two boxes
    Vector3f p = p2 - p1;

    // Calculate a vector that project the distance vector to the each axes of box A
    Vector3f pp(
        Utl::Math::DotMat3x3AxisToVector(matA, p, Utl::Math::_X),
        Utl::Math::DotMat3x3AxisToVector(matA, p, Utl::Math::_Y),
        Utl::Math::DotMat3x3AxisToVector(matA, p, Utl::Math::_Z)
    );

    // Get half lengths of the each boxes
    Vector3f sizeA = boxA->GetScalingSize() * Utl::Inv::_2;
    Vector3f sizeB = boxB->GetScalingSize() * Utl::Inv::_2;
    
    // Take dot products between each other's axis vectors
    float R11 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_X, Utl::Math::_X);
    float R12 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_X, Utl::Math::_Y);
    float R13 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_X, Utl::Math::_Z);
    float R21 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Y, Utl::Math::_X);
    float R22 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Y, Utl::Math::_Y);
    float R23 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Y, Utl::Math::_Z);
    float R31 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Z, Utl::Math::_X);
    float R32 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Z, Utl::Math::_Y);
    float R33 = Utl::Math::DotMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Z, Utl::Math::_Z);

    // Calculate its absolute value
    float Q11 = std::abs(R11);
    float Q12 = std::abs(R12);
    float Q13 = std::abs(R13);
    float Q21 = std::abs(R21);
    float Q22 = std::abs(R22);
    float Q23 = std::abs(R23);
    float Q31 = std::abs(R31);
    float Q32 = std::abs(R32);
    float Q33 = std::abs(R33);

    // Variable declarations
    float s = (std::numeric_limits<float>::lowest)(); // Lowest penetration depth
    int   codeNum = 0; // Which code calculations have the greatest depth
    float bestDotA;    // Result of obbA projection to the axis with the lowest penetration depth
    float bestDotB;    // Result of obbA projection to the axis with the lowest penetration depth
    Matrix3x3f* normalAxisMat = nullptr;  // Pointer to rotation matrix with normal
    UINT8       normalIndex = 0;          // Index of the normal vector in the rotation matrix
    bool        isReverseNormal = false;  // Invert the normals?

    // Separate axis judgment of the axes that the collider has
    {
        // Lambda formula for separate axis judgment
        auto separationAxisJudge = [&](float distanceDot, float sizeDotA, float sizeDotB, Matrix3x3f* axisMat, UINT8 iNormal, int code) {
            float s2 = std::abs(distanceDot) - (sizeDotA + sizeDotB);
            if (s2 > 0) { return false; }
            // Update value if lowest depth
            if (s2 > s) {
                s = s2;
                normalAxisMat = axisMat;
                normalIndex = iNormal;
                codeNum = code;
                bestDotA = sizeDotA;
                bestDotB = sizeDotB;
                isReverseNormal = (distanceDot < 0);
            }
            return true;
        };

        // Separate axis judgment by axis of boxA
        // Separate axis judgment by X axis of boxA
        if (!separationAxisJudge(pp.x(), sizeA.x(), (sizeB.x() * Q11 + sizeB.y() * Q12 + sizeB.z() * Q13),
            &matA, Utl::Math::_X, 1)) {
            return false;
        }
        // Separate axis judgment by Y axis of boxA
        if (!separationAxisJudge(pp.y(), sizeA.y(), (sizeB.x() * Q21 + sizeB.y() * Q22 + sizeB.z() * Q23),
            &matA, Utl::Math::_Y, 2)) {
            return false;
        }
        // Separate axis judgment by Z axis of boxA
        if (!separationAxisJudge(pp.z(), sizeA.z(), (sizeB.x() * Q31 + sizeB.y() * Q32 + sizeB.z() * Q33),
            &matA, Utl::Math::_Z, 3)) {
            return false;
        }

        // Calculate a vector that project the distance vector to the each axes of box B
        Vector3f pp2(
            Utl::Math::DotMat3x3AxisToVector(matB, p, Utl::Math::_X),
            Utl::Math::DotMat3x3AxisToVector(matB, p, Utl::Math::_Y),
            Utl::Math::DotMat3x3AxisToVector(matB, p, Utl::Math::_Z)
        );

        // Separate axis judgment by axis of boxB
        // Separate axis judgment by X axis of boxB
        if (!separationAxisJudge(pp2.x(), (sizeA.x() * Q11 + sizeA.y() * Q21 + sizeA.z() * Q31), sizeB.x(),
            &matB, Utl::Math::_X, 4)) {
            return false;
        }
        // Separate axis judgment by Y axis of boxB
        if (!separationAxisJudge(pp2.y(), (sizeA.x() * Q12 + sizeA.y() * Q22 + sizeA.z() * Q32), sizeB.y(),
            &matB, Utl::Math::_Y, 5)) {
            return false;
        }
        // Separate axis judgment by Z axis of boxB
        if (!separationAxisJudge(pp2.z(), (sizeA.x() * Q13 + sizeA.y() * Q23 + sizeA.z() * Q33), sizeB.z(),
            &matB, Utl::Math::_Z, 6)) {
            return false;
        }
    }

    // Separate axis judgment of cross product vectors
    {
        // Lambda formula for separate axis judgment
        auto separationAxisJudge = [&](float distanceDot, float sizeDotA, float sizeDotB, const Vector3f& axis, int code) {
            float s2 = std::abs(distanceDot) - (sizeDotA + sizeDotB);
            if (s2 > 0) { return false; }
            return true;
        };

        // #NOTE : If the vectors are parallel to each other, 
        //         there is no need to judge whether they are parallel or not, 
        //         because there is no need to judge whether the cross product vector is a separate axis.

        // A's X axis x B's X,Y,Z axis
        // A's X axis x B's X axis
        float dotA;
        float dotB;
        Vector3f cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_X, Utl::Math::_X);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Y) * sizeA.y()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Z) * sizeA.z());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Y) * sizeB.y()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Z) * sizeB.z());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 7)) { return false; }
        }
        // A's X axis x B's Y axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_X, Utl::Math::_Y);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Y) * sizeA.y()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Z) * sizeA.z());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_X) * sizeB.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Z) * sizeB.z());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 8)) { return false; }
        }
        // A's X axis x B's Z axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_X, Utl::Math::_Z);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Y) * sizeA.y()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Z) * sizeA.z());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_X) * sizeB.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Y) * sizeB.y());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 9)) { return false; }
        }


        // A's Y axis x B's X,Y,Z axis
        // A's Y axis x B's X axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Y, Utl::Math::_X);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_X) * sizeA.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Z) * sizeA.z());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Y) * sizeB.y()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Z) * sizeB.z());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 10)) { return false; }
        }
        // A's Y axis x B's Y axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Y, Utl::Math::_Y);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_X) * sizeA.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Z) * sizeA.z());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_X) * sizeB.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Z) * sizeB.z());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 11)) { return false; }
        }
        // A's Y axis x B's Z axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Y, Utl::Math::_Z);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_X) * sizeA.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Z) * sizeA.z());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_X) * sizeB.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Y) * sizeB.y());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 12)) { return false; }
        }


        // A's Z axis x B's X,Y,Z axis
        // A's Z axis x B's X axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Z, Utl::Math::_X);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_X) * sizeA.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Y) * sizeA.y());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Y) * sizeB.y()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Z) * sizeB.z());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 13)) { return false; }
        }
        // A's Z axis x B's Y axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Z, Utl::Math::_Y);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_X) * sizeA.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Y) * sizeA.y());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_X) * sizeB.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Z) * sizeB.z());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 14)) { return false; }
        }
        // A's Z axis x B's Z axis
        cross = Utl::Math::CrossMat3x3AxisToMat3x3Axis(matA, matB, Utl::Math::_Z, Utl::Math::_Z);
        if (!Utl::Math::IsUnitVector3fZero(cross)) {
            // Normalize
            cross.Normalize();
            // Project lengths of A and B
            dotA =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_X) * sizeA.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matA, cross, Utl::Math::_Y) * sizeA.y());
            dotB =
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_X) * sizeB.x()) +
                std::abs(Utl::Math::DotMat3x3AxisToVector(matB, cross, Utl::Math::_Y) * sizeB.y());
            // Separate axis judgment
            if (!separationAxisJudge(cross.Dot(p), dotA, dotB, cross, 15)) { return false; }
        }
    }

    // #NOTE : It's a collision when it comes to this point.

    // If the colliders overlap each other, contact points are not calculated
    if (pair->CanOverlapAfterApproval()) {
        return false;
    }
    
    // Assign rotation matrix axis to the normal
    Vector3f normal(0.0f, 0.0f, 0.0f); // Normal vector of the contact point
    if (normalAxisMat != nullptr) {
        normal = Vector3f(
            (*normalAxisMat)(normalIndex, Utl::Math::_X), 
            (*normalAxisMat)(normalIndex, Utl::Math::_Y), 
            (*normalAxisMat)(normalIndex, Utl::Math::_Z));
    }

    // If the normal needed to inverse, inverse it
    if (isReverseNormal) {
        normal = -normal;
    }

    // Calculate 8 vertices of the two boxes
    Vector3f vertexA[8];
    Vector3f vertexB[8];
    Utl::Calc::CalculateBoxVertex8(p1, sizeA, matA, vertexA);
    Utl::Calc::CalculateBoxVertex8(p2, sizeB, matB, vertexB);

    // Calculate one coordinate points in the midplane of the two boxes
    Vector3f separationPlaneA = p1 + normal * bestDotA;
    Vector3f separationPlaneB = p2 + normal * -bestDotB;

    // Calculate distances of the 8 vertices to rge midplane
    float vertexDistanceA[8];
    float vertexDistanceB[8];
    float minDistA = (std::numeric_limits<float>::max)();
    float minDistB = (std::numeric_limits<float>::max)();
    for (int i = 0; i < 8; ++i) {
        vertexDistanceA[i] = Utl::Calc::CalculateDistancePlanePoint(separationPlaneA, normal, vertexA[i]);
        if (vertexDistanceA[i] < minDistA) {
            minDistA = vertexDistanceA[i];
        }
        vertexDistanceB[i] = Utl::Calc::CalculateDistancePlanePoint(separationPlaneB, normal, vertexB[i]);
        if (vertexDistanceB[i] < minDistB) {
            minDistB = vertexDistanceB[i];
        }
    }

    // Calculate the indices of at most 4 vertices closest to the midplane
    std::vector<int> closestVertexIndexA;
    std::vector<int> closestVertexIndexB;
    int noSameMinIndexA = -1; // Index of the vertex of A that is not the same but is closest in distance
    int noSameMinIndexB = -1; // Index of the vertex of B that is not the same but is closest in distance
    for (int i = 0; i < 8; ++i) {
        // If it is almost equal to the shortest distance, it is the vertex closest to the midplane
        if (Utl::IsEqual(vertexDistanceA[i], minDistA)) {
            closestVertexIndexA.push_back(i);
        }
        else if (noSameMinIndexA == -1 || vertexDistanceA[i] < vertexDistanceA[noSameMinIndexA]) {
            noSameMinIndexA = i;
        }
        if (Utl::IsEqual(vertexDistanceB[i], minDistB)) {
            closestVertexIndexB.push_back(i);
        }
        else if (noSameMinIndexB == -1 || vertexDistanceB[i] < vertexDistanceB[noSameMinIndexB]) {
            noSameMinIndexB = i;
        }
    }

    // If the effect of accuracy results in 3 points being determined to be closest, 
    // add the next smallest point in error prevention and score 4 points.
    if (closestVertexIndexA.size() == 3) {
        closestVertexIndexA.push_back(noSameMinIndexA);
    }
    if (closestVertexIndexB.size() == 3) {
        closestVertexIndexB.push_back(noSameMinIndexB);
    }

    // Find a intersection
    calclate.FindIntersection(pair, -normal, s, vertexA, vertexB, closestVertexIndexA, closestVertexIndexB);

    // Return result
    return true;
}

// // Find a intersection at 4 : 4
void CFindOBBIntersection::Find4_4(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& normalB, float depth, 
    Vector3f* vertexArrayA, Vector3f* vertexArrayB, 
    const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB) {
    // Quaternions to be moved to a coordinate space where the normal direction is up
    Quaternionf spaceMoveRotate = Utl::Math::VectorToVectorQuaternion(Utl::Math::UNIT3_UP, normalB);

    // Move a vertex to another space coordinate
    Vector3f vertexSpaceA[4];
    Vector3f vertexSpaceB[4];
    int i = 0;
    // A
    for (int i = 0; i < 4; ++i) {
        vertexSpaceA[i] = spaceMoveRotate * vertexArrayA[closestIndexA[i]];
    }
    // B
    for (int i = 0; i < 4; ++i) {
        vertexSpaceB[i] = spaceMoveRotate * vertexArrayB[closestIndexB[i]];
    }

    // Sort each on the axis of centroid
    Utl::Calc::SortRectangleVertexCentroidXZ(vertexSpaceA);
    Utl::Calc::SortRectangleVertexCentroidXZ(vertexSpaceB);

    // Dynamic array to store intersection points
    std::vector<Vector3f> intersectPoint;

    // If a vertex is contained within the other quadrangle, it is intersections
    for (int i = 0; i < 4; ++i) {
        // Is A contained in B?
        if (Utl::Calc::IsPointInRectangleXZ(vertexSpaceA[i], vertexSpaceB[0], vertexSpaceB[1], vertexSpaceB[2], vertexSpaceB[3])) {
            // If there are no equal points, make it a contact point
            if (!IsSamePointArray2Point(intersectPoint, vertexSpaceA[i])) {
                intersectPoint.push_back(vertexSpaceA[i]);

                // y is once set to 0
                intersectPoint.back().y() = 0.0f;
            }
        }

        // If 4 vertices are found, exit loop
        if (intersectPoint.size() >= 4) { break; }

        // Is B contained in A?
        if (Utl::Calc::IsPointInRectangleXZ(vertexSpaceB[i], vertexSpaceA[0], vertexSpaceA[1], vertexSpaceA[2], vertexSpaceA[3])) {
            // If there are no equal points, make it a contact point
            if (!IsSamePointArray2Point(intersectPoint, vertexSpaceB[i])) {
                intersectPoint.push_back(vertexSpaceB[i]);

                // y is once set to 0
                intersectPoint.back().y() = 0.0f;
            }
        }

        // If 4 vertices are found, exit loop
        if (intersectPoint.size() >= 4) { break; }
    }

    // If the 4 intersections have not been determined
    if (intersectPoint.size() < 4) {
        Vector3f out1 = Vector3f::Zero();
        Vector3f out2 = Vector3f::Zero();
        bool isOut2 = false;

        for (int j = 0; j < 4; ++j) {
            for (int i = 0; i < 4; ++i) {
                // Does the line segment consisting of each of the 4 vertices intersect the line segment of the quadrangle?
                bool isIntersect = Utl::Calc::CalclateIntersectSegmentsXZ(vertexSpaceA[j], vertexSpaceA[(j + 1) % 4] - vertexSpaceA[j],
                    vertexSpaceB[i], vertexSpaceB[(i + 1) % 4] - vertexSpaceB[i],
                    &out1, &out2, &isOut2);

                // If intersected
                if (isIntersect) {
                    // If the same point does not already exist, add it to the array
                    if (!IsSamePointArray2Point(intersectPoint, out1)) {
                        intersectPoint.push_back(out1);
                    }
                    // If 4 vertices are found, exit loop
                    if (intersectPoint.size() >= 4) {
                        break;
                    }
                    // If only there was one more intersection
                    else if (isOut2) {
                        // If another collision point and an identical point do not already exist, add it to the array
                        if (!IsSamePointArray2Point(intersectPoint, out2)) {
                            intersectPoint.push_back(out2);
                        }
                    }
                }
            }

            // If 4 vertices are found, exit loop
            if (intersectPoint.size() >= 4) { break; }
        }
    }

    // Align with the Y coordinate of B to make it the collision point of B
    float y = vertexSpaceB[0].y();
    for (auto it : intersectPoint) {
        // Align Y coordinates
        it.y() = y;
        // Return collision point to world coordinate space
        it = spaceMoveRotate.GetConjugation() * it;
        
        // Add a contact point
        pair->contactData.AddContactPoint(normalB, it, depth);
    }
}

// Find a intersection at 4 : 2
void CFindOBBIntersection::Find4_2(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& normalB, float depth, 
    Vector3f* vertexArrayA, Vector3f* vertexArrayB, 
    const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB) {
    // Quaternions to be moved to a coordinate space where the normal direction is up
    Quaternionf spaceMoveRotate = Utl::Math::VectorToVectorQuaternion(Utl::Math::UNIT3_UP, normalB);

    // A is the side with 2 vertices?
    bool isA2Vertices = (closestIndexA.size() == 2) ? true : false;

    // Get array of 2 vertices and array of 2 vertices
    const std::vector<int>& vertex2Vector = (isA2Vertices) ? closestIndexA : closestIndexB;
    const std::vector<int>& vertex4Vector = (isA2Vertices) ? closestIndexB : closestIndexA;
    Vector3f* vertexArray2 = (isA2Vertices) ? vertexArrayA : vertexArrayB;
    Vector3f* vertexArray4 = (isA2Vertices) ? vertexArrayB : vertexArrayA;

    // Move a vertex to another space coordinate
    Vector3f vertex2space[2];
    Vector3f vertex4space[4];
    // The side with 2 vertices
    for (int i = 0; i < 2; ++i) {
        vertex2space[i] = spaceMoveRotate * vertexArray2[vertex2Vector[i]];
    }
    // The side with 4 vertices
    for (int i = 0; i < 4; ++i) {
        vertex4space[i] = spaceMoveRotate * vertexArray4[vertex4Vector[i]];
    }

    // Sort the array on the side with 4 vertices on the centroid
    Utl::Calc::SortRectangleVertexCentroidXZ(vertex4space);

    // If two vertices are contained within a quadrangle, it is an intersection
    std::vector<Vector3f> intersectPoint;
    for (int i = 0; i < 2; ++i) {
        if (Utl::Calc::IsPointInRectangleXZ(vertex2space[i], vertex4space[0], vertex4space[1], vertex4space[2], vertex4space[3])) {
            intersectPoint.push_back(vertex2space[i]);

            // y is once set to 0
            intersectPoint.back().y() = 0.0f;
        }
    }

    // If two intersections are not sought
    if (intersectPoint.size() < 2) {
        Vector3f out1 = Vector3f::Zero();
        Vector3f out2 = Vector3f::Zero();
        bool isOut2 = false;

        // Get the line segment vector of the side with two vertices
        Vector3f vVertex2 = vertex2space[1] - vertex2space[0];
        for (int i = 0; i < 4; ++i) {
            // Does the line segment consisting of each of the 2 vertices intersect the line segment of the quadrangle?
            bool isIntersect = Utl::Calc::CalclateIntersectSegmentsXZ(vertex2space[0], vVertex2, 
                vertex4space[i], vertex4space[(i + 1) % 4] - vertex4space[i],
                &out1, &out2, &isOut2);

            // If intersected
            if (isIntersect) {
                // If the same point does not already exist, add it to the array
                if (!IsSamePointArray2Point(intersectPoint, out1)) {
                    intersectPoint.push_back(out1);
                }
                // If 2 vertices are found, exit loop
                if (intersectPoint.size() >= 2) {
                    break;
                }
                // If only there was one more intersection
                else if (isOut2) {
                    // If another collision point and an identical point do not already exist, add it to the array
                    if (!IsSamePointArray2Point(intersectPoint, out2)) {
                        intersectPoint.push_back(out2);
                    }
                }
            }
        }
    }

    // Align with the Y coordinate of B to make it the collision point of B
    float y = (isA2Vertices) ? vertex4space[0].y() : vertex2space[0].y();
    for (auto it : intersectPoint) {
        // Align Y coordinates
        it.y() = y;
        // Return collision point to world coordinate space
        it = spaceMoveRotate.GetConjugation() * it;

        // Add a contact point
        pair->contactData.AddContactPoint(normalB, it, depth);
    }
}

// Find a intersection at 4 : 1
void CFindOBBIntersection::Find4_1(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& normalB, float depth, 
    Vector3f* vertexArrayA, Vector3f* vertexArrayB, 
    const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB) {

    // A is the side with 1 vertices?
    bool isA1Vertices = (closestIndexA.size() == 1) ? true : false;

    if (isA1Vertices) {
        // Add a contact point
        pair->contactData.AddContactPoint(normalB, vertexArrayA[closestIndexA[0]] + normalB * depth, depth);
    }
    else {
        // Add a contact point
        pair->contactData.AddContactPoint(normalB, vertexArrayB[closestIndexB[0]], depth);
    }
}

// Find a intersection at 2 : 2
void CFindOBBIntersection::Find2_2(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& normalB, float depth, 
    Vector3f* vertexArrayA, Vector3f* vertexArrayB, 
    const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB) {
    // Quaternions to be moved to a coordinate space where the normal direction is up
    Quaternionf spaceMoveRotate = Utl::Math::VectorToVectorQuaternion(Utl::Math::UNIT3_UP, normalB);

    // Move a vertex to another space coordinate
    Vector3f vertexSpaceA[2];
    Vector3f vertexSpaceB[2];
    // A
    for (int i = 0; i < 2; ++i) {
        vertexSpaceA[i] = spaceMoveRotate * vertexArrayA[closestIndexA[i]];
    }
    // B
    for (int i = 0; i < 2; ++i) {
        vertexSpaceB[i] = spaceMoveRotate * vertexArrayB[closestIndexB[i]];
    }

    // Variable declarations
    std::vector<Vector3f> intersectPoint;
    Vector3f out1 = Vector3f::Zero();
    Vector3f out2 = Vector3f::Zero();
    bool isOut2 = false;

    // Do the lines intersect each other?
    bool isIntersect = Utl::Calc::CalclateIntersectSegmentsXZ(vertexSpaceA[0], vertexSpaceA[1] - vertexSpaceA[0],
        vertexSpaceB[0], vertexSpaceB[1] - vertexSpaceB[0],
        &out1, &out2, &isOut2);

    // If intersected
    if (isIntersect) {
        // Add a contact point
        intersectPoint.push_back(out1);
        
        // If only there was one more intersection
        if (isOut2) {
            // If another collision point and an identical point do not already exist, add it to the array
            if (!IsSamePointArray2Point(intersectPoint, out2)) {
                intersectPoint.push_back(out2);
            }
        }
    }

    // Align with the Y coordinate of B to make it the collision point of B
    float y = vertexSpaceB[0].y();
    for (auto it : intersectPoint) {
        // Align Y coordinates
        it.y() = y;

        // Return collision point to world coordinate space
        it = spaceMoveRotate.GetConjugation() * it;

        // Add a contact point
        pair->contactData.AddContactPoint(normalB, it, depth);
    }
}

// Find a intersection at 2 : 1
void CFindOBBIntersection::Find2_1(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& normalB, float depth, 
    Vector3f* vertexArrayA, Vector3f* vertexArrayB,
    const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB) {

    // A is the side with 1 vertices?
    bool isA1Vertices = (closestIndexA.size() == 1) ? true : false;

    if (isA1Vertices) {
        // Add a contact point
        pair->contactData.AddContactPoint(normalB, vertexArrayA[closestIndexA[0]] + normalB * depth, depth);
    }
    else {
        // Add a contact point
        pair->contactData.AddContactPoint(normalB, vertexArrayB[closestIndexB[0]], depth);
    }
}

// Find a intersection at 1 : 1
void CFindOBBIntersection::Find1_1(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& normalB, float depth, 
    Vector3f* vertexArrayA, Vector3f* vertexArrayB, 
    const std::vector<int>& closestIndexA, const std::vector<int>& closestIndexB) {
    // Add a contact point
    pair->contactData.AddContactPoint(normalB, vertexArrayB[closestIndexB[0]], depth);
}

// Are there nearly identical points in the coordinate array?
bool IsSamePointArray2Point(const std::vector<Vector3f>& pointArray, const Vector3f& point) {
    for (const Vector3f& it : pointArray) {
        if (Utl::IsEqual(it.x(), point.x()) && Utl::IsEqual(it.z(), point.z())) {
            return true;
        }
    }
    return false;
}
