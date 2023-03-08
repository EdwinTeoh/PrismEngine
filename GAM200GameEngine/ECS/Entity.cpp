/*******************************************************************************
filename    Entity.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "ECS.h"
#include "Entity.h"
#include "IComponent.h"
#include "../System/Macro.h"
#include "ECS.h"

namespace Engine
{
	namespace ECS
	{
		/// <summary>
		/// Setter function for entity name
		/// </summary>
		/// <param name="name"></param>
		void Entity::Rename(std::string name)
		{
			_name = name;
		}

		/// <summary>
		/// Setter function for entity tag
		/// </summary>
		/// <param name="tag"></param>
		void Entity::SetTag(std::string tag)
		{
			_tag = tag;
		}

		/// <summary>
		/// setter function for entity ID
		/// </summary>
		/// <param name="id"></param>
		void Entity::SetID(EntityID id)
		{
			_id = id;
		}

		/// <summary>
		/// Getter function for entity name
		/// </summary>
		/// <returns></returns>
		std::string Entity::GetName() const
		{
			return _name;
		}


		std::string Entity::GetTag() const
		{
			return _tag;
		}

		EntityID Entity::GetID() const
		{
			return _id;
		}

		bool Entity::Deserialize(const rapidjson::Value& obj)
		{
			SetID((EntityID)obj["ID"].GetInt());
			Rename(obj["Name"].GetString());
			SetTag(obj["Tag"].GetString());
			SetLayer(obj["LayerName"].GetString());
			std::cout << "EntityID: " << _id << " successfully deserialized "
				<< std::endl;
			return true;
		}

		int Entity::GetEntityNoFromList(const rapidjson::Document& doc)
		{
			int index = 0;

			if (doc.HasMember("EntityManager") && doc["EntityManager"].IsArray())
			{
				for (rapidjson::SizeType i = 0; i < doc["EntityManager"].Size(); ++i)
				{
					const rapidjson::Value& em = doc["EntityManager"][i];
					if (em["ID"].GetInt() == _id)
					{
						index = i;
						break;
					}
				}
			}

			return index;
		}
		bool Entity::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
		{

			writer->StartObject();

			writer->String("ID");
			writer->Int(_id);

			writer->String("Name");
			writer->String(_name.c_str());

			writer->String("Tag");
			writer->String(_tag.c_str());

			writer->String("LayerName");
			writer->String(_layername.c_str());

			writer->EndObject();
	
			return true;
		}


		namespace Manager
		{
			/// <summary>
			/// default ctor for EntityManager
			/// </summary>
			EntityManager::EntityManager()
			{
				EntityID id = 0;
				for (size_t i = 0; i < MAX_ENTITIES; ++i)
				{
					_openIDs.push(id++);
				}
			}

			/// <summary>
			/// Iterator for entitylist
			/// </summary>
			/// <param name="id"></param>
			/// <returns></returns>
			std::vector<Entity>::iterator EntityManager::GetIterator(EntityID id)
			{
				std::vector<Entity>::iterator end = _EntityList.end();
				for (std::vector<Entity>::iterator it = _EntityList.begin(); it != end; ++it)
				{
					if ((*it).GetID() == id)
					{
						return it;
					}
				}
				// ASSERT
				return end;
			}

			/// <summary>
			/// Finds a specific Entity
			/// </summary>
			/// <param name="id"></param>
			/// <returns></returns>
			Entity& EntityManager::FindEntity(EntityID id)
			{
				if (GetIterator(id) != _EntityList.end())
					return *(GetIterator(id));
				else
					throw std::out_of_range("Entity does not exist");
			}

			/// <summary>
			/// Creates a new Entity
			/// </summary>
			/// <returns></returns>
			EntityID EntityManager::CreateNewEntity()
			{
				if (_openIDs.size() == 0)
					throw std::exception("Too Many IDs");

				EntityID id = _openIDs.front();
				_openIDs.pop();
				std::string name("Entity ");
				_EntityList.push_back(Entity(id, name + std::to_string(id)));
				return id;
			}

			/// <summary>
			/// Destruction of Entity by ID
			/// </summary>
			/// <param name="id"></param>
			void EntityManager::DestroyEntity(EntityID id)
			{
				try
				{
					compManager->RemoveAllComponentFromEntity(id);
					_openIDs.push(id);
					_EntityList.erase(GetIterator(id));
				}
				catch (...)
				{
				//Logging here "something gone wrong"
					//Logging here "something gone wrong"
				}
			}

			/// <summary>
			/// Destruction of all Entities
			/// </summary>
			void EntityManager::DestroyAllEntities()
			{
				while (_EntityList.size())
					DestroyEntity(_EntityList[0].GetID());
			}

			/// <summary>
			/// Getter function for a vector of EntityManager
			/// </summary>
			/// <returns></returns>
			std::vector<Entity>& EntityManager::GetEntityList()
			{
				return _EntityList;
			}

			/// <summary>
			/// Get next available ID for an entity
			/// </summary>
			/// <returns></returns>
			EntityID EntityManager::GetNextID()
			{
				return _openIDs.front();
			}

			/// <summary>
			/// Check if entity with specified ID exists
			/// </summary>
			/// <param name="id"></param>
			/// <returns></returns>
			bool EntityManager::EntityExist(EntityID id)
			{
				for (auto& it : _EntityList)
				{
					if (it.GetID() == id)
						return true;
				}
				return false;
			}

			void EntityManager::QueueDestroyEntity(EntityID id)
			{
				_forDelete.push_back(id);
			}

			void EntityManager::DestroyQueuedEntities()
			{
				for (auto it : _forDelete)
				{
					for (auto ids : _EntityList)
					{
						if (ids.GetID() == it)
							this->DestroyEntity(it);
					}
				}
				_forDelete.clear();
			}

			void EntityManager::ClearOpenIDs()
			{
				std::queue<EntityID> empty;
				std::swap(_openIDs, empty);

				EntityID id = 0;
				for (size_t i = 0; i < MAX_ENTITIES; ++i)
				{
					_openIDs.push(id++);
				}
			}

			bool EntityManager::Deserialize(const rapidjson::Document& doc, int id)
			{
				UNREFERENCED_PARAMETER(id);

				if (doc.HasMember("EntityManager") && doc["EntityManager"].IsArray())
				{
					for (rapidjson::SizeType i = 0; i < doc["EntityManager"].Size(); ++i)
					{
						
						const rapidjson::Value& em = doc["EntityManager"][i];
						ECS::entityManager->FindEntity(CreateNewEntity()).Deserialize(em);
						//_EntityList.push_back(e);
					}
				}
				else
				{
					std::cout << "EntityManager: no input for it!"
						<< std::endl;
				}


				return true;
			}


			bool EntityManager::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
			{

				writer->Key("EntityManager");

				writer->StartArray();

				for (std::vector<Entity>::const_iterator it = _EntityList.begin(); it != _EntityList.end(); it++)
				{
					(*it).Serialize(writer);
				}
				writer->EndArray();

				return true;
			}
		}
	}
}