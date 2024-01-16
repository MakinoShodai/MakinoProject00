#include "UtilityForComplexCalculations.h"

// Calculate the shortest distance between a plane and a point
float Utl::Calc::CalculateDistancePlanePoint(const Vector3f& plane, const Vector3f& planeNormal, const Vector3f& point) {
    // Create a vector from the plane to the point
    Vector3f v = plane - point;

    // Project this vector to the normal of the plane
    return std::abs(planeNormal.Dot(v));
}

// Calculate the shortest distance between line segments
float Utl::Calc::CalculateDistanceSegments(const Vector3f& vCenterA2B, const Vector3f& dirA, const Vector3f& dirB, float halfLengthA, float halfLengthB,
    Vector3f* offsetA, Vector3f* offsetB, Vector3f* vTieClosestPoint) {
    // Variables declaration
    float tA;
    float tB;
    
    // Calculate dot products
    float dot_dirA2dirB = dirA.Dot(dirB);
    float dot_dirA2distance = dirA.Dot(vCenterA2B);
    float dot_dirB2distance = dirB.Dot(vCenterA2B);

    /*
     * When the closest points of each line segments A and B, are determined as pA and pB, do as follows.
     * cA,cB : Center coordinates of each line segments
     * tA,tB : Distance to the nearest point relative to the direction of its own direction vector
     * 
     * pA = cA + tA * dirA
     * pB = cB + tB * dirB
     * 
     * When the vector between the closest points, is determined v, do as follows.
     * v = pB - pA;
     * v = (cB + tB * dirB) - (cA + tA * dirA)
     *   = (cB - cA) + tB * dirB - tA * dirA 
     * 
     * Calculate the minimum value of this as v^2 (Length squared.
     * v^2 = ((cB - cA) + tB * dirB - tA * dirA) * ((cB - cA) + tB * dirB - tA * dirA)
     *     = (cB - cA)^2 + 2tB((cB - cA)・dirB) - 2tA((cB - cA)・dirA) - 2 * tB * tA(diB・dirA) + tB^2 * (dirB・dirB) + tA^2 * (dirA・dirA)
     *     = (cB - cA)^2 + 2tB((cB - cA)・dirB) - 2tA((cB - cA)・dirA) - 2 * tB * tA(diB・dirA) + tB^2 + tA^2
     * 
     * Partial differentiation of this equation with respect to tA.
     * ∂v^2 / ∂tA = -2((cB - cA)・dirA) - 2tB * (diB・dirA) + 2tA
     * 
     * Solve this equation for tA.
     *  0 = -2((cB - cA)・dirA) - 2tB * (diB・dirA) + 2tA
     *    = -((cB - cA)・dirA) - tB * (diB・dirA) + tA
     * tA = tB(diB・dirA) + ((cB - cA)・dirA)
     * 
     * Similarly, the partial diffentiation with respect to tB is solved for tB as follows.
     * tB = tA(dirB・dirA) - ((cB - cA)・dirB)
     * 
     * This is a simultaneous equation, and solving it yields.
     * tA = (tA(dirB・dirA) - ((cB - cA)・dirB))(diB・dirA) + ((cB - cA)・dirA)
     * tA = tA(dirB・dirA)^2 - ((cB - cA)・dirB)(diB・dirA) + ((cB - cA)・dirA)
     * tA - tA(dirB・dirA)^2 = -((cB - cA)・dirB)(diB・dirA) + ((cB - cA)・dirA)
     * tA(1 - (dirB・dirA)^2) = -((cB - cA)・dirB)(diB・dirA) + ((cB - cA)・dirA)
     * tA = ((cB - cA)・dirA) - ((cB - cA)・dirB)(diB・dirA) / (1 - (dirB・dirA)^2)
     */

    // Calculate the denominator
    float denom = 1.0f - dot_dirA2dirB * dot_dirA2dirB;

    // If this is parallel
    if (Utl::IsFloatZero(denom)) {
        tA = 0.0f;
    }
    else {
        tA = (dot_dirB2distance * dot_dirA2dirB - dot_dirA2distance) / denom;

        // Keep tA within the line segment of A
        tA = Utl::Clamp(tA, -halfLengthA, halfLengthA);
    }

    // Calculate tB
    // #NOTE : tB = tA(dirB・dirA) - ((cB - cA)・dirB)
    tB = tA * dot_dirA2dirB - dot_dirB2distance;

    // Keep tB within the line segment of B
    if (tB < -halfLengthB) {
        tB = -halfLengthB;
        // A changes with it
        // #NOTE : tA = tB(diB・dirA) + ((cB - cA)・dirA)
        tA = tB * dot_dirA2dirB + dot_dirA2distance;

        // Keep tA within the line segment of A
        tA = Utl::Clamp(tA, -halfLengthA, halfLengthA);
    }
    else if (tB > halfLengthB){
        tB = halfLengthB;
        // A changes with it
        // #NOTE : tA = tB(diB・dirA) + ((cB - cA)・dirA)
        tA = tB * dot_dirA2dirB + dot_dirA2distance;

        // Keep tA within the line segment of A
        tA = Utl::Clamp(tA, -halfLengthA, halfLengthA);
    }

    // Calculate offsets of each line segments
    *offsetA = tA * dirA;
    *offsetB = tB * dirB;

    // v = (cB - cA) + tB * dirB - tA * dirA 
    *vTieClosestPoint = vCenterA2B + *offsetB - *offsetA;

    // Return length
    return vTieClosestPoint->Length();
}

