/*******************************************************************************
filename    ECS.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "../framework.h"
#include "Entity.h"
#include "Factory.h"

namespace Engine
{
	namespace ECS
	{
		extern Manager::ComponentManager* compManager;
		extern Manager::EntityManager* entityManager;

		// Global Functions
		void Init(const std::string& path = "NULL");
		void Update();
		void Quit();
	}
}