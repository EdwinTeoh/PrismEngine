/* Start Header ************************************************************************/
/*!
\file Vector3D.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Sep 23, 2020
\brief
    This file contains the definition for Vector3D

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath> // abs, sqrt
#include <cassert> // assert
#include <algorithm> // max, min
#ifdef _MSC_VER
        // Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( disable : 4201 )
#endif

namespace Engine
{
    namespace Math
    {
        typedef union Vector3D
        {
            struct
            {
                float _x, _y, _z;
            };

            // Constructor
            Vector3D();
            Vector3D(float uX, float uY, float uZ);

            // Vector Addition
            Vector3D operator+(const Vector3D& rhs);
            Vector3D& operator+=(const Vector3D& rhs);
            // Vector Subtraction
            Vector3D operator-(const Vector3D& rhs);
            Vector3D& operator-=(const Vector3D& rhs);
            // Scalar Multiplication
            Vector3D operator*(float value);
            Vector3D& operator*=(float value);
            // Scalar Division
            Vector3D operator/(float value);
            Vector3D& operator/=(float value);

            // Negative x, y ,z
            Vector3D operator-() const;

            // Comparing Vector3D to Vector3D
            bool operator !=(const Vector3D& rhs);

            // Vector3D functions
            // magnitude square function for Normalisation
            float MagSq() const;
            void Normalise();

            // Returns z coordinate - Not used now as only 2D
            // float GetZ() const;
        } Vector3D, Vec3D, Vec3, Point3D, Pt3;

        // pResult will be the unit vector of pVec0
        void V3Normalize(Vec3& pResult, Vec3& pVec0);

        // Returns the length of the vector pVec0
        float V3Length(const Vec3& pVec0);

        // Returns the square of pVec0 length. Avoid square root.
        float V3SqLength(const Vec3& pVec0);

        // Takes pVec0 and pVec1 as 2D points and returns the distance between the two points.
        float V3Distance(const Pt3& pVec0, const Pt3& pVec1);

        // Takes pVec0 and pVec1 as 2D points and returns the squared distance between the two points.
        float V3SqDistance(const Pt3& pVec0, const Pt3& pVec1);

        // Returns the dot product between pVec0 and pVec1
        float V3Dot(const Vec3& pVec0, const Vec3& pVec1);

        // Returns the cross product between pVec0 and pVec1
        Vector3D V3CrossProduct(const Vec3& pVec0, const Vec3& pVec1);

        // Set pResult to (x, y)
        void V3Set(Vec3* pResult, float _x, float _y, float _z);

        // Sets pResult to zero
        void V3Zero(Vec3* pResult);

        // Take pVec0 and scale by float. Returns pResult
        void V3Scale(Vec3* pResult, Vec3* pVec0, float s);

        // Take pVec0 - pVec1 and set result to pResult
        void V3Sub(Vec3* pResult, Vec3* pVec0, Vec3* pVec1);

    }
}
#endif  // VECTOR3D_H