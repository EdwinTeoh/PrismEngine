/* Start Header ************************************************************************/
/*!
\file Vector4D.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Sep 23, 2020
\brief
	This file contains the implementation for Vector4D

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Vector4D.h"

namespace Engine
{
	namespace Math
	{
	// Constructor
	Vector4D::Vector4D()
		:_x{ 0.0f }, _y{ 0.0f }, _z{ 0.0f }, _w{ 0.0f }
	{
	}

	Vector4D::Vector4D(float _x, float _y, float _z, float _w)
		: _x{ _x }, _y{ _y }, _z{ _z }, _w{ _w }
	{
	}

	// Vector Addition
	Vector4D& Vector4D::operator+=(const Vector4D& rhs)
	{
		_x += rhs._x;
		_y += rhs._y;
		_z += rhs._z;
		_w += rhs._w;

		return *this;
	}

	// Vector Subtraction
	Vector4D& Vector4D::operator-=(const Vector4D& rhs)
	{
		_x -= rhs._x;
		_y -= rhs._y;
		_z -= rhs._z;
		_w -= rhs._w;

		return *this;
	}

	// Vector Scale
	Vector4D& Vector4D::operator*=(float rhs)
	{
		_x *= rhs;
		_y *= rhs;
		_z *= rhs;
		_w *= rhs;
		return *this;
	}

	// Vector Division
	Vector4D& Vector4D::operator/=(float rhs)
	{
		_x /= rhs;
		_y /= rhs;
		_z /= rhs;
		_w /= rhs;

		return *this;
	}

	// Comparision Operator
	bool Vector4D::operator!=(const Vector4D& rhs)
	{
		return (_x != rhs._x || _y != rhs._y || _z != rhs._z || _w != rhs._w);
	}

	// Negate the points 
	Vector4D Vector4D::operator-() const
	{
		return Vector4D(-_x, -_y, -_z, _w);
	}

	// Binary Vector Addition
	Vector4D operator+(const Vector4D& lhs, const Vector4D& rhs)
	{
		return Vector4D(lhs._x + rhs._x, lhs._y + rhs._y, lhs._z + rhs._z, lhs._w + rhs._w);
	}

	// Binary Vector Subtraction
	Vector4D operator-(const Vector4D& lhs, const Vector4D& rhs)
	{
		return Vector4D(lhs._x - rhs._x, lhs._y - rhs._y, lhs._z + rhs._z, lhs._w - rhs._w);
	}

	// Binary Vector Scale
	Vector4D operator*(const Vector4D& lhs, float rhs)
	{
		return Vector4D(lhs._x * rhs, lhs._y * rhs, lhs._z * rhs, lhs._w);
	}

	// Binary Vector Scale
	Vector4D operator*(float lhs, const Vector4D& rhs)
	{
		return Vector4D(lhs * rhs._x, lhs * rhs._y, lhs * rhs._z, rhs._w);
	}

	// Binary Vector Division
	Vector4D operator/(const Vector4D& lhs, float rhs)
	{
		return Vector4D(lhs._x / rhs, lhs._y / rhs, lhs._z / rhs, lhs._w);
	}
	}
}