/* Start Header ************************************************************************/
/*!
\file Vector4D.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Sep 23, 2020
\brief
	This file contains the definition for Vector4D

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef VECTOR4D_H
#define VECTOR4D_H

#include <iostream>

namespace Engine
{
	namespace Math
	{
#ifdef _MSC_VER
		// Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( disable : 4201 )
#endif
		typedef union Vector4D
		{
			struct
			{
				float _x, _y, _z, _w;
			};

			// Constructors
			Vector4D();
			Vector4D(float _x, float _y, float _z, float _w);

			// Assignment operators
			Vector4D& operator+=(const Vector4D& rhs);
			Vector4D& operator-=(const Vector4D& rhs);
			Vector4D& operator*=(float rhs);
			Vector4D& operator/=(float rhs);

			// Unary operators
			Vector4D operator-() const;

			// Comparison operators
			bool operator!=(const Vector4D& rhs);

		} Vector4D, Vec4, Point4D, Pt4;

		// Binary operators
		Vector4D operator+(const Vector4D& lhs, const Vector4D& rhs);
		Vector4D operator-(const Vector4D& lhs, const Vector4D& rhs);
		Vector4D operator*(const Vector4D& lhs, float rhs);
		Vector4D operator*(float lhs, const Vector4D& rhs);
		Vector4D operator/(const Vector4D& lhs, float rhs);
	}
}
#endif // End VECTOR4D_H
