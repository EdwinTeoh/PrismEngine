/* Start Header ************************************************************************/
/*!
\file Vector3D.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Sep 23, 2020
\brief
    This file contains the implementation for Vector3D

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Vector3D.h"

namespace Engine
{
    namespace Math
    {
        // Constructor
        Vector3D::Vector3D()
            : _x(0.0f), _y(0.0f), _z(0.0f)
        {
        }
        Vector3D::Vector3D(float uX, float uY, float uZ)
            : _x(uX), _y(uY), _z(uZ)
        {
        }

        // Vector Addition
        Vector3D Vector3D::operator+(const Vector3D& rhs)
        {
            return Vector3D(_x + rhs._x, _y + rhs._y, _z + rhs._z);
        }
        Vector3D& Vector3D::operator+=(const Vector3D& rhs)
        {
            _x = _x + rhs._x;
            _y = _y + rhs._y;
            _z = _z + rhs._z;

            return *this;
        }

        // Vector Substraction
        Vector3D Vector3D::operator-(const Vector3D& rhs)
        {
            return Vector3D(_x - rhs._x, _y - rhs._y, _z - rhs._z);
        }
        Vector3D& Vector3D::operator-=(const Vector3D& rhs)
        {
            _x = _x - rhs._x;
            _y = _y - rhs._y;
            _z = _z - rhs._z;

            return *this;
        }

        // Scalar Multiplication
        Vector3D Vector3D::operator*(float value)
        {
            return Vector3D(_x * value, _y * value, _z * value);
        }
        Vector3D& Vector3D::operator*=(float value)
        {
            _x = _x * value;
            _y = _y * value;
            _z = _z * value;

            return *this;
        }

        //scalar division
        Vector3D Vector3D::operator/(float value)
        {
            assert(value != 0);
            return Vector3D(_x / value, _y / value, _z / value);
        }
        Vector3D& Vector3D::operator/=(float value)
        {
            assert(value != 0);
            _x /= value;
            _y /= value;
            _z /= value;
            return *this;
        }

        // Negative x, y ,z
        Vector3D Vector3D::operator-() const
        {
            return Vector3D(-_x, -_y, -_z);
        }

        // Comparing Vector3D to Vector3D
        bool Vector3D::operator !=(const Vector3D& rhs)
        {
            return (_x != rhs._x || _y != rhs._y || _z != rhs._z);
        }

        // Vector3D functions
        // magnitude square function for Normalisation
        float Vector3D::MagSq() const
        {
            return _x * _x + _y * _y + _z * _z;
        }

        // Normalise the vector
        void Vector3D::Normalise()
        {
            _x /= _x / sqrt(MagSq());
            _y /= _y / sqrt(MagSq());
            _z /= _z / sqrt(MagSq());
        }

        // pResult will be the unit vector of pVec0
        void V3Normalize(Vec3& pResult, Vec3& pVec0)
        {
            float n = sqrt(pVec0._x * pVec0._x + pVec0._y * pVec0._y + pVec0._z * pVec0._z);
            pResult._x = pVec0._x / n;
            pResult._y = pVec0._y / n;
            pResult._z = pVec0._z / n;
        }

        // Returns the length of the vector pVec0
        float V3Length(const Vec3& pVec0)
        {
            return sqrt(pVec0._x * pVec0._x + pVec0._y * pVec0._y + pVec0._z * pVec0._z);
        }

        // Returns the square of pVec0 length. Avoid square root.
        float V3SqLength(const Vec3& pVec0)
        {
            return pVec0._x * pVec0._x + pVec0._y * pVec0._y + pVec0._z * pVec0._z;
        }

        // Takes pVec0 and pVec1 as 2D points and returns the distance between the two points.
        float V3Distance(const Pt3& pVec0, const Pt3& pVec1)
        {
            float tmpx = pVec0._x - pVec1._x;
            float tmpy = pVec0._y - pVec1._y;
            float tmpz = pVec0._z - pVec1._z;

            return sqrt(tmpx * tmpx + tmpy * tmpy + tmpz * tmpz);
        }

        // Takes pVec0 and pVec1 as 2D points and returns the squared distance between the two points.
        float V3SqDistance(const Pt3& pVec0, const Pt3& pVec1)
        {
            float tmpx = pVec0._x - pVec1._x;
            float tmpy = pVec0._y - pVec1._y;
            float tmpz = pVec0._z - pVec1._z;

            return tmpx * tmpx + tmpy * tmpy + tmpz * tmpz;
        }

        // Returns the dot product between pVec0 and pVec1
        float V3Dot(const Vec3& pVec0, const Vec3& pVec1)
        {
            return pVec0._x * pVec1._x + pVec0._y * pVec1._y + pVec0._z * pVec1._z;
        }

        // Returns the cross product between pVec0 and pVec1
        Vector3D V3CrossProduct(const Vec3& pVec0, const Vec3& pVec1)
        {
            Vector3D result;
            result._x = pVec0._y * pVec1._z - pVec0._z * pVec1._y;
            result._y = -(pVec0._x * pVec1._z - pVec0._z * pVec1._x);
            result._z = pVec0._x * pVec1._y - pVec0._y * pVec1._x;

            return result;
        }

        // Set pResult to (x, y)
        void V3Set(Vec3* pResult, float _x, float _y, float _z)
        {
            pResult->_x = _x;
            pResult->_y = _y;
            pResult->_z = _z;
        }

        // Sets pResult to zero
        void V3Zero(Vec3* pResult)
        {
            pResult->_x = 0.0f;
            pResult->_y = 0.0f;
            pResult->_z = 0.0f;
        }

        // Take pVec0 and scale by float. Returns pResult
        void V3Scale(Vec3* pResult, Vec3* pVec0, float s)
        {
            *pResult = *pVec0 * s;
        }

        // Take pVec0 - pVec1 and set result to pResult
        void V3Sub(Vec3* pResult, Vec3* pVec0, Vec3* pVec1)
        {
            *pResult = *pVec0 - *pVec1;
        }

    }
}