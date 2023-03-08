/*******************************************************************************
filename    Factory.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "../Serializer/RapidJsonBase.h"
#include "Factory.h"
#include "Entity.h"
#include "Components.h"
#include "../Clock/Clock.h"
#include "../GameLogic/GameLogic.h"
#include <filesystem>

namespace Engine
{
	namespace ECS
	{
		using namespace rapidjson;
		std::unique_ptr<Factory> Factory::_pFactory;

		/// <summary>
		/// Give a path for serialization.
		/// </summary>
		/// <param name="path"></param>
		//Factory::Factory()
		//{
		//	
		//}

		/// <summary>
		/// creates an entity from data saved in a file
		/// </summary>
		/// <param name="filepath"></param>
		void Factory::DeserializeFile(std::string filepath)
		{
			//time_t start = p_Clock.GetSysClockNow();

			std::ifstream ifs(filepath);
			if (!ifs.is_open())
			{
				std::cout << "Could not open or find scene file!" << std::endl;
				return;
			}
			if (ifs.peek() == std::ifstream::traits_type::eof())
			{
				std::cout << "File is empty!" << std::endl;
				return;
			}

			IStreamWrapper isw(ifs);
			Document document;
			if (document.ParseStream(isw).HasParseError())
			{
				std::cout << "Serialiser parse error!" << std::endl;
				return;
			}

			auto transform = make<TransformComponent>();
			auto rigidBody2d = make<RigidBodyComponent>();
			auto rectCollider = make<RectColliderComponent>();
			auto sprite = make<SpriteComponent>();
			auto circleColid = make<CircleColliderComponent>();
			auto patrol = make<PatrolComponent>();
			auto vacuum = make<VacuumComponent>();
			auto platform = make<PlatformComponent>();
			auto obstacle = make<ObstacleComponent>();
			auto attack = make<AttackComponent>();
			auto vacuumable = make<VacuumableComponent>();
			auto audio = make<AudioComponent>();
			auto particle = make<ParticleComponent>();

			std::cout << "\nLoading from [" << filepath << "]" << std::endl;

			if (entityManager->Deserialize(document))
			{
				std::cout << "EntityManager: " << "successfully deserialized.\n" << std::endl;
			}

			for (Entity& e : entityManager->GetEntityList())
			{
				EntityID entity_id = e.GetID();

				std::cout << "Loading Entity " << entity_id << "..." << std::endl;

				if (transform.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(transform));
					transform.PrintDeserializeSuccessMessage();
				}

				if (rigidBody2d.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(rigidBody2d));
					rigidBody2d.PrintDeserializeSuccessMessage();
				}

				if (sprite.Deserialize(document, entity_id))
				{
					std::filesystem::path s_path(sprite._texPath);
					sprite._texName = s_path.filename().string();
					e.AddComponent(std::move(sprite));
					sprite.PrintDeserializeSuccessMessage();
				}

				if (rectCollider.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(rectCollider));
					rectCollider.PrintDeserializeSuccessMessage();
				}

				if (patrol.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(patrol));
					patrol.PrintDeserializeSuccessMessage();
				}

				if (vacuum.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(vacuum));
					vacuum.PrintDeserializeSuccessMessage();
				}

				if (platform.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(platform));
					platform.PrintDeserializeSuccessMessage();
				}

				if (circleColid.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(circleColid));
					circleColid.PrintDeserializeSuccessMessage();
				}

				if (obstacle.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(obstacle));
					obstacle.PrintDeserializeSuccessMessage();
				}

				if (attack.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(attack));
					attack.PrintDeserializeSuccessMessage();
				}

				if (vacuumable.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(vacuumable));
					vacuumable.PrintDeserializeSuccessMessage();
				}

				if (audio.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(audio));
					audio.PrintDeserializeSuccessMessage();
				}

				if (particle.Deserialize(document, entity_id))
				{
					e.AddComponent(std::move(particle));
					particle.PrintDeserializeSuccessMessage();
				}
			}

			ifs.close();

			std::cout << "\n Loaded from [" << filepath << "]" << std::endl;

			//std::cout << "Deserializing took approximately " << (p_Clock.GetSysClockNow() - start) << " seconds." << std::endl;
		}

		void Factory::SerializeToFile(std::string filepath)
		{
			//time_t start = p_Clock.GetSysClockNow();

			StringBuffer buffer;
			PrettyWriter<StringBuffer> writer(buffer);
			writer.SetFormatOptions(kFormatSingleLineArray);

			writer.StartObject(); // Global start

			size_t newID = 0;

			// Individual Entities Start
			for (auto& e : entityManager->GetEntityList())
			{
				writer.Key(std::to_string(newID++).c_str());
				writer.StartObject();

				// Would be much better to use inheritance for this
				if (compManager->CheckForEntity<TransformComponent>(e.GetID()))
					compManager->GetEntityComponent<TransformComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<SpriteComponent>(e.GetID()))
					compManager->GetEntityComponent<SpriteComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<RigidBodyComponent>(e.GetID()))
					compManager->GetEntityComponent<RigidBodyComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<RectColliderComponent>(e.GetID()))
					compManager->GetEntityComponent<RectColliderComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<LineColliderComponent>(e.GetID()))
					compManager->GetEntityComponent<LineColliderComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<CircleColliderComponent>(e.GetID()))
					compManager->GetEntityComponent<CircleColliderComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<PatrolComponent>(e.GetID()))
					compManager->GetEntityComponent<PatrolComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<VacuumComponent>(e.GetID()))
					compManager->GetEntityComponent<VacuumComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<ObstacleComponent>(e.GetID()))
					compManager->GetEntityComponent<ObstacleComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<PlatformComponent>(e.GetID()))
					compManager->GetEntityComponent<PlatformComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<AttackComponent>(e.GetID()))
					compManager->GetEntityComponent<AttackComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<VacuumableComponent>(e.GetID()))
					compManager->GetEntityComponent<VacuumableComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<AudioComponent>(e.GetID()))
					compManager->GetEntityComponent<AudioComponent>(e.GetID()).Serialize(&writer);

				if (compManager->CheckForEntity<ParticleComponent>(e.GetID()))
					compManager->GetEntityComponent<ParticleComponent>(e.GetID()).Serialize(&writer);

				writer.EndObject();
			}
			// Individual Entities End

			writer.SetFormatOptions(kFormatDefault);

			// EntityManager Start
			writer.Key("EntityManager");
			writer.StartArray();
			newID = 0;
			for (auto& e : entityManager->GetEntityList())
			{
				writer.StartObject();
				writer.KeyAndValue("ID", (int)newID);
				if (e.GetName().find("Entity ") != std::string::npos)
				{
					e.Rename("Entity " + std::to_string(newID));
				}
				writer.KeyAndValue("Name", e.GetName().c_str());
				writer.KeyAndValue("Tag", e.GetTag().c_str());
				writer.KeyAndValue("LayerName", e.GetLayer().c_str());
				writer.EndObject();

				++newID;
			}
			writer.EndArray();
			// EntityManager End

			writer.EndObject(); // Global end

			std::ofstream ofs(filepath);
			ofs << buffer.GetString();
			ofs.close();

			std::cout << "\nSaved to [" << filepath << "]" << std::endl;

			//std::cout << "Serializing took approximately " << (p_Clock.GetSysClockNow() - start) << " seconds." << std::endl;
		}

		void Factory::DeserializeScene(std::string filepath)
		{
			//Have to stop all updates at this point of time before doing this..
			//IF NOT EXCEPTION WILL, AND IT WILL BE THROWN DEFINITELY.
			
			for (Entity e : entityManager->GetEntityList())
			{
				compManager->RemoveAllComponentFromEntity(e.GetID());
			}
			entityManager->DestroyAllEntities();
			entityManager->ClearOpenIDs();
			DeserializeFile(filepath);
		}

	///// <summary>
	///// initialize factory
	///// </summary>
	///// <param name="path"></param>

	void Factory::HandleMessage(Messaging::SerializerMessage msg)
	{
		switch (msg.EventType)
		{
		case Messaging::FACTORIZE:
			p_Factory->SerializeToFile(msg.filepath);
			break;
		case Messaging::DEFACTORIZE:
			p_Factory->DeserializeFile(msg.filepath);
			break;
		case Messaging::DESERIALIZE_SCENE:
			std::cout << "Deserializing..." << std::endl;
			p_Factory->DeserializeScene(msg.filepath);
			GameLogic::GetInstance()->ReInit(); // THIS IS NECESSARY TO BE AFTER DESERIALIZING
			p_AISystem->ReInit();
			std::cout << "Deserializing complete." << std::endl;
			break;
		default:
			break;
		}
	}
	
	void InitFactory()
	{
		Messaging::msgManager->GetSystem<Messaging::SerializerMessage>().AddReceiver(std::function<void(Messaging::SerializerMessage)>(Factory::HandleMessage));
	}

	///// <summary>
	///// Deletes the factory to exit ( prevents leak )
	///// </summary>
	//void QuitFactory()
	//{
	//	Factory::pFactory.reset();
	//}
	
	}
}