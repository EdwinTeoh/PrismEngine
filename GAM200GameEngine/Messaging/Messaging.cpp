/*******************************************************************************
filename    Massaging.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Messaging.h"

namespace Engine
{
	namespace Messaging
	{
		MessageManager* msgManager;

		Message::Message(const std::string& msg) :
			EventMSG{ msg } {}

		Message::Message(std::string&& msg) :
			EventMSG()
		{
			std::swap(EventMSG, msg);
		}

		std::string Message::getEvent()
		{
			return EventMSG;
		}

		void MessageManager::UpdateAllSystems()
		{
			_physics.UpdateEvents();
			_ecs.UpdateEvents();
			_serialize.UpdateEvents();
			_scene.UpdateEvents();
			_audio.UpdateEvents();
			_resource.UpdateEvents();
		}

		void MessageManager::ClearAllSystems()
		{
			_physics.ClearAll();
			_ecs.ClearAll();
			_serialize.ClearAll();
			_scene.ClearAll();
			_audio.ClearAll();
			_resource.ClearAll();
		}

		void Init()
		{
			msgManager = new MessageManager();
		}

		void Update()
		{
			msgManager->UpdateAllSystems();
		}

		void Quit()
		{
			msgManager->ClearAllSystems();
			delete msgManager;
			msgManager = nullptr;
		}

	}
}