// Calculate the coordinate on the triangle closest to the point
Vector3f Utl::Calc::ClosestTriangle2Point(const Vector3f& p, const Vector3f& a, const Vector3f& b, const Vector3f& c, Barycentric3f* result) {
    /* 
     * Calculate the coordinates of the barycentric coordinate of point P in triangle ABC.
     * barycentric coordinate(u, v, w) is calculated by area of RAB / area of ABC,
     * using the vertical projection R of point R onto the triangle,
     * when calculating u, for example.
     * 
     * Also, since this is a good area ratio, it is expressed as follows.
     *   Area ratio of RAB / (Area ratio of RAB + Area ratio of RBC + Area ratio of RCA)
     * 
     * To do this calculation, we need to calculate the area ratio of the RAB, RBC, RCA triangles.
     * The following calculation is used for this.
     * Normal of triangle N = (B - A) x (C - A)
     *   [eq.1] Signed area of triangle PAB = ((A - R) x (B - R))・Normalize(N) / 2
     * Note that /2 is not necessary, since it is only the ratio of the area that needs to be calculated.
     * 
     * where eq.1 can be transformed into the following equation (steps omitted).
     *   ((A - P) x (B - P))・Normalize(N) / 2
     * 
     * I rewrite this to say.
     *   (PA x PB)・(AB x AC)
     * 
     * reference : http://fnorio.com/0126scalar_vector_product/scalar_vector_product.html
     * →(5) Section 4, "Lagrange's identity" and "Cauchy-Schwartz inequality."
     * Using (A x B)・(C x D) = (A・C)(B・D) - (A・D)(B・C) used in this reference, the area ratio can be obtained by the following equation
     *   [eq.2] (PA・AB)(PB・AC) - (PA・AC)(PB・AB)
     * 
     * Now, if point P is perpendicularly projected onto a triangle and it does not lie on the triangle,
     * Point P exists at either Voronoi area of vertex A,B,C or Voronoi area of edge AB,BC,CA.
     * 
     * # Judgment when point P exists in Voronoi area of vertex A.
     * If AP・AB < 0 and AP・AC < 0 is satisfied, then point P is in the Voronoi area of vertex A, 
     * so return point P with barycentric coordinate(1, 0, 0).
     * 
     * # Judgment when point P exists in Voronoi area of edge AB.
     * If barycentric coordinate(u,v,w) in A is negative, point P is determined to be outside edge AB.
     * Moreover, AP・AB >= 0 and BP・BA >= 0 determine that point P resides in the Voronoi area of edge AB.
     * In this case, we can use the ratio of the projection of the point P onto the edge AB, 
     * so we can return the nearest point at A + (AP・AB / AB・AB) * AB
     * 
     * In this case, AB・AB (length of edge AB) can be calculated by AP・AB + BP・BA, 
     * which has already been calculated, 
     * and the nearest point can be returned by A + (AP・AB / (AP・AB + BP・BA)) * AB.
     * 
     * Also, (AP・AB / AB・AB) and (AP・AB / (AP・AB + BP・BA)) used here are equal to the barycentric coordinate v.
     * And since w is known to be less than or equal to 0, 
     * we can express the coordinates of the barycentric coordinate in terms of (1 - v, v, 0).
     * 
     * # summary
     * So far, we know that AP,BP,CP, not PA,PB,PC, is easier to calculate.
     * Equation 2 contains two terms, both of which contain two of PA,PB,PC, so the result is the same even if the signs are reversed.
     * So we handle here with the following equation with PA,PB,PC inverted.
     *   PAB : (AP・AB)(BP・AC) - (AP・AC)(BP・AB) = (PA x PB)・(AB x AC)
     *   PBC : (BP・AB)(CP・AC) - (BP・AC)(CP・AB) = (PB x PC)・(AB x AC)
     *   PCA : (CP・AB)(AP・AC) - (CP・AC)(AP・AB) = (PC x PA)・(AB x AC)
     * In addition, PAB,PBC,PCA can be used to denote u,v,w by using the following, respectively.
     * u = PBC / ABC
     * v = PCA / ABC
     * w = PAB / ABC
     */

    // Calculate the vector of the line segments
    Vector3f AP = p - a;
    Vector3f BP = p - b;
    Vector3f CP = p - c;
    Vector3f AB = b - a;
    Vector3f AC = c - a;

    // Calculate the dot products needed
    float ap_ab = AP.Dot(AB);
    float ap_ac = AP.Dot(AC);

    // If it is the Voronoi area of vertex A
    if (ap_ab <= 0.0f && ap_ac <= 0.0f) {
        if (result) { *result = Barycentric3f(1.f, 0.f, 0.f); } // Barycentric coordinate(1, 0, 0)
        return a;
    }

    // Calculate the dot products needed
    float bp_ab = BP.Dot(AB);
    float bp_ac = BP.Dot(AC);

    // Determine if it is in the Voronoi area of vertex B (short version)
    // 
    // Conditions are BA・BP <= 0 and BC・BP <= 0
    // 
    // Now, the first condition can be represented by AB・BP >= 0.
    // 
    // The second conditional judgment is made by indirectly seeking BC.
    // From BC = C - B, AB = B - A, AC = C - A, 
    // we can represent the following.
    //   BC = AC - AB
    //   BC = (C - A) - (B - A)
    //      = C - A - B + A
    //      = C - B
    // 
    // Also, since BP・BC = BP・AC - BP・AB is known, 
    // use BP・AC - BP・AB <= 0 to judge the condition.
    float bp_bc = bp_ac - bp_ab;
    if (bp_ab >= 0.0f && bp_bc <= 0.0f) {
        if (result) { *result = Barycentric3f(0.f, 1.f, 0.f); } // Barycentric coordinate(0, 1, 0)
        return b;
    }

    // Calculate PAB
    float pab = ap_ab * bp_ac - ap_ac * bp_ab;
    // If it is the Voronoi area of edge AB
    if (pab <= 0.0f && ap_ab >= 0.0f && bp_ab <= 0.0f) {
        // (AP・AB / (AP・AB + BP・BA))
        float v = ap_ab / (ap_ab - bp_ab);
        if (result) { *result = Barycentric3f(1.f - v, v, 0.f); } // Barycentric coordinate(1 - v, v, 0)
        // Return the nearest point
        return a + AB * v;
    }

    // Calculate the dot products needed
    float cp_ab = CP.Dot(AB);
    float cp_ac = CP.Dot(AC);

    // Determine if it is in the Voronoi area of vertex C (short version)
    // Use CB = B - C = (B-A)-(C-A) for CP・CB,
    // and replace it with CP・CB = CP・AB - CP・AC.
    float cp_cb = cp_ab - cp_ac;
    if (cp_cb <= 0.0f && cp_ac >= 0.0f) {
        if (result) { *result = Barycentric3f(0.f, 0.f, 1.f); } // Barycentric coordinate(0, 0, 1)
        return c;
    }

    // Calculate PBC
    float pbc = bp_ab * cp_ac - bp_ac * cp_ab;
    // If it is the Voronoi area of edge BC.
    // Conditions are  u <= 0 and BP・BC >= 0, CP・CB >= 0.
    if (pbc <= 0.0f && bp_bc >= 0.0f && cp_cb >= 0.0f) {
        // BP・BC / (BP・BC + CP・CB)
        float w = bp_bc / (bp_bc + cp_cb);
        if (result) { *result = Barycentric3f(0.f, 1.f - w, w); } // Barycentric coordinate(0, 1 - w, w)
        // Return the nearest point
        return b + w * (c - b);
    }

    // Calculate PCA
    float pca = cp_ab * ap_ac - cp_ac * ap_ab;
    // If it is the Voronoi area of edge CA.
    // Conditions are  v <= 0 and AP・AC >= 0, CP・CA >= 0.
    if (pca <= 0.0f && ap_ac >= 0.0f && cp_ac <= 0.0f) {
        // AP・AC / (AP・AC + CP・CA)
        float w = ap_ac / (ap_ac - cp_ac);
        if (result) { *result = Barycentric3f(1.f - w, 0.f, w); } // Barycentric coordinate(1 - w, 0, w)
        // Return the nearest point
        return a + w * AC;
    }

    // Since point P is inside triangle ABC, calculate it using the coordinates of the barycentric coordinate.
    // P = A + v(B - A) + w(C - A)
    float denom = 1.0f / (pab + pbc + pca);
    float v = pca * denom;
    float w = pab * denom;
    
    // Return barycentric coordinate
    if (result) { 
        float u = 1.0f - v - w;
        *result = Barycentric3f(u, v, w); 
    }
    return a + v * AB + w * AC;
}

