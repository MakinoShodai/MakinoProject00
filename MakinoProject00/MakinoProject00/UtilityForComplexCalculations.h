#ifndef __UTILITY_FOR_COMPLEX_CALCULATIONS_H__
#define __UTILITY_FOR_COMPLEX_CALCULATIONS_H__

#include "UtilityForMath.h"

namespace Utl {
    namespace Calc {
        /**
           @brief Calculate the shortest distance between a plane and a point
           @param plane One coordinate point in the plane
           @param planeNormal Normal of a plane (unit vector)
           @param point Coordinate of the point
           @return Shortest distance
        */
        float CalculateDistancePlanePoint(const Vector3f& plane, const Vector3f& planeNormal, const Vector3f& point);
        /**
           @brief Calculate the shortest distance between line segments
           @param vCenterA2B Vector connecting the center coordinates of two line segments from A to B
           @param dirA Direction vector of line segment A (unit vector)
           @param dirB Direction vector of line segment B (unit vector)
           @param halfLengthA Half length of line segment A
           @param halfLengthB Half length of line segment B
           @param offsetA Variable for return value that receives offset of line segment A
           @param offsetB Variable for return value that receives offset of line segment B
           @param vTieClosestPoint Variable for return value that receives vector connecting the points closest to each other
           @return Shortest distance
        */
        float CalculateDistanceSegments(const Vector3f& vCenterA2B, const Vector3f& dirA, const Vector3f& dirB, float halfLengthA, float halfLengthB,
            Vector3f* offsetA, Vector3f* offsetB, Vector3f* vTieClosestPoint);
        

        /** 
           @brief Calculate the coordinate on the triangle closest to the point
           @param point Coordinate of the point
           @param a First vertex coordinate of the trinagle
           @param b Second vertex coordinate of the trinagle
           @param c Third vertex coordinate of the trinagle
           @param d Fourth vertex coordinate of the trinagle
           @param result Variable for return value that receives barycentric coordinates
           @return Coordinate on the triangle
        */
        Vector3f ClosestTriangle2Point(const Vector3f& point, const Vector3f& a, const Vector3f& b, const Vector3f& c, Barycentric3f* result = nullptr);


        /**
           @brief Is the point contained within a rectangle in 2-dimensional coordinate space in the XZ plane?
           @param point Coordinate of the point
           @param a First vertex coordinate of the rectangle
           @param b Second vertex coordinate of the rectangle
           @param c Third vertex coordinate of the rectangle
           @param d Fourth vertex coordinate of the rectangle
           @return returns true if the point is contained within the rectangle
        */
        bool IsPointInRectangleXZ(const Vector3f& point, const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector3f& d);


        /**
           @brief Calculate the intersection of line segments in the 2-dimensional coordinate space of the XZ plane
           @param s1 Starting point of the first line segment
           @param v1 Direction of the first line segment
           @param s2 Starting point of the second line segment
           @param v2 Direction of the second line segment
           @param out1 Variable for return value that receives the first intersection 
           @param out2 Variable for return value that receives the second intersection 
           @param isOut2 Variable for return value that receives the second intersection exists.
           @return Did the line segments intersect each other?
        */
        bool CalclateIntersectSegmentsXZ(const Vector3f& s1, const Vector3f& v1, const Vector3f& s2, const Vector3f& v2, Vector3f* out1, Vector3f* out2, bool* isOut2);
        /**
           @brief Calculate the intersection of a straight line and an infinite cylinder
           @param L Point on the straight line
           @param V Direction of the straight line
           @param cylinder1 First point on the center axis of the cylinder
           @param cylinder2 Second point on the center axis of the cylinder
           @param radius Radius of the cylinder
           @param penetStart Variable for return value that receives the start point of penetration
           @param penetEnd Variable for return value that receives the end point of penetration
           @return Is it intersected?
        */
        bool CalculateIntersectLineInfiniteCylinder(const Vector3f& L, const Vector3f& V, 
            const Vector3f& cylinder1, const Vector3f& cylinder2, float radius, 
            Vector3f* penetStart, Vector3f* penetEnd);


        /**
           @brief Calculate the 8 vertex coordinates that the box has
           @param center Coordinate of center
           @param sizeHalf Half size of the each axis of the box
           @param rotateMat Rotation matrix of the box
           @param vertexArray Pointer to array to store vertex coordinates. Be sure to send an array of size 8 or larger
           @note To increase ease of use, std::array<Vector3f, 8> is not used
        */
        void CalculateBoxVertex8(const Vector3f& center, const Vector3f& sizeHalf, const Matrix3x3f& rotateMat, Vector3f* vertexArray);
        /**
           @brief Sort the vertices of a square by the angle from the centroid in the XZ plane
           @param vertexArray Pointer to array to vertex coordinates. Be sure to send an array of size 8 or larger
           @note To increase ease of use, std::array<Vector3f, 4> is not used
        */
        void SortRectangleVertexCentroidXZ(Vector3f* vertexArray);
    } // namespace Calc
} // namespace Utl

#endif // !__UTILITY_FOR_COMPLEX_CALCULATIONS_H__
