/* Start Header ************************************************************************/
/*!
\file Vector2D.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Sep 23, 2020
\brief
    This file contains the definition for Vector2D

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef VECTOR2D_H
#define VECTOR2D_H
// #include "Precompiled.h"
#include <iostream>
#include <glm/glm.hpp>

#define EPSILON		0.0001f
#define PI			3.14159265358f

namespace Engine
{
   namespace Math
    {
#ifdef _MSC_VER
        // Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( disable : 4201 )
#endif

        typedef union Vector2D
        {
            struct
            {
                float _x, _y;
            };

            // Constructor
            Vector2D();
            Vector2D(float x, float y);
            Vector2D(const glm::vec3& rhs);
            Vector2D(float);

            // Converting Vector2D to GlmVec3
            glm::vec3 ConvertVec2DtoGlmVec3();

            // Vector Addition
            Vector2D& operator+=(const Vector2D& rhs);
            // Vector Subtraction
            Vector2D& operator-=(const Vector2D& rhs);
            // Scalar Multiplication
            Vector2D& operator*=(float value);
            // Scalar Division
            Vector2D& operator/=(float value);

            // Negative x, y ,z
            Vector2D operator-() const;

            // Rotate
            void V2Rotate(float angle);

            // Make normal vector2D
            Vector2D& MakeNormal();
            // Get normal vector2D
            Vector2D GetNormal() const;

            // Comparing Vector2D to Vector2D
            bool operator!=(const Vector2D& rhs);
            // Comparing Vector2D to float
            bool operator!=(float rhs);
            bool operator==(float rhs);

            Vector2D& operator=(const glm::vec3& rhs);
            float Magnitude();

            // operator <<
            friend std::ostream& operator<<(std::ostream& os, const Vector2D& rhs);


        } Vector2D, Vec2D, Vec2, Point2D, Pt2;

        // Binary operators
        Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs);
        Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs);
        Vector2D operator*(const Vector2D& lhs, float rhs);
        Vector2D operator*(float lhs, const Vector2D& rhs);
        Vector2D operator/(const Vector2D& lhs, float rhs);

        // pResult will be the unit vector of pVec0
        void V2Normalize(Vector2D& pResult, Vector2D& pVec0);

        // Returns the length of the vector pVec0
        float V2Length(const Vector2D& pVec0);

        // Returns the square of pVec0 length. Avoid square root.
        float V2SqLength(const Vector2D& pVec0);

        // Takes pVec0 and pVec1 as 2D points and returns the distance between the two points.
        float V2Distance(const Point2D& pVec0, const Point2D& pVec1);

        // Takes pVec0 and pVec1 as 2D points and returns the squared distance between the two points.
        float V2SqDistance(const Point2D& pVec0, const Point2D& pVec1);

        // Returns the dot product between pVec0 and pVec1
        float V2Dot(const Vec2& pVec0, const Vec2& pVec1);

        // Returns the cross product magnitude between pVec0 and pVec1
        float V2CrossProductMag(const Vec2& pVec0, const Vec2& pVec1);

        // Set pResult to (x, y)
        void V2Set(Vec2* pResult, float _x, float _y);

        // Sets pResult to zero
        void V2Zero(Vec2* pResult);

        // Take pVec0 and scale by float. Returns pResult
        void V2Scale(Vec2* pResult, Vec2* pVec0, float s);

        // Take pVec0 - pVec1 and set result to pResult
        void V2Sub(Vec2* pResult, Vec2* pVec0, Vec2* pVec1);
    }

}

#endif  // VECTOR2D_H