// Is the point contained within a rectangle in 2-dimensional coordinate space in the XZ plane?
bool Utl::Calc::IsPointInRectangleXZ(const Vector3f& point, const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector3f& d) {
    float isCross_ab = Utl::Math::CrossVector3In2D(b - a, point - a);
    float isCross_bc = Utl::Math::CrossVector3In2D(c - b, point - b);
    float isCross_cd = Utl::Math::CrossVector3In2D(d - c, point - c);
    float isCross_da = Utl::Math::CrossVector3In2D(a - d, point - d);

    // If all signs are the same, then the point exists inside
    return ((isCross_ab >= 0 && isCross_bc >= 0 && isCross_cd >= 0 && isCross_da >= 0) ||
        (isCross_ab <= 0 && isCross_bc <= 0 && isCross_cd <= 0 && isCross_da <= 0));
}

// Calculate the intersection of line segments in the 2-dimensional coordinate space of the XZ plane
bool Utl::Calc::CalclateIntersectSegmentsXZ(const Vector3f& s1, const Vector3f& v1, const Vector3f& s2, const Vector3f& v2, Vector3f* out1, Vector3f* out2, bool* isOut2) {
    // Create a distance vector between two line segments
    Vector3f v = s2 - s1;
    float cross_v1_v2 = Utl::Math::CrossVector3In2D(v1, v2);
    // If two line segments are parallel
    if (Utl::IsFloatZero(cross_v1_v2)) {
        // Calculate the end points of two line segments
        Vector3f e1 = s1 + v1;
        Vector3f e2 = s2 + v2;

        // Determine if they overlap on the x-axis
        float min1_x = (std::min)(s1.x(), e1.x());
        float max1_x = (std::max)(s1.x(), e1.x());
        float min2_x = (std::min)(s2.x(), e2.x());
        float max2_x = (std::max)(s2.x(), e2.x());
        if (max1_x < min2_x || max2_x < min1_x) {
            // No overlap
            return false;
        }
        // Determine if they overlap on the z-axis
        float min1_z = (std::min)(s1.z(), e1.z());
        float max1_z = (std::max)(s1.z(), e1.z());
        float min2_z = (std::min)(s2.z(), e2.z());
        float max2_z = (std::max)(s2.z(), e2.z());
        if (max1_z < min2_z || max2_z < min1_z) {
            // No overlap
            return false;
        }

        // Calculate the start and end points of the intersection range
        out1->x() = (std::max)(min1_x, min2_x);
        out2->x() = (std::min)(max1_x, max2_x);
        out1->z() = (std::max)(min1_z, min2_z);
        out2->z() = (std::min)(max1_z, max2_z);
        
        // The second intersection existed.
        *isOut2 = true;
        return true;
    }

    float cross_v_v1 = Utl::Math::CrossVector3In2D(v, v1);
    float cross_v_v2 = Utl::Math::CrossVector3In2D(v, v2);

    float t1 = cross_v_v1 / cross_v1_v2;
    float t2 = cross_v_v2 / cross_v1_v2;

    // If t1,t2 are not in the range 0 to 1, they do not intersect.
    if (t1 + FLT_EPSILON < 0 || t1 - FLT_EPSILON > 1 || t2 + FLT_EPSILON < 0 || t2 - FLT_EPSILON > 1) {
        return false;
    }

    // Calculate intersect
    *out1 = s1 + v1 * t1;

    // The second intersection didn't existed.
    *isOut2 = false;

    return true;
}

