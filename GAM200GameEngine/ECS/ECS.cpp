/*******************************************************************************
filename    ECS.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "ECS.h"
#include "Factory.h"
#include "ComponentManager.h"
#include "Entity.h"

namespace Engine
{
	namespace ECS
	{
		Manager::ComponentManager* compManager;
		Manager::EntityManager* entityManager;

		void Init(const std::string& path)
		{
			UNREFERENCED_PARAMETER(path);
			compManager = new Manager::ComponentManager();
			entityManager = new Manager::EntityManager();
			InitFactory();
		}
		
		/// <summary>
		/// This update function is to destroy the unwanted entity
		/// </summary>
		void Update()
		{
			entityManager->DestroyQueuedEntities();
		}

		void Quit()
		{
			delete compManager;
			delete entityManager;
		//	QuitFactory();
		}

	}
}