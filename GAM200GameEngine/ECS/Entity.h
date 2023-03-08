/*******************************************************************************
filename    Entity.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "../framework.h"
#include "../System/Macro.h"
#include "../Serializer/RapidJsonBase.h"
#include "ComponentManager.h"
#include "IComponent.h"

namespace Engine
{
	namespace ECS
	{
		extern Manager::ComponentManager* compManager;

		class Entity:public Serializer::ISerializer
		{
			EntityID _id;
			std::string _name;
			std::string _tag;
			std::string _layername;
		public:
			std::vector<Components::IComponent*> _attachedComponents;

			Entity() : 
				_id{ 0 }, _name{ "NOT GIVEN" }, _tag{ "NOT TAGGED" }, _layername{ "NO LAYER" }{}
			Entity(EntityID id, std::string name) :
				_id{ id }, _name{ name }, _tag{ "NOT TAGGED" }, _layername{ "NO LAYER" }{}
			Entity(EntityID id, std::string name, std::string tag) :
				_id{ id }, _name{ name }, _tag{ tag }, _layername{ "NO LAYER" }{}
		
			Entity(const Entity&) = default; 
			Entity(Entity&&) = default;
			Entity& operator=(const Entity&) = default;
			Entity& operator=(Entity&&) = default;

			void Rename(std::string name);
			void SetTag(std::string tag);
			void SetLayer(const std::string & layer) { _layername = layer; }
			// Don't call this without permission
			void SetID(EntityID id);
			std::string GetName() const;
			std::string GetTag() const;
			std::string GetLayer() const { return _layername; }
			EntityID GetID() const;
			void AddToAttachedComponentsList(Components::IComponent*&& c)
			{
				_attachedComponents.push_back(c);
			}

			template<typename T>
			void AddComponent(T&& component);

			template<typename T>
			void RemoveComponent();

			bool Deserialize(const rapidjson::Value& obj);
			int GetEntityNoFromList(const rapidjson::Document&);
			bool Deserialize(const rapidjson::Document&, int) { return false; }
			bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const;
		};

		namespace Manager
		{
			class EntityManager:public Serializer::ISerializer
			{
				std::queue<EntityID> _openIDs;
				std::vector<Entity> _EntityList;
				std::vector<Entity>::iterator GetIterator(EntityID id);
				std::vector<EntityID> _forDelete;

			public:
				EntityManager();
				Entity& FindEntity(EntityID id);
				EntityID CreateNewEntity();
				void DestroyEntity(EntityID id);
				void DestroyAllEntities();
				std::vector<Entity>& GetEntityList();
				EntityID GetNextID();
				bool EntityExist(EntityID id);
				void QueueDestroyEntity(EntityID id); 
				void DestroyQueuedEntities();
				void ClearOpenIDs();

				bool Deserialize(const rapidjson::Document&, int id = 0);
				bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const;
			};
		}

		/// <summary>
		/// adds a component to entity
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="component"></param>
		template<typename T>
		void Entity::AddComponent(T&& component)
		{
			compManager->AddComponentToEntity<T>(_id, std::forward<T&&>(component));
			AddToAttachedComponentsList(&compManager->GetEntityComponent<T>(_id));
		}

		template<typename T>
		void Entity::RemoveComponent()
		{
			compManager->RemoveComponentFromEntity<T>(_id);
		}
	}
}