// Calculate the intersection of a straight line and an infinite cylinder
bool Utl::Calc::CalculateIntersectLineInfiniteCylinder(const Vector3f& L, const Vector3f& V, 
    const Vector3f& cylinder1, const Vector3f& cylinder2, float radius, 
    Vector3f* penetStart, Vector3f* penetEnd) {
    /*
     * Define the point on the straight line as L, and the direction of the straight line as V.
     * Define the radius of infinite cylinder as r, and the points of axis of infinite cyinder as P1, P2.
     * Also, these points P1, P2 are represented in a local coordinate system with the points on the straight line as the origin.
     * There exist two intersections of the infinite cylinder and the straight line. These are defined as Q1, Q2.
     * Q1, Q2 has a circle vertical to the axis of the infinite cylinder. The center coordinates of these are R1, R2.
     * 
     * where the point Q on the straight line is represented, using the coefficient a, as.
     *   [eq.1] Q = aV
     * 
     * In addition, since line segment QR and the axis of the cylinder are vertical, the following relational expression can be calculated.
     *   [eq.2] (Qn - Rn)・(P2 - P1) = 0
     * 
     * Also, since Qn - Rn is the same length as the radius of the infinite cylinder, it is expressed as follows.
     *   [eq.3] r^2 = (Qn - Rn)・(Qn - Rn)
     *            = (Qn - Rn)^2
     * 
     * Since Rn lies on the straight line P1P2, by replacing (P2 - P1) with S and using the coefficient b, it is expressed as follows.
     *   [eq.4] R1 = P1 + b(P2 - P1) = P1 + bS
     *
     * Solve eq.2 and eq.3 to calculate a and b.
     * First, using eq.1 and eq.4, expand eq.2.
     *   0 = (Q1 - R1)・(P2 - P1)
     *     = (Q1 - R1)・S
     *     = (aV - P1 + bS)・S
     *     = a(V・S) - (P1・S) + b(S・S)
     * 
     * Solve for b from this equation.
     *          b(S・S) = a(S・V) - (P・S)
     *   [eq.5] b = (a(S・V) / (S・S) - (P1・S) / (S・S))
     * 
     * Expand eq.3
     *   r^2 = (Q-R)・(Q-R)
     *           (aV - (P1 + bS))・(aV - (P1 + bS))
     *           (aV - P1 - bS)・(aV - P1 - bS)
     *            a^2(V・V) + (P1・P1) + b^2(S・S) - 2a(P1・V) + 2b(P1・S) - 2ab(V・S)
     *            a^2(V・V) + (P1・P1) + b^2(S・S) - 2a((P1・V) + b(V・S)) + 2b(P1・S)
     * 
     * Transfer r^2 to the right hand side.
     * ※ A new line is inserted between each term due to the horizontal spread of the expression.
     *   0 = a^2(V・V)
     *       + (P1・P1)
     *       + b^2(S・S)
     *       - 2a((P1・V) + b(V・S))
     *       + 2b(P1・S)
     *       - r^2
     *
     * Substitute eq.5 into this equation and solve for a
     *   0 = a^2(V・V)
     *       + (P1・P1)
     *       + (S・S)(a(S・V) / (S・S) - (P1・S) / (S・S))^2
     *       - 2a((P1・V) + (a(S・V) / (S・S) + (P1・S) / (S・S))(V・S))
     *       + 2(a(S・V) / (S・S) - (P1・S) / (S・S))(P1・S)
     *       - r^2
     *
     *     = a^2(V・V)
     *       + (P1・P1)
     *       + (S・S)((a(S・V) / (S・S))^2 - 2a((S・V)(P1・S) / (S・S)^2) + ((P1・S) / (S・S))^2)
     *       - 2a((P1・V) + (a(S・V)^2 / (S・S) + (P1・S)(V・S) / (S・S))
     *       + 2(a(S・V)(P1・S) / (S・S) - (P1・S)^2 / (S・S))
     *       - r^2
     *
     *     = a^2(V・V)
     *       + P1・P1
     *       + a^2((S・V)^2 / (S・S)) - 2a((S・V)(P1・S) / (S・S)) + (P1・S)^2 / (S・S)
     *       - 2a(P1・V) - 2a^2((S・V)^2 / (S・S)) - 2a((P1・S)(V・S) / (S・S))
     *       + 2a(S・V)(P1・S) / (S・S) - 2((P1・S)^2 / (S・S))
     *       - r^2
     *
     * Summarize this equation with a^2 terms, 2a terms, and other terms
     *     = a^2(V・V - 2((S・V)^2 / (S・S)) + (S・V)^2 / (S・S))
     *       + 2a((P1・V) - (P1・S)(V・S) / (S・S) - (S・V)(P1・S) / (S・S) + (S・V)(P1・S) / (S・S))
     *       + (P1・P1 + (P1・S)^2 / (S・S) - 2((P1・S)^2 / (S・S)) - r^2)
     *
     *     = a^2(V・V - (S・V)^2 / (S・S))
     *       - 2a((P1・V) - (S・V)(P1・S) / (S・S))
     *       + (P1・P1 - (P1・S)^2 / (S・S) - r^2)
     * Replace the term-by-term values with coefficients A, B, C.
     *   0 = a^2A - 2aB + C
     * 
     * Now that we have the same form as quadratic equation ax^2 + bx + c = 0, , I use the formula to express.
     *   a = (V±√(B^2 - AC)) / A
     */
    
    // Move a point on the central axis of a cylinder to a local coordinate space with a point on a straight line as the origin.
    Vector3f P1 = cylinder1 - L;
    Vector3f P2 = cylinder2 - L;

    // Calculate the vector S
    Vector3f S = P2 - P1;

    // Calculate the dot products needed
    float V_V   = V.Dot(V);
    float S_V   = S.Dot(V);
    float P1_V  = P1.Dot(V);
    float S_S   = S.Dot(S);
    float P1_S  = P1.Dot(S);
    float P1_P1 = P1.Dot(P1);

    // Cylinders don't intersect because they are not oriented in the same direction
    if (S_S <= FLT_EPSILON) {
        return false;
    }

    // Calculate each coefficients
    float invS_S = 1.0f / S_S;
    float A = V_V - S_V * S_V * invS_S;
    float B = P1_V - S_V * P1_S * invS_S;
    float C = P1_P1 - P1_S * P1_S * invS_S - radius * radius;

    // If A is 0, the axis vector of the cylinder and the given vector are parallel
    if (std::abs(A) <= FLT_EPSILON) {
        return false;
    }

    // Calculate scalar values in √
    float sqrtScalar = B * B - A * C;
    // Straight lines and cylinders don't intersect.
    if (sqrtScalar < 0.0f) {
        return false;
    }
    // Calculate square root
    sqrtScalar = sqrtf(sqrtScalar);

    // Start point of penetration
    float invA = 1.0f / A;
    if (penetStart) {
        float a = (B - sqrtScalar) * invA;
        *penetStart = L + V * a;
    }
    // End point of penetration
    if (penetEnd) {
        float a = (B + sqrtScalar) * invA;
        *penetEnd = L + V * a;
    }

    // Intersect
    return true;
}

