/*******************************************************************************
filename    Factory.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "../framework.h"
#include "../Messaging/Messaging.h"
#include "ECS.h"
#include "Entity.h"
#include "IComponent.h"

#define p_Factory Engine::ECS::Factory::GetInstance()

namespace Engine
{
	namespace ECS
	{
		class Factory
		{
			static std::unique_ptr<Factory> _pFactory;
		public:
			//Factory();
			void DeserializeFile(std::string);
			void SerializeToFile(std::string);
			void DeserializeScene(std::string);
		
			Factory() = default;
			~Factory() = default;

			static std::unique_ptr<Factory> GetInstance()
			{
				if (!_pFactory)
				{
					_pFactory = std::make_unique<Factory>();
				}

				return std::move(_pFactory);
			}

			/// <summary>
			/// Makes a component
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <returns></returns>
			template <typename T>
			static T make()
			{
				static_assert(std::is_base_of<Components::IComponent, T>::value, "Object is not a Component");

				return T();
			}

			static void HandleMessage(Messaging::SerializerMessage msg);
		};

		void InitFactory();
		void QuitFactory();
	}
}