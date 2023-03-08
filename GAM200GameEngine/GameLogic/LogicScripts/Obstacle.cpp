/*******************************************************************************
filename    Obstacle.cpp
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        December 09, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "Obstacle.h"
#include "../../AI/AISystem.h"
#include "../../Physics/Collision.h"
#include "../../ECS/Factory.h"
#include <filesystem>

namespace Engine
{
    Obstacle* Obstacle::_instance = nullptr;

    Obstacle::Obstacle() :
        _isOpened(false),
        _obstacleTag("Door"),
        _pos({0.0f,  0.0f}),
        _vacuumID((EntityID)-1),
        _objThatHitsID((EntityID)-1),
        _timer(2.0f)
    {
    }

    void Obstacle::SetScriptName()
    {
        _scriptName = "Obstacle";
    }
    
    std::string Obstacle::GetScriptName()
    {
        return _scriptName;
    }

    Obstacle* Obstacle::GetInstance()
    {
        /*if (!_instance)
        {*/
            _instance = new Obstacle;
        /*}*/

        return _instance;
    }

    ScriptType* Obstacle::Clone() const
    {
        return new Obstacle{ *this };
    }

    void Obstacle::Init()
    {
        _vacuumID = (EntityID)-1;

        // Finding the vacuum ID
        if (ECS::compManager->CheckForList<VacuumComponent>())
        {
            std::map<EntityID, VacuumComponent>& list
                = ECS::compManager->GetComponentList<VacuumComponent>().GetList();

            for (auto i = list.begin(); i != list.end(); ++i)
            {
                // Vacuum
                if ((ECS::entityManager->FindEntity((*i).first).GetTag() == "Vacuum"))
                {
                    _vacuumID = (*i).first;
                    break;
                }
            }
        }
        
        if (ECS::compManager->CheckForList<TransformComponent>())
            _pos = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner())._position;
        
        // restart open timer
        _timer = 2.0f;
        _isOpened = false;
        _objThatHitsID = (EntityID)-1;
    }

    void Obstacle::Update(float dt)
    {
        // If breakable door is closed
        if (!_isOpened)
        {
            // Loop through AI's vacuumable objects list
            if (ECS::compManager->CheckForList<VacuumableComponent>())
            {
                std::map<EntityID, VacuumableComponent>& list
                    = ECS::compManager->GetComponentList<VacuumableComponent>().GetList();
            
                for (auto i = list.begin(); i != list.end(); ++i)
                {
                    // Check if there is a vacuum component
                    if (ECS::compManager->CheckForList<VacuumComponent>())
                    {
                        if (_vacuumID != -1)
                        {
                            if (!ECS::compManager->CheckForEntity<VacuumComponent>(_vacuumID))
                            {
                                return;
                            }

                            VacuumComponent& vacuum =
                                ECS::compManager->GetEntityComponent<VacuumComponent>(_vacuumID);

                            // Checks if vacuum is holding objects, else no need to check
                            if (i->first != vacuum._holdObjID)
                            {
                                // If true for vacuumable AABB and door AABB
                                if (IsHitByObject(i->first, dt))
                                {
                                    _objThatHitsID = i->first;
                                    // open door
                                    _isOpened = true;
                                    topDoorID = AddTopDoor(i->first);
                                    bottomDoorID = AddBottomDoor(i->first);
                                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _DOORBREAK, "DoorBreak.ogg", false, 0.7f, false, true, false, std::string(""));
                                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            
        }
        // Door is opened
        else
        {
            OpenDoor();
        }
    }

    bool Obstacle::IsHitByObject(EntityID objID, float dt)
    {
        if(ECS::compManager->CheckForEntity<RectColliderComponent>(objID))
             return Collision::AABBToAABB(GetOwner(), objID, dt);
        return false;
    }

    void Obstacle::OpenDoor()
    {
        TransformComponent& doorPOS
            = ECS::compManager->
            GetEntityComponent<TransformComponent>(GetOwner());

        TransformComponent& objPOS
            = ECS::compManager->
            GetEntityComponent<TransformComponent>(_objThatHitsID);
        
        if (topDoorID != (EntityID)-1)
        {
            TransformComponent& topDoorPOS
                = ECS::compManager->GetEntityComponent<TransformComponent>(topDoorID);
            // Object on the right of the door
            if (doorPOS._position._x < objPOS._position._x)
            {
                topDoorPOS._position._y -= 0.4f;
                topDoorPOS._position._x -= 0.4f;
                topDoorPOS._rotate -= 2.0f;
            }
            else
            {
                topDoorPOS._position._y -= 0.4f;
                topDoorPOS._position._x += 0.4f;
                topDoorPOS._rotate += 2.0f;
            }
        }

        if (bottomDoorID != (EntityID)-1)
        {
            TransformComponent& bottomDoorPOS
                = ECS::compManager->
            GetEntityComponent<TransformComponent>(bottomDoorID);

            // Object on the right of the door
            if (doorPOS._position._x < objPOS._position._x)
            {
                bottomDoorPOS._position._y -= 0.2f; // from 0.4 change to 0.2
                bottomDoorPOS._position._x -= 0.2f; // from 0.4 change to 0.2
                bottomDoorPOS._rotate -= 3.0f;  // from 2.0 change to 3.0f
            }
            else
            {
                bottomDoorPOS._position._y -= 0.2f; // from 0.4 change to 0.2
                bottomDoorPOS._position._x += 0.2f; // from 0.4 change to 0.2
                bottomDoorPOS._rotate += 3.0f;  // from 2.0 change to 3.0f
            }
        }

        ECS::compManager->RemoveComponentFromEntity<RectColliderComponent>(GetOwner());
        ECS::compManager->RemoveComponentFromEntity<SpriteComponent>(GetOwner());
        // delete obj?
    }

    EntityID Obstacle::AddTopDoor(EntityID objID)
    {
        UNREFERENCED_PARAMETER(objID);

        TransformComponent& doorPOS
            = ECS::compManager->
            GetEntityComponent<TransformComponent>(GetOwner());

        // Adding new entity to create "split"
        EntityID doorID = ECS::entityManager->CreateNewEntity();
        ECS::Entity door = ECS::entityManager->FindEntity(doorID);
        auto transform = p_Factory->make<TransformComponent>();
        //TransformComponent transform;
        transform._scale = { 1.0F, 6.0F };
        transform._position = { doorPOS._position._x,  doorPOS._position._y + 1.0f };
        auto sprite = p_Factory->make<SpriteComponent>();
        //SpriteComponent sprite;
        sprite._texPath = "Assets/Textures/BrokenDoor_Upper.png";
        std::filesystem::path s_path(sprite._texPath);
        sprite._texName = s_path.filename().string();
        door.AddComponent(std::move(transform));
        door.AddComponent(std::move(sprite));

        return doorID;
    }

    EntityID Obstacle::AddBottomDoor(EntityID objID)
    {
        UNREFERENCED_PARAMETER(objID);
        UNREFERENCED_PARAMETER(objID);

        TransformComponent& doorPOS
            = ECS::compManager->
            GetEntityComponent<TransformComponent>(GetOwner());

        // Adding new entity to create "split"
        EntityID doorID = ECS::entityManager->CreateNewEntity();
        ECS::Entity door = ECS::entityManager->FindEntity(doorID);
        auto transform = p_Factory->make<TransformComponent>();
        //TransformComponent transform;
        transform._scale = { 1.0F, 2.0F };
        transform._position = { doorPOS._position._x,  doorPOS._position._y - 1.0f };
        auto sprite = p_Factory->make<SpriteComponent>();
        //SpriteComponent sprite;
        sprite._texPath = "Assets/Textures/BrokenDoor_Lower.png";
        std::filesystem::path s_path(sprite._texPath);
        sprite._texName = s_path.filename().string();
        door.AddComponent(std::move(transform));
        door.AddComponent(std::move(sprite));

        return doorID;
    }
    
    void Obstacle::End()
    {
        delete _instance;
    }
}