// Calculate the 8 vertex coordinates that the box has
void Utl::Calc::CalculateBoxVertex8(const Vector3f& center, const Vector3f& sizeHalf, const Matrix3x3f& rotateMat, Vector3f* vertexArray) {
    int index = 0;

    // Calculate the 8 vertex coordinates
    for (float signX : {-1.0f, 1.0f}) {
        for (float signY : {-1.0f, 1.0f}) {
            for (float signZ : {-1.0f, 1.0f}) {
                vertexArray[index] = center +
                    Utl::Math::MultiplyMat3x3AxisToScalar(rotateMat, sizeHalf.x() * signX, Utl::Math::_X) +
                    Utl::Math::MultiplyMat3x3AxisToScalar(rotateMat, sizeHalf.y() * signY, Utl::Math::_Y) +
                    Utl::Math::MultiplyMat3x3AxisToScalar(rotateMat, sizeHalf.z() * signZ, Utl::Math::_Z);
                index++;
            }
        }
    }
}

// Sort the vertices of a square by the angle from the centroid in the XZ plane
void Utl::Calc::SortRectangleVertexCentroidXZ(Vector3f* vertexArray) {
    // Calculate centroids
    float centroidX = 0.0f;
    float centroidZ = 0.0f;
    for (int i = 0; i < 4; ++i) {
        centroidX += vertexArray[i].x();
        centroidZ += vertexArray[i].z();
    }
    centroidX *= Utl::Inv::_4;
    centroidZ *= Utl::Inv::_4;

    // Sort
    std::sort(vertexArray, vertexArray + 4, [&centroidX, &centroidZ](const Vector3f& a, const Vector3f& b) {
        float angleA = std::atan2f(a.z() - centroidZ, a.x() - centroidX);
        float angleB = std::atan2f(b.z() - centroidZ, b.x() - centroidX);
        return angleA < angleB;
    });
}
