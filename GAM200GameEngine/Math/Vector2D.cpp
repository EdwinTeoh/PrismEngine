/* Start Header ************************************************************************/
/*!
\file Vector2D.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Sep 23, 2020
\brief 
    This file contains the implementation for Vector2D

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Vector2D.h"

namespace Engine
{
    namespace Math
    {
        // Constructor
        Vector2D::Vector2D()
            :_x{ 0.0f }, _y{ 0.0f }
        {
        }

        Vector2D::Vector2D(float x, float y)
            : _x{ x }, _y{ y }
        {

        }

        Vector2D::Vector2D(const glm::vec3& rhs)
            : _x{ rhs.x }, _y{ rhs.y }
        {

        }

        Vector2D::Vector2D(float x)
            : _x{ x }, _y{ x }
        {

        }

        glm::vec3 Vector2D::ConvertVec2DtoGlmVec3()
        {
            return glm::vec3{ _x, _y, 0.0f };
        }

        // Vector Addition
        Vector2D& Vector2D::operator+=(const Vector2D& rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        // Vector Substraction
        Vector2D& Vector2D::operator-=(const Vector2D& rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        // Scalar Multiplication
        Vector2D& Vector2D::operator*=(float rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        //scalar division
        Vector2D& Vector2D::operator/=(float rhs)
        {
            if (rhs)
            {
                this->_x /= rhs;
                this->_y /= rhs;
            }
            return *this;
        }

        bool Vector2D::operator!=(float rhs)
        {
            return (_x != rhs && _y != rhs);
        }

        // Comparing Vector2D to Vector2D
        bool Vector2D::operator!=(const Vector2D& rhs)
        {
            return (_x != rhs._x || _y != rhs._y);
        }

        bool Vector2D::operator==(float rhs)
        {
            return (_x == rhs && _y == rhs);
        }
        // Convert glm to Vector2D
        Vector2D& Vector2D::operator=(const glm::vec3& rhs)
        {
            _x = rhs.x;
            _y = rhs.y;

            return *this;
        }

        Vector2D Vector2D::operator-() const
        {
            return Vector2D(-_x, -_y);
        }

        // Rotate Vec2D
        void Vector2D::V2Rotate(float angle)
        {
            float angleRad = angle * PI / 180.f; // Converting it to Radians
            float temp = _x;
            // rotate
            _x = cos(angleRad) * _x - sin(angleRad) * _y;
            _y = cos(angleRad) * _y + sin(angleRad) * temp;
        }

        // Get normal vector2D
        Vector2D Vector2D::GetNormal() const
        {
            return *this / V2Length(*this);
        }

        float Vector2D::Magnitude()
        {
            return sqrtf((this->_x * this->_x) + (this->_y * this->_y));
        }

        // Make normal vector2D
        Vector2D& Vector2D::MakeNormal()
        {
            return *this = this->GetNormal();
        }

        // operator+
        Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs)
        {
            return Vector2D(lhs._x + rhs._x, lhs._y + rhs._y);
        }

        // operator-
        Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs)
        {
            return Vector2D(lhs._x - rhs._x, lhs._y - rhs._y);
        }

        // operator*
        Vector2D operator*(const Vector2D& lhs, float rhs)
        {
            return Vector2D(lhs._x * rhs, lhs._y * rhs);
        }

        // operator*
        Vector2D operator*(float lhs, const Vector2D& rhs)
        {
            return rhs * lhs;
        }

        // operator/
        Vector2D operator/(const Vector2D& lhs, float rhs)
        {
            //if (rhs)
            return Vector2D(lhs._x / rhs, lhs._y / rhs);

        }

        // Vector2DNormalize
        void V2Normalize(Vector2D& pResult, const Vector2D& pVec0)
        {
            pResult = pVec0;
            if (V2Length(pVec0))
            {
                pResult /= V2Length(pVec0);
            }
        }

        // Vector2DLength
        float V2Length(const Vector2D& pVec0)
        {
            return sqrtf(V2SqLength(pVec0));
        }

        // Vector2DSquareLength
        float V2SqLength(const Vector2D& pVec0)
        {
            return V2Dot(pVec0, pVec0);
        }

        // Vector2DDistance
        float V2Distance(const Point2D& pVec0, const Point2D& pVec1)
        {
            return V2Length(pVec0 - pVec1);
        }

        // Vector2DSquareDistance
        float V2SqDistance(const Point2D& pVec0, const Point2D& pVec1)
        {
            return V2SqLength(pVec0 - pVec1);
        }

        // Vector2DDotProduct
        float V2Dot(const Vec2& pVec0, const Vec2& pVec1)
        {
            return pVec0._x * pVec1._x + pVec0._y * pVec1._y;
        }

        // Vector2DCrossProductMag
        float V2CrossProductMag(const Vec2& pVec0, const Vec2& pVec1)
        {
            return pVec0._x * pVec1._y - pVec0._y * pVec1._x;
        }

        // Sets x and y and stores result in pResult
        void V2Set(Vec2* pResult, float x, float y)
        {
            pResult->_x = x;
            pResult->_y = y;
        }

        // Sets pResult to 0
        void V2Zero(Vec2* pResult)
        {
            pResult->_x = 0;
            pResult->_y = 0;
        }

        // Scales pVec0 by s and stores result in pResult
        void V2Scale(Vec2* pResult, Vec2* pVec0, float s)
        {
            *pResult = *pVec0 * s;
        }

        // Subtract 2 Vec2D and stores the result in pResult
        void V2Sub(Vec2* pResult, Vec2* pVec0, Vec2* pVec1)
        {
            *pResult = *pVec0 - *pVec1;
        }

        // operator <<
        std::ostream& operator<<(std::ostream& os, const Vector2D& rhs)
        {
            return os << "(" << rhs._x << ", " << rhs._y << ")";
        }
    }
}