/*******************************************************************************
filename    Vacuum.cpp
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        November 19, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once
#include "Vacuum.h"
#include "../../Physics/Collision.h"
#include "../../ECS/Components.h"
#include "../GSM/IdleState.h"
#include "../../Input/Input.h"
#include "../../Editor/PE_Editor.h"
#include "../../ECS/ECS.h"

#include "../../GameLogic/Player_State.h"

#include "../../Resource/AssetManager.h"
#include "../../UI/PE_UI.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Engine
{
    namespace AI
    {
        Vacuum::Vacuum() :
            _parentID(NULL),
            _suckForce(),
            _blowForce(),
            _dt(1.0f / 60.0f),
            _isHolding(false),
            _holdObjID(NULL),
            _currentState(VacuumState::OFF),
            _blowFactor(5.0f),
            _isSucking(false),
            _suckObjID(NULL)
        {
        }

        Vacuum::~Vacuum()
        {
        }

        void Vacuum::Init()
        {
            ECS::Entity* tmp = Scripts::GetOwner();

            if (ECS::compManager->CheckForList<VacuumComponent>())
            {
                if (tmp != nullptr)
                {
                    VacuumComponent& vacuum
                        = ECS::compManager->GetEntityComponent<VacuumComponent>(GetOwner()->GetID());

                    RectColliderComponent& vacuumBOX
                        = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner()->GetID());

                    _stateMachine = new StateMachine<Scripts>((Scripts*)tmp);
                    this->SetOwner(tmp);

                    _stateMachine->SetOwner(this);
                    _stateMachine->SetGlobalState(new IdleState());
                    _parentID = vacuum._parent;

                    if (Player::GetInstance())
                    {
                        Player::GetInstance()->_vacuumEntity = GetOwner()->GetID();
                    }
                    
                    _currDir = Direction::NONE;

                    _initialRectHeight = vacuumBOX._height;
                    _initialRectWidth = vacuumBOX._width;

                    _isSucking = false;
                    _isHolding = false;
                    _holdObjID = NULL;
                    _suckObjID = NULL;


                    _blowFactor = 400.0f;
                    // _arcForce = 1500.0f;
                    // no mouse = 300.0f
                    _arcForce = 300.0f;
                    _shootForce = 1400.0f;
                    // Set Vacuum's blow and suck force
                    _blowForce = { 5.0f, 0.0f };
                    _suckForce = { 5.0f, 1.0f };
                }
            }
        }

        void Vacuum::Update(float dt)
        {
            _dt = dt;
            
            if (!ECS::compManager->CheckForEntity<VacuumComponent>(GetOwner()->GetID()))
            {
                return;
            }

            // Update the transform to the parent's transform
            // Check if Vacuum has a parent Transform Component
            // and it's own Transform Component
            if (ECS::compManager->CheckForEntity<TransformComponent>(_parentID)
                && ECS::compManager->CheckForEntity<TransformComponent>(GetOwner()->GetID()))
            {
                TransformComponent& vacPos
                    = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID());
                RectColliderComponent& vacBox
                    = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner()->GetID());

                TransformComponent& parentPos
                    = ECS::compManager->GetEntityComponent<TransformComponent>(_parentID);
                RectColliderComponent& parentBox
                    = ECS::compManager->GetEntityComponent<RectColliderComponent>(_parentID);

                vacBox._direction = parentBox._direction;
                if (vacBox._direction == RectColliderComponent::Direction::LEFT)
                {
                    vacPos._position._x = parentPos._position._x - 0.1f;
                }
                else
                    vacPos._position._x = parentPos._position._x + 0.1f;
                // Adjust position of Vacuum position
                vacPos._position._y = parentPos._position._y - 0.2f;

                // Holding has its own update logic to prevent wall clip
                if (!_isHolding)
                {
                    // Update to Parent's direction
                    if (parentBox._direction == RectColliderComponent::Direction::LEFT)
                    {
                        _currDir = Direction::LEFT;
                        if (vacBox.GetOffSet()._x > 0)
                        {
                            vacBox._offset._x *= -1;
                            ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID())._scale._x *= -1;
                        }
                    }
                    else
                    {
                        _currDir = Direction::RIGHT;
                        if (vacBox.GetOffSet()._x < 0)
                        {
                            vacBox._offset._x *= -1;
                            ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID())._scale._x *= -1;
                        }
                    }
                }
            }

            // If holding, can only suck current obj
            // and blow current object
            if (_isHolding)
            {
                // If enemy dies while being held by vacuum
                // if (ECS::compManager->CheckForEntity<PatrolComponent>(_holdObjID))
                // {
                //     PatrolComponent& objPatrol
                //         = ECS::compManager->GetEntityComponent<PatrolComponent>(_holdObjID);
                //     if (objPatrol._currHP <= 0)
                //     {
                //         if (ECS::compManager->CheckForList<VacuumableComponent>())
                //         {
                //             std::map<EntityID, VacuumableComponent>& list
                //                 = ECS::compManager->GetComponentList<VacuumableComponent>().GetList();
                //             (void)list;
                //             /* for (size_t objID = 0; objID < p_AISystem->_objects.size(); ++objID)
                //              {
                //                  if (_holdObjID == p_AISystem->_objects[objID])
                //                      p_AISystem->_objects.erase(p_AISystem->_objects.begin(), p_AISystem->_objects.begin() + objID);
                //                  _isHolding = false;
                //                  _holdObjID = NULL;
                //              }*/
                //         }
                // 
                //     }
                // }
                Hold(_holdObjID);
            }
            // Redo this to if not holding then loop to check if in range
            if (ECS::compManager->CheckForList<VacuumableComponent>())
            {
                std::map<EntityID, VacuumableComponent>& list
                    = ECS::compManager->GetComponentList<VacuumableComponent>().GetList();

                for (auto i = list.begin(); i != list.end(); ++i)
                {
                    // Loop through all vacuumable objects
                    if (IsInRange(i->first))
                    {
                        switch (_currentState)
                        {
                        case VacuumState::OFF:
                            // spriteIndex = 0;
                            if (_isHolding)
                            {
                                Hold(i->first);
                            }
                            break;
                        case VacuumState::SUCK:
                            // spriteIndex = 2;
                            if (!_isSucking)
                            {
                                if (_suckObjID == NULL)
                                {
                                    EntityID tmp = GetCloserObj();
                                    if (tmp != NULL)
                                    {
                                        _suckObjID = GetCloserObj();
                                        Suck(_suckObjID);
                                    }
                                    else _isSucking = false;
                                }
                            }
                            else Suck(_suckObjID);
                            break;
                        case VacuumState::ISHOLDING:
                            _spriteIndex = 3;
                            Hold(i->first);
                            break;
                        case VacuumState::SHOOT:
                            // spriteIndex = 1;
                            Shoot(i->first);
                            break;
                        case VacuumState::BLOW:
                            // spriteIndex = 1;
                            Blow(i->first);
                            break;
                        default:
                            break;
                        }

                        
                    }
                }
            }

            if (_currentState != VacuumState::ISHOLDING)
            {
                Player::GetInstance()->_isHoldingObj = false;
                Player::GetInstance()->_holdObjID = (EntityID)-1;

            }

            // Update vacuum UI
            compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._texName = "vacuum-" + std::to_string(_spriteIndex) + ".png";
            // std::cout << compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._texName << std::endl;
            UpdateInput();

            if (_currentState != VacuumState::SUCK)
            {
                _isSucking = false;
                _suckObjID = NULL;
            }

            VacuumComponent& vacuum
                = ECS::compManager->GetEntityComponent<VacuumComponent>(GetOwner()->GetID());

            vacuum._vacuumState = _currentState;
            vacuum._isHolding = _isHolding;
            vacuum._parent = _parentID;

            vacuum._holdObjID = _holdObjID;
        }

        int Vacuum::GetHoldObjID()
        {
            return _holdObjID;
        }

        void Vacuum::UpdateInput()
        {
            if (sJeffState == JeffState::j_Idle
                || sJeffState == JeffState::j_Walk)
            {
                ShowVacuum();
                // || p_Input->KeyIsHeld(GLFW_KEY_E))
                if (p_Input->MouseButtonIsPressed(GLFW_MOUSE_BUTTON_LEFT) ||
                    p_Input->MouseButtonIsHeld(GLFW_MOUSE_BUTTON_LEFT)) 
                {
                    if (!_isHolding)
                    {
                       _spriteIndex = 2;
                    }

                    if (_isHolding)
                        _currentState = VacuumState::ISHOLDING;
                    else
                    {
                        _currentState = VacuumState::SUCK;
                        // Play suck sound                  
                        if (ECS::entityManager->FindEntity(0).GetName() == "Player")
                        {
                            Messaging::AudioMessage message(Messaging::PLAY_COMPONENT, _VSUCK,
                                ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_VSUCK), false, 0.5f, false, false, true, std::string(""));
                            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);

                            TrigSuck = true;
                        }
                    }
                }
                // || p_Input->KeyIsHeld(GLFW_KEY_R))
                else if (p_Input->MouseButtonIsPressed(GLFW_MOUSE_BUTTON_RIGHT) ||
                         p_Input->MouseButtonIsHeld(GLFW_MOUSE_BUTTON_RIGHT)) 
                {
                    if (!_isHolding)
                    {
                        _currentState = VacuumState::BLOW;
                        // Play blow sound
                        if (ECS::entityManager->FindEntity(0).GetName() == "Player")
                        {
                            Messaging::AudioMessage message(Messaging::PLAY_COMPONENT, _VBLOW,
                                ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_VBLOW), false, 0.7f, false, false, true, std::string(""));
                            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                        }
                    }
                    else
                    {
                        _currentState = VacuumState::SHOOT;
                        Messaging::AudioMessage message(Messaging::PLAY_SFX, _VACSHOOT,
                            "Shoot.ogg", false, 0.7f, false, true, false, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                    }


                    TrigBlow = true;
                    _spriteIndex = 1;
                }

                // else if (p_Input->MouseButtonIsPressed(GLFW_MOUSE_BUTTON_LEFT))
                // {
                //     if (_isHolding)
                //     {
                //         _currentState = VacuumState::SHOOT;
                //         _mousePos = { p_Editor->_gameWindowMousePosition.x,
                //                    p_Editor->_gameWindowMousePosition.y };
                // 
                //         glm::vec4 clip{ _mousePos._x, _mousePos._y, 0.0f, 1.0f };
                //         glm::vec4 Eye = glm::inverse(p_Editor->scene_camera->GetProjMatrix()) * clip;
                //         glm::vec4 World = glm::inverse(p_Editor->scene_camera->GetViewMatrix()) * Eye;
                // 
                //         _mousePos = World;
                //     }
                // }
                

                else
                {
                    if (_currentState != VacuumState::ISHOLDING)
                    {
                        _currentState = VacuumState::OFF;
                        _spriteIndex = 0;
                    }

                    if (TrigBlow)
                    {
                        Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _VBLOW,
                            ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_VBLOW), false, 0.7f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                        TrigBlow = false;
                    }

                    if (TrigSuck)
                    {
                        Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _VSUCK,
                            ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_VSUCK), false, 0.7f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
                        TrigSuck = false;
                    }
                }
                return;
            }

            else
            {
                HideVacuum();
            }
        }

        void Vacuum::HideVacuum()
        {
            // Set Obj gravity to true
                // no longer holding. Will be affected by gravity
            RigidBodyComponent& objRB
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(_holdObjID);
            objRB.SetIsGravity(true);

            SpriteComponent& s
                = ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID());

            s._renderable = false;

            _isHolding = false;
            _holdObjID = NULL;
            Player::GetInstance()->_isHoldingObj = _isHolding;
            Player::GetInstance()->_holdObjID = _holdObjID;
            _currentState = VacuumState::OFF;
        }

        void Vacuum::ShowVacuum()
        {
            SpriteComponent& s
                = ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID());

            s._renderable = true;
        }

        void Vacuum::Suck(EntityID targetID)
        {
            if (targetID != _suckObjID)
                return;

            _isSucking = true;
            
            // Get Object data
            // TransformComponent& objectPOS
            //     = ECS::compManager->GetEntityComponent<TransformComponent>(targetID);
            RectColliderComponent& objectBOX
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(targetID);
            // Get Vacuum data
            TransformComponent& vacuumPOS
                = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID());
            RectColliderComponent& vacuumBOX
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner()->GetID());

            RigidBodyComponent& objRBC 
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(targetID);

            // Get the vacuum offset (there's direction)
            Vec2D vacBOX = vacuumPOS._position + vacuumBOX.GetOffSet();
            Vec2D objBOX = { objectBOX.GetWidth(), objectBOX.GetHeight() };

            if (ECS::compManager->CheckForEntity<RigidBodyComponent>(targetID))
            {
                if (!IsInVacuum(_parentID, targetID))
                {
                    if (_currDir == Direction::LEFT)
                    {
                        objRBC.AddForce({ _suckForce, 0.5f });
                    }
                    else
                    {
                        objRBC.AddForce({ { -_suckForce._x, _suckForce._y }, 0.5f });
                    }

                    if (_suckObjID == NULL)
                        _suckObjID = targetID;
                }
                else
                {
                    _currentState = VacuumState::ISHOLDING;
                    objRBC.ClearAllForces();
                    objRBC.SetVelocity(0.0f);
                    objRBC.SetAcceleration(0.0f);
                    _suckObjID = NULL;
                    _isSucking = false;
                    Hold(targetID);
                }
            }
        }

        void Vacuum::Hold(EntityID targetID)
        {
            if (_isHolding)
                if (targetID != _holdObjID)
                    return;

            Player::GetInstance()->_isHoldingObj = true;
            Player::GetInstance()->_holdObjID = targetID;

            TransformComponent& objPOS
                = ECS::compManager->GetEntityComponent<TransformComponent>(targetID);
            TransformComponent& vacPOS
                = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID());
            RectColliderComponent& vacBOX
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner()->GetID());
            RectColliderComponent& parentBOX
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(_parentID);
            RectColliderComponent& objBOX
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(targetID);
            // turning
            // update position when player turns
            if ((_currDir == Direction::LEFT) 
                && (parentBOX._direction == RectColliderComponent::Direction::RIGHT))
            {
                // change direction
                _currDir = Direction::RIGHT;
                if (vacBOX.GetOffSet()._x < 0)
                    vacBOX._offset._x *= -1;

                // Hold the object at the vacuum
                float vac = ((vacPOS._position._x + vacBOX.GetWidth() / 2.0f + vacBOX.GetOffSet()._x)
                    - (objBOX.GetWidth() / 2.0f + objBOX.GetOffSet()._x));

                objPOS._position._x = vac;

                objPOS._position._x = vac;
            }

            else if ((_currDir == Direction::RIGHT)
                && (parentBOX._direction == RectColliderComponent::Direction::LEFT))
            {
                // change direction
                _currDir = Direction::LEFT;
                if (vacBOX.GetOffSet()._x > 0)
                    vacBOX._offset._x *= -1;

                // Hold the object at the vacuum
                float vac = ((vacPOS._position._x - vacBOX.GetWidth() / 2.0f + vacBOX.GetOffSet()._x)
                    + (objBOX.GetWidth() / 2.0f + objBOX.GetOffSet()._x));

                objPOS._position._x = vac;
            }

            // same direction
            else
            {
                // If there are no collisionsflag on the obj
                // means not colliding with the wall
                // can move
                // if there is collision, dont update position of the box
                if (_currDir == Direction::LEFT)
                {
                    if (objBOX._collisionBitflag == RectColliderComponent::CollisionBitflag::NONE_ ||
                        objBOX._collisionBitflag == RectColliderComponent::CollisionBitflag::RIGHT)
                    {
                        float vac = ((vacPOS._position._x + vacBOX.GetWidth() / 2.0f + vacBOX.GetOffSet()._x)
                            - (objBOX.GetWidth() / 2.0f + objBOX.GetOffSet()._x));

                        objPOS._position._x = vac;
                    }
                }
                else
                {
                    if (objBOX._collisionBitflag == RectColliderComponent::CollisionBitflag::NONE_ ||
                        objBOX._collisionBitflag == RectColliderComponent::CollisionBitflag::LEFT_)
                    {
                        float vac = ((vacPOS._position._x - vacBOX.GetWidth() / 2.0f + vacBOX.GetOffSet()._x)
                            + (objBOX.GetWidth() / 2.0f + objBOX.GetOffSet()._x));

                        objPOS._position._x = vac;
                    }
                }
            }

            if (_currDir == Direction::RIGHT)
            {
                if (vacPOS._scale._x < 0)
                    vacPOS._scale._x *= -1;
            }
            else
            {
                if (vacPOS._scale._x > 0)
                    vacPOS._scale._x *= -1;
            }

            // Let go of box when touch floor/ platform
            // release when there is a bottom collision
            if (objPOS._position._y == vacPOS._position._y)
            {
                // if there is floor collision
                // dont update the y
                if (objBOX._collisionBitflag == RectColliderComponent::CollisionBitflag::DOWN_)
                {
                    ReleaseHoldObj();
                    return;
                }
                // If object is taller than parent
                else if (objBOX.GetHeight() > parentBOX.GetHeight() - 1.0f)
                {
                    // adjust the y if its touching the floor
                    // find parent's min y
                    TransformComponent& parentPOS
                        = ECS::compManager->GetEntityComponent<TransformComponent>(_parentID);
                    float parentMinY = parentPOS._position._y - parentBOX.GetHeight() / 2.0f;
                    // find obj min y
                    float objMinY = objPOS._position._y - objBOX.GetHeight() / 2.0f;
                    // add the diff between the mins to the position
                    objPOS._position._y += (parentMinY - objMinY) + 0.5f;
                }
                else
                    objPOS._position._y = vacPOS._position._y;
            }
            else
            {
                // If object is taller than parent
                if (objBOX.GetHeight() > parentBOX.GetHeight() - 1.0f)
                {
                    // adjust the y if its touching the floor
                    // find parent's min y
                    TransformComponent& parentPOS
                        = ECS::compManager->GetEntityComponent<TransformComponent>(_parentID);
                    float parentMinY = parentPOS._position._y - parentBOX.GetHeight() / 2.0f;
                    // find obj min y
                    float objMinY = objPOS._position._y - objBOX.GetHeight() / 2.0f;
                    // add the diff between the mins to the position
                    objPOS._position._y += (parentMinY - objMinY) + 0.5f;
                }
                else
                objPOS._position._y = vacPOS._position._y;
            }

            // Set gravity to false
            // else the -y velocity will affect yeeting
            RigidBodyComponent& objRB 
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(targetID);
            objRB.SetIsGravity(false);
            
            _isHolding = true;
            _holdObjID = targetID;
        }

        void Vacuum::ReleaseHoldObj()
        {
            RigidBodyComponent& objRB
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(_holdObjID);
            objRB.SetIsGravity(true);

            Player::GetInstance()->_isHoldingObj = false;
            Player::GetInstance()->_holdObjID = (EntityID)-1;

            _isHolding = false;
            _holdObjID = NULL;

            _currentState = VacuumState::OFF;
        }

        void Vacuum::Shoot(EntityID targetID)
        {
            if (targetID != _holdObjID)
                return;

            if (ECS::compManager->CheckForList<RigidBodyComponent>())
            {
                RigidBodyComponent& objRB
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(targetID);
                // RectColliderComponent& objBOX
                //     = ECS::compManager->GetEntityComponent<RectColliderComponent>(targetID);
                // TransformComponent& parentPOS
                //     = ECS::compManager->GetEntityComponent<TransformComponent>(_parentID);
                // RectColliderComponent& parentBOX
                //     = ECS::compManager->GetEntityComponent<RectColliderComponent>(_parentID);
                objRB.SetTotalForce({ 0.0f, 0.0f });

                if (_currDir == Direction::LEFT)
                {
                    Vec2 arcForce = { 0.0f, _arcForce };
                    Vec2 force = { -_shootForce, 0.0f };
                    // Arc should be only 1 frame so it doesnt yeet upwards
                    objRB.AddForce({ arcForce, 1.0f / 60.0f });
                    objRB.AddForce({ force, 0.1f });

                    // Mouse aim
                    // Shoot according to y mouse position
                    // If left, cannot shoot right
                    // if (_mousePos._x < parentPOS._position._x - parentBOX._width / 2)
                    // {
                    //     Vec2D mag = _mousePos - parentPOS._position;
                    //     mag = mag.MakeNormal();
                    // 
                    //     Vec2 arcForce = { 0.0f, mag._y * _shootForce };
                    //     Vec2 force = { -(_shootForce * std::abs(mag._x)), 0.0f };
                    //     objRB.AddForce({ arcForce,  1.0f / 60.0f });
                    //     float lifetime = 0.1f;
                    //     objRB.AddForce({ force,  lifetime });
                    // }
                }
                else
                {
                    Vec2 arcForce = { 0.0f, _arcForce };
                    Vec2 force = { _shootForce, 0.0f };
                    // Arc should be only 1 frame so it doesnt yeet upwards
                    objRB.AddForce({ arcForce, 1.0f / 60.0f });
                    objRB.AddForce({ force, 0.1f });

                    // Mouse aim
                    // If right, cannot shoot left
                    // if (_mousePos._x > parentPOS._position._x + parentBOX._width / 2)
                    // {
                    //     Vec2D mag = _mousePos - parentPOS._position;
                    //     mag = mag.MakeNormal();
                    //     Vec2 arcForce = { 0.0f, mag._y * _shootForce };
                    //     Vec2 force = { _shootForce * std::abs(mag._x), 0.0f };
                    //     objRB.AddForce({ arcForce,  1.0f / 60.0f });
                    //     float lifetime = 0.1f;
                    //     objRB.AddForce({ force,  lifetime });
                    // }
                }

            }
            _isHolding = false;
            _holdObjID = NULL;

            // Set Obj gravity to true
            // no longer holding. Will be affected by gravity
            RigidBodyComponent& objRB
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(targetID);
            objRB.SetIsGravity(true);
        }

        void Vacuum::Blow(EntityID targetID)
        {
            if (ECS::compManager->CheckForList<RigidBodyComponent>())
            {
                RigidBodyComponent& objRB
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(targetID);

                if (_currDir == Direction::LEFT)
                {
                    objRB.AddForce({ -_blowForce, 0.5f });
                }

                else if (_currDir == Direction::RIGHT)
                {
                    objRB.AddForce({ _blowForce, 0.5f });
                }
            }
            _isHolding = false;
            _holdObjID = NULL;
            RigidBodyComponent& objRB
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(targetID);
            objRB.SetIsGravity(true);
        }

        bool Vacuum::IsInRange(EntityID target)
        {
            if (ECS::compManager->CheckForEntity<RectColliderComponent>(GetOwner()->GetID())
                && ECS::compManager->CheckForEntity<RectColliderComponent>(target))
            {
                return Collision::AABBToAABB(target, GetOwner()->GetID(), _dt);
            }
            return false;
        }

        bool Vacuum::IsInVacuum(EntityID vacuum, EntityID object)
        {
            return Collision::AABBToAABB(vacuum, object, _dt);
        }

        EntityID Vacuum::GetCloserObj()
        {
            EntityID closerToVac = NULL;

            if (ECS::compManager->CheckForList<VacuumableComponent>())
            {
                TransformComponent& vacPOS
                    = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID());
                 
                std::map<EntityID, VacuumableComponent>& list
                    = ECS::compManager->GetComponentList<VacuumableComponent>().GetList();
                
                for (auto i = list.begin(); i != list.end(); ++i)
                {
                    if (closerToVac == NULL)
                    {
                        if(IsInRange(i->first))
                            closerToVac = i->first;
                        continue;
                    }

                    // Check if in vaccum collider
                    if (IsInRange(i->first))
                    {
                        TransformComponent& currCloserToVacPOS
                            = ECS::compManager->GetEntityComponent<TransformComponent>(closerToVac);
                        TransformComponent& POS
                            = ECS::compManager->GetEntityComponent<TransformComponent>(i->first);
                    
                        // Check x axis
                        // Vac.x - pos.x > 0
                        if (_currDir == Direction::LEFT)
                        {
                            float distance1 = vacPOS._position._x - currCloserToVacPOS._position._x;
                            float distance2 = vacPOS._position._x - POS._position._x;

                            if (distance2 < distance1)
                            {
                                closerToVac = i->first;
                            }
                        }
                        else
                        {
                            float distance1 = currCloserToVacPOS._position._x - vacPOS._position._x;
                            float distance2 = POS._position._x - vacPOS._position._x;

                            if (distance2 < distance1)
                            {
                                closerToVac = i->first;
                            }
                        }
                    }

                }
            }
            return closerToVac;
        }
    }
}
