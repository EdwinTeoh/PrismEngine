/*******************************************************************************
filename    Picker.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, ---% Code Contribution
			Leow Yumi, 440002919, yumi.leow@digipen.edu, ---% Code Contribution
date        September 20, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "Camera.h"

namespace Engine
{
	namespace Graphics
	{
		struct AABB
		{
			glm::vec2 min;
			glm::vec2 max;

			// Default constructor 
			AABB();

			// Coversion constructor using transform component
			AABB(TransformComponent trans);
		};

		struct Picker
		{
			static int Pick(Camera* cam, std::multimap<float, TransformComponent*> scene);
		};
	}
}