#ifndef _MACRO_H_
#define _MACRO_H_

#include "stdint.h"
#include "../Math/Vector2D.h"
#include "../Math/Vector3D.h"
#include "../Math/Vector4D.h"
#include "../Math/Matrix4x4.h"

namespace Engine
{
	using EntityID = uint16_t;

	const size_t MAX_ENTITIES = 3000;

	using Pt = Math::Point2D;
	using Vec2D = Math::Vector2D;
	using Vec3D = Math::Vector3D;
	using Vec4D = Math::Vector4D;
	using Matx4 = Math::Matrix4x4;
}

#endif // _MACRO_H_