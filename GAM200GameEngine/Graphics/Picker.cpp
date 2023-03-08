/*******************************************************************************
filename    Picker.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, ---% Code Contribution
			Leow Yumi, 440002919, yumi.leow@digipen.edu, ---% Code Contribution
date        September 20, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Picker.h"
#include "../Editor/PE_Editor.h"

namespace Engine
{
	namespace Graphics
	{
		AABB::AABB()
		{

		}

		AABB::AABB(TransformComponent trans)
		{
			min.x = -0.5f * std::abs(trans._scale._x) + trans._position._x;
			min.y = -0.5f * std::abs(trans._scale._y) + trans._position._y;

			max.x = 0.5f - std::abs(trans._scale._x) + trans._position._x;
			max.y = 0.5f * std::abs(trans._scale._y) + trans._position._y;
		}

		int Picker::Pick(Camera* cam, std::multimap<float, TransformComponent*> scene)
		{
			const float x_ndc = p_Editor->_gameWindowMousePosition.x;
			const float y_ndc = p_Editor->_gameWindowMousePosition.y;

			if (abs(x_ndc) > 1.0f || abs(y_ndc) > 1.0f)
				return p_Editor->_pickedObject;

			glm::vec4 clip{ x_ndc, y_ndc, 0.0f, 1.0f };
			glm::vec4 Eye = glm::inverse(cam->GetProjMatrix()) * clip;
			//Eye = glm::vec4(Eye.x, Eye.y, 0.0f, 1.0f);
			glm::vec4 World = glm::inverse(cam->GetViewMatrix()) * Eye;

			// Copying map
			std::map<TransformComponent*, EntityID> tMap;
			for (auto& [id, t] : ECS::compManager->GetComponentList<TransformComponent>().GetList())
				tMap.insert({ &t, id });

			int index = -1;

			// Intersection using bounding box method
			for (std::multimap<float, TransformComponent*>::const_reverse_iterator it = scene.rbegin(); it != scene.rend(); ++it)
			{
				AABB bounding;

				bounding.min.x = -0.5f * std::abs(it->second->_scale._x) + it->second->_position._x;
				bounding.min.y = -0.5f * std::abs(it->second->_scale._y) + it->second->_position._y;

				bounding.max.x = 0.5f * std::abs(it->second->_scale._x) + it->second->_position._x;
				bounding.max.y = 0.5f * std::abs(it->second->_scale._y) + it->second->_position._y;

				// Check for collision with AABB
				if (World.x < bounding.min.x || World.x > bounding.max.x || World.y < bounding.min.y || World.y > bounding.max.y)
					continue;

				index = tMap.at(it->second);

				return index;
			}

			return -1;
		}
	}
}