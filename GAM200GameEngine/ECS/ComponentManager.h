/*******************************************************************************
filename    ComponentManager.h
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

namespace Engine
{
	namespace ECS
	{
		class IComponentList
		{
		public:
			virtual void RemoveFromEntity(EntityID id) = 0;
			virtual ~IComponentList() = default;
		};

		template <typename T>
		class ComponentList : public IComponentList
		{
			//Expensive, need to optimize
			//Recommanded to change
			std::map<EntityID, T> _components;

		public:
			ComponentList() = default;
			ComponentList(const ComponentList&) = delete;
			ComponentList(ComponentList&&) = delete;
			ComponentList& operator=(ComponentList&&) = default;
			ComponentList& operator=(const ComponentList&) = delete;
			virtual ~ComponentList() = default;

			/// <summary>
			/// Remove component through ID from the entity
			/// </summary>
			/// <param name="id"></param>
			void RemoveFromEntity(EntityID id)
			{
				_components.erase(id);
			}

			/// <summary>
			/// Adds entity to a componentList
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <param name="id"></param>
			/// <param name="comp"></param>
			template<typename T>
			void AddToEntity(EntityID id, T&& comp)
			{
				//if (_components.find(id) != _components.end())
				//{
				//	throw std::exception("Component already exists within this entity");
				//}
				_components[id] = std::forward<T&&>(comp);
			}

			/// <summary>
			/// Getter function for component ID
			/// </summary>
			/// <param name="id"></param>
			/// <returns></returns>
			T& GetComponent(EntityID id)
			{
				return _components[id];
			}

			/// <summary>
			/// Boolean check for empty component list
			/// </summary>
			/// <returns></returns>
			bool ListIsEmpty()
			{
				return _components.empty();
			}

			/// <summary>
			/// Gets the map for the componentlist
			/// </summary>
			/// <returns></returns>
			std::map<EntityID, T>& GetList()
			{
				return _components;
			}
		};

		namespace Manager
		{
			class ComponentManager
			{
				std::map<std::string, std::shared_ptr<IComponentList>> _componentLists;

			public:

				/// <summary>
				/// Check if ComponentList Exist
				/// </summary>
				/// <typeparam name="T"></typeparam>
				/// <returns></returns>
				template<typename T>
				bool CheckForList()
				{
					std::string component = typeid(T).name();
					return (_componentLists.find(component) != _componentLists.end());
				}

				/// <summary>
				/// Check if entity exists in a ComponentList
				/// </summary>
				/// <typeparam name="T"></typeparam>
				/// <param name="id"></param>
				/// <returns></returns>
				template<typename T>
				bool CheckForEntity(EntityID id)
				{
					std::string component = typeid(T).name();
					if (CheckForList<T>())
					{
						auto& list = GetComponentList<T>().GetList();
						return (list.find(id) != list.end());
					}
					return false;
				}

				/// <summary>
				/// Getter Function for a ComponentList
				/// </summary>
				/// <typeparam name="T"></typeparam>
				/// <returns></returns>
				template<typename T>
				ComponentList<T>& GetComponentList()
				{
					std::string component = typeid(T).name();
					if (!CheckForList<T>())
					{
						throw std::exception("Component not registered");
					}

					return *(std::static_pointer_cast<ComponentList<T>>(_componentLists[component]));
				}

				template <typename T>
				ComponentList<T>& GetComponentListF()
				{
					return *(std::static_pointer_cast<ComponentList<T>>(_componentLists[typeid(T).name()]));
				}

				/// <summary>
				/// Getter function for a component in an entity
				/// </summary>
				/// <param name="id"></param> 
				/// <returns></returns>
				template<typename T>
				T& GetEntityComponent(const EntityID& id)
				{
					if (!CheckForEntity<T>(id))
						throw std::exception("Entity does not have specified component");
					return GetComponentList<T>().GetComponent(id);
				}

				/// <summary>
				/// Adds a component to entity in a componentlist
				/// </summary>
				/// <typeparam name="T"></typeparam>
				/// <param name="id"></param>
				/// <param name="comp"></param>
				template <typename T>
				void AddComponentToEntity(EntityID id, T&& comp)
				{
					std::string component = typeid(T).name();
					if (!CheckForList<T>())
					{
						_componentLists[component] = std::make_shared<ComponentList<T>>();
					}
					auto listptr = std::static_pointer_cast<ComponentList<T>>(_componentLists[component]);
					listptr->AddToEntity(id, std::forward<T&&>(comp));
				}

				/// <summary>
				/// Removes component from entity
				/// Removes entity from componentlist
				/// Delete if list is empty
				/// </summary>
				/// <typeparam name="T"></typeparam>
				/// <param name="id"></param>
				template<typename T>
				void RemoveComponentFromEntity(EntityID id)
				{
					GetComponentList<T>().RemoveFromEntity(id);
					if (GetComponentList<T>().ListIsEmpty())
					{
						_componentLists.erase(typeid(T).name());
					}
				}

				/// <summary>
				/// Remove all components from entity
				/// </summary>
				/// <param name="id"></param>
				void RemoveAllComponentFromEntity(EntityID id)
				{
					for (auto& it : _componentLists)
					{
						it.second->RemoveFromEntity(id);
					}
				}

				/// <summary>
				/// Returns the map of componentManager
				/// </summary>
				/// <returns></returns>
				const std::map<std::string, std::shared_ptr<IComponentList>>& GetManagerList()
				{
					return _componentLists;
				}

				~ComponentManager()
				{
					_componentLists.clear();
				}
			};
		}
	}
}