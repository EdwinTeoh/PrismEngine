/*******************************************************************************
filename   GameLogic.cpp
author(s)   Zhang Xinyu,xinyu.z@digipen.edu
date        November 25, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Player.h"
#include "../../Graphics/Camera.h"
#include "../../Sound/SoundEnum.h"
#include "../LogicScripts/Player.h"
#include "../../Editor/PE_Editor.h"
#include "../../UI/PE_UI.h"
#include "../../AI/AISystem.h"

#include <GLFW/glfw3.h>

#define CoolDownTime 2.0f;

using namespace Engine::ECS;

namespace Engine
{
    Player* Player::_instance = nullptr;
    Vec2D Player::CameraTriggerPoints::_nextCameraPosition;
    float Player::CameraTriggerPoints::_nextZoom;
    float _transtime = 2.0f;
    float _deathtime = 3.0f;

    Player::Player() :
        _climb(false),
        _crouchState{ false },
        _crawl(false),
        _CollideWithWall{ false },
        _isHurt(false),
        _timer(2.0f),
        _playerPositionY{ 0.0 },
        _defaultGravity(1.0f)
    {
        //SetOwner(0);
    }

    Player::~Player()
    {

    }

    void Player::SetScriptName()
    {
        _scriptName = "Player";
    }
    std::string Player::GetScriptName()
    {
        return _scriptName;
    }

    void Player::OpenDoor()
    {
        for (auto [id, pos] : _doorCoord)
        {
            float& newPos = compManager->GetEntityComponent<TransformComponent>(id)._position._y;
            float speed = compManager->GetEntityComponent<PlatformComponent>(id)._speed;
            float dist = compManager->GetEntityComponent<PlatformComponent>(id).getDistance()._y + pos;
            if (newPos < dist)
                newPos += speed;
            else if (newPos >= dist)
                newPos = dist;
        }
    }
    void Player::BackToIdle(float countdownSpeed)
    {
        _timer -= 1.0f / 60.0f * countdownSpeed;
        if (_timer <= 0)
        {
            _timer = 2.0f;
            ChangeState_Jeff(JeffState::j_Idle);
        }
    }

    bool Player::CheckInteractable(float dt)
    {
        EntityID id = GetOwner();
        std::map<EntityID, RectColliderComponent>& list
            = compManager->GetComponentList<RectColliderComponent>().GetList();

        bool HealthStationInUse = false;

        for (auto& i : list)
        {
            auto& e_id = i.first;

            if (entityManager->FindEntity(e_id).GetTag() == "Spike")
            {
                if (Collision::AABBToAABB(id, e_id, dt))
                {
                    ChangeState_Jeff(JeffState::j_Dead);
                }
            }
            else if (entityManager->FindEntity(e_id).GetTag() == "SpikeBall")
            {
                EntityID _spikeID = _spawnObjs.find("Spike")->second;
                EntityID _spawnID = _spawnObjs.find("SpawnSpikeBall")->second;
                if (Collision::AABBToAABB(id, e_id, dt))
                {
                    if (sJeffState != JeffState::j_Hurt)
                    {
                        if (!_isImmune)
                        {
                            auto damage = compManager->GetEntityComponent<AttackComponent>(e_id)._damage;
                            _currHP -= damage;
                            _isImmune = true;
                            ChangeState_Jeff(JeffState::j_Hurt);
                        }

                    }
                }
                if (Collision::AABBToAABB(_spikeID, e_id, dt))
                {
                    compManager->GetEntityComponent<SpriteComponent>(e_id)._renderable = false;
                    compManager->GetEntityComponent<TransformComponent>(e_id)._position =
                        compManager->GetEntityComponent<TransformComponent>(_spawnID)._position;
                    break;
                }


                if (compManager->GetEntityComponent<SpriteComponent>(e_id)._renderable == false)
                {
                    compManager->GetEntityComponent<SpriteComponent>(e_id)._renderable = true;
                    compManager->GetEntityComponent<TransformComponent>(e_id)._position =
                        compManager->GetEntityComponent<TransformComponent>(_spawnID)._position;
                    break;
                }

                if (sJeffState == JeffState::j_Hurt)
                {
                    BackToIdle();
                }
            }


            if (entityManager->FindEntity(e_id).GetTag() == "Checkpoint")
            {
                _checkpoint = e_id;
                if (Collision::AABBToAABB(id, e_id, dt))
                {
                    //once touches mark
                    ChangeState_Mark(MarkState::m_CELEBRATE);
                    //p_Editor->_isPaused = true;
                    Messaging::SceneMessage message(Messaging::SET_CHANGE, "", "", true, "");
                    Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message);

                    std::string currentName = Scene::SceneManager::sceneManager->GetCurrentSceneName();
                    std::string nextScene;

                    if (currentName == "Level-1.1")
                        nextScene = "Level-2.1.json";
                    else if (currentName == "Level-2.3")
                    {
                        nextScene = "Level-3.1.json";
                        Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _ELEVATORMOVE, "ElevatorMoveUD.ogg", false, 1.5f, false, true, false, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
                        Messaging::AudioMessage message2(Messaging::STOP_LOOPED_SOUND, _ELEVATORMOVE, "ElevatorMoveLR.ogg", false, 1.5f, false, true, false, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message2);
                    }
                    else if (currentName == "Level-3.3")
                    {
                        Scene::SceneManager::sceneManager->SetSceneStatus(true);
                        nextScene = "Outro.json";
                    }

                    //compManager->GetEntityComponent<TransformComponent>(id)._position._x = compManager->GetEntityComponent<TransformComponent>(id)._position._x + 10.0f;


                    _transtime -= Utility::Clock::GetInstance().GetGDT();

                    if (_transtime <= 0)
                    {
                        if (nextScene == "Menu.json")
                        {
                            Messaging::AudioMessage message3(Messaging::STOP_ALL_AUDIO, _NONE,
                                "", false, 0.7f, false, false, false, std::string(""));
                            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message3);

                            Messaging::SceneMessage message4(Messaging::CHANGE_SCENE, nextScene, "", false, "");
                            Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message4);
                            _transtime = 2.0f;
                        }
                        else
                        {
                            GameLogic::GetInstance()->SetEndState(1, nextScene);
                            _transtime = 2.0f;
                        }

                      
                    }

                    //open the blue door
                    OpenDoor();
                    return false;
                }
            }
            else if (entityManager->FindEntity(e_id).GetTag() == "Shock Platform")
            {
                if (Collision::AABBToAABB(id, e_id, dt))
                {
                    if (sJeffState != JeffState::j_Shock && !_isImmune)
                    {
                        _isImmune = true;
                        _immuneTimer = 0.0f;
                        float direction = compManager->GetEntityComponent<TransformComponent>(id)._scale._x / abs(compManager->GetEntityComponent<TransformComponent>(id)._scale._x);
                        compManager->GetEntityComponent<RigidBodyComponent>(id).SetGravityScale(_defaultGravity);
                        compManager->GetEntityComponent<RigidBodyComponent>(id).SetVelocity(Vec2());
                        compManager->GetEntityComponent<RigidBodyComponent>(id).AddForce(Force({ -2500.0f * direction, 1500.0f }, 1.0f / 60));
                        _currHP -= compManager->GetEntityComponent<AttackComponent>(e_id)._damage;

                        Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _CRAWL,
                            ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_CRAWL), false, 0.2f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);

                        Messaging::AudioMessage message2(Messaging::STOP_LOOPED_SOUND, _WALK2,
                            compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message2);

                        Messaging::AudioMessage message3(Messaging::STOP_LOOPED_SOUND, _FLY,
                            compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_FLY), false, 1.5f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message3);

                        Messaging::AudioMessage message(Messaging::PLAY_SFX, _ZAP, "Zap.ogg", false, 0.5f, false, true, false, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                        ChangeState_Jeff(JeffState::j_Shock);
                    }
                }
                if (sJeffState == JeffState::j_Hurt)
                    BackToIdle();
            }
            else if (entityManager->FindEntity(e_id).GetTag() == "RedDoor")
            {
                if (Collision::AABBToAABB(id, e_id, dt))
                {
                    //trigger the win condition 
                    GameLogic::GetInstance()->SetEndState(1, "WIN");
                }
            }
            else if (entityManager->FindEntity(e_id).GetTag() == "Trap")
            {
                if (Collision::AABBToAABB(id, e_id, dt))
                {
                    auto& rb = ECS::compManager->GetEntityComponent<RigidBodyComponent>(GetOwner());
                    auto& rc = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner());
                    //auto& t_trap = ECS::compManager->GetEntityComponent<TransformComponent>(e_id);
                    auto& t_player = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner());
                    //int direction = static_cast<int>((t_trap._position._x - t_player._position._x) / abs(t_trap._position._x - t_player._position._x));
                    int direction = static_cast<int>(t_player._scale._x / abs(t_player._scale._x));

                    if (sJeffState != JeffState::j_Squish && sJeffState != JeffState::j_Unsquish
                        && (rc._collisionBitflag & RectColliderComponent::CollisionBitflag::UP___))
                    {
                        rb.AddForce(Vec2(1500.0f * -direction, 50.0f));
                        ChangeState_Jeff(JeffState::j_Squish);
                        _isImmune = true;
                        _immuneTimer = 0.0f;
                    }

                    if (sJeffState == JeffState::j_Float)
                    {
                        rb.AddForce(Vec2(1500.0f * -direction, 50.0f));
                        ChangeState_Jeff(JeffState::j_Squish);
                        ECS::compManager->GetEntityComponent<RigidBodyComponent>(GetOwner()).SetGravityScale(_defaultGravity);
                    }
                        
                }
            }
            else if (entityManager->FindEntity(e_id).GetTag() == "ClimbPath")
            {
                CheckClimbing(dt, e_id);
            }
            else if (entityManager->FindEntity(e_id).GetTag() == "FallCollider")
            {
                if (Collision::AABBToAABB(id, e_id, dt))
                {
                    ChangeState_Jeff(JeffState::j_Fall);
                }
            }
            else if (entityManager->FindEntity(e_id).GetTag() == "Health Station")
            {
                
                auto& emitter = ECS::compManager->GetEntityComponent<ParticleComponent>(e_id);
                if (IsAbove(GetOwner(), e_id)) //restore to full health
                {
                    auto& s = ECS::compManager->GetEntityComponent<SpriteComponent>(e_id);

                    if (_currHP == _maxHP)
                    {
                        emitter.isActive = false;
                        if (!Animator::_animatorCont[s._animatorIndex].GetCurrentClip()->Compare(AnimClip::_clipCont[1]))
                        {
                            Animator::_animatorCont[s._animatorIndex].SetCurrentClip(AnimClip::_clipCont[1]);
                            HealthStationInUse = false;
                        }
                        continue;
                    }
                    else
                    {
                        emitter.isActive = true;

                        _timer -= dt;

                        if (_timer <= 1.0f)
                        {
                            _currHP += 10;
                             //std::cout << _currHP << " timer: " << _timer <<   std::endl;
                            _timer = 2.0f;
                        }

                        if (!Animator::_animatorCont[s._animatorIndex].GetCurrentClip()->Compare(AnimClip::_clipCont[2]))
                        {
                            Animator::_animatorCont[s._animatorIndex].SetCurrentClip(AnimClip::_clipCont[2]);
                            HealthStationInUse = true;
                            continue;
                        }
                    }
                }
                // Not standing above the health station
                else 
                {
                    if (HealthStationInUse)
                        continue;

                    auto& s = ECS::compManager->GetEntityComponent<SpriteComponent>(e_id);
                    Animator::_animatorCont[s._animatorIndex].SetCurrentClip(AnimClip::_clipCont[1]);
                    HealthStationInUse = false;
                    emitter.isActive = false;
                }
            }
        }
        return true;
    }

    void Player::CheckClimbing(float dt, EntityID obj)
    {
        EntityID id = GetOwner();
    
        if (Collision::AABBToAABB(id, obj, dt))
        {
            _climb = true;//for applying force
            climbPathID = obj;
        }
    }

    void Player::Init()
    {
        TransformComponent& t = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner());
        RectColliderComponent& rc = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner());
        RigidBodyComponent& rb = ECS::compManager->GetEntityComponent<RigidBodyComponent>(GetOwner());

        _currHP = _maxHP;
        _QPressed = false;
        _stillClimbing = false;
        _playerPositionY = t._position._y;
        _isImmune = false;
        _immuneTimer = 0.0f;

        _mass = rb.GetMass();
        rb.SetGravityScale(5.0f);
        _defaultGravity = rb.GetGravityScale();
        _movementSpeed = rb.GetMovementSpeed();
        _endState = 0;
        UpdateWallMap();
        UpdateDoorCoord();
        if (!_spawnObjs.empty())
            _spawnObjs.clear();
        std::map<EntityID, RectColliderComponent>& list
            = compManager->GetComponentList<RectColliderComponent>().GetList();

        UpdateHp();

        _backCollider = -1;
        for (auto i = list.begin(); i != list.end(); ++i)
        {
            if (entityManager->FindEntity((*i).first).GetTag() == "BACK")
            {
                _backCollider = (*i).first;
            }
            if (entityManager->FindEntity((*i).first).GetTag() == "Checkpoint")
            {
                _checkpoint = (*i).first;
                ChangeState_Mark(MarkState::m_IDLE);
            }
            if (entityManager->FindEntity((*i).first).GetTag() == "Spike")
                _spawnObjs.insert({"Spike",(*i).first });
            if (entityManager->FindEntity((*i).first).GetTag() == "SpawnSpikeBall")
                _spawnObjs.insert({ "SpawnSpikeBall",(*i).first });
            if (entityManager->FindEntity((*i).first).GetTag() == "SpawnExplosiveBarrel")
                _spawnObjs.insert({"SpawnExplosiveBarrel" ,(*i).first });
        }

        /*******************************************************************************
        * Hard-coded values to ensure consistency between scenes
        *******************************************************************************/
       // t._scale = { 3.8f, 5.0f };
        _colliderScale[0] = { 2.1f, 4.2f, 0.0f, 0.0f };
        _colliderScale[1] = { 3.7f, 1.7f, 0.1f, -1.0f };
        _colliderScale[2] = { 2.1f, 1.5f, 0.0f, -1.1f };
        rc.SetSizeAndOffset(_colliderScale[0]);

        p_Camera->SetPosition(t.getPos()._x, t.getPos()._y);
        ChangeState_Jeff(JeffState::j_Idle);

        for (auto& point : cameraTriggerPoints)
        {
            point._triggered = false;
        }

        p_Camera->SetZoom(16.0f);

        if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Level-1.1")
        {
            p_Camera->SetPosition(t._position._x - 2.0f, t._position._y);
            p_Camera->SetZoom(80.0f);

            cameraTriggerPoints.clear();
            cameraTriggerPoints.push_back(CameraTriggerPoints(-1000, 10, -13, -3, 16));
            cameraTriggerPoints.push_back(CameraTriggerPoints(-3, 10, 16, -8, 16));
            cameraTriggerPoints.push_back(CameraTriggerPoints(29, 10, 51, -17, 2));
            cameraTriggerPoints.push_back(CameraTriggerPoints(68, 10, 86, -14, 16));
            cameraTriggerPoints.push_back(CameraTriggerPoints(89, 0, 82, 25.5, 16));
            cameraTriggerPoints.push_back(CameraTriggerPoints(90, 0, 82, 25.5, 16)); // Must have one extra
            //cameraTriggerPoints.push_back(CameraTriggerPoints(66, 20, 60, 26, 16));
            //cameraTriggerPoints.push_back(CameraTriggerPoints(47, 20, 35, 26, 16));

            CameraTriggerPoints::_nextCameraPosition = { p_Camera->GetPosition().x, p_Camera->GetPosition().y };
            CameraTriggerPoints::_nextZoom = p_Camera->GetZoom();
        }
        else if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Level-2.1")
        {
            p_Camera->SetPosition(t._position._x, t._position._y);
            p_Camera->SetZoom(80.0f);

            cameraTriggerPoints.clear();
            cameraTriggerPoints.push_back(CameraTriggerPoints(-1000, 0, 0, -2, 6));
            cameraTriggerPoints.push_back(CameraTriggerPoints(21, -4, 45, -2, 2));
            cameraTriggerPoints.push_back(CameraTriggerPoints(60, 0, 76, 0, 16));
            cameraTriggerPoints.push_back(CameraTriggerPoints(61, 0, 76, 0, 16)); // Must have one extra
        }
    }

    void Player::UpdateDirection(const bool isFacingLeft)
    {
        float scaleX = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner())._scale._x;

        if ((isFacingLeft && scaleX > 0) || (!isFacingLeft && scaleX < 0))
        {
            ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner())._scale._x *= -1;
        }

        if (isFacingLeft)
        {
            ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner())._direction = RectColliderComponent::Direction::LEFT;
        }
        else
        {
            ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner())._direction = RectColliderComponent::Direction::RIGHT;
        }
    }

    void Jump(EntityID id)
    {
        // Jump action
        float lifetime = 0.0f;
        RigidBodyComponent& rb = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id);
        rb.AddForce({ rb._forceList[0]._force * 3.0f, lifetime });

        // Cue jump sound
        Messaging::AudioMessage message(
            Messaging::PLAY_COMPONENT, _JUMP,
            compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_JUMP),
            false, 0.1f, false, true, false,
            std::string(""));
        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
    }

    // Updated by Yumi
    void Player::Update(float dt)
    {
        (void)dt;

        static bool shouldMove = true;

        if (_backCollider >= 0)
        {
            float zoomModifier = (16 - p_Camera->GetZoom()) / 8;
            Vec2D newPosition = p_Camera->GetPositionVec2() - Vec2D{ 20.0f + zoomModifier * 2.4f, 0.0f };
            ECS::compManager->GetEntityComponent<TransformComponent>((EntityID)_backCollider)._position = newPosition;
        }

        RectColliderComponent& rc = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner());
        RigidBodyComponent& rb = ECS::compManager->GetEntityComponent<RigidBodyComponent>(GetOwner());

        Vec2D climbForce = { 0.0f, 1.5f };

        static bool isFacingLeft = false, canJump = true;
        
        TransformComponent& t = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner());

        if (t._scale._x < 0) //set the right direction on spawn
            isFacingLeft = true;
        else
            isFacingLeft = false;

        if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Level-1.1" ||
            Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Level-2.1")
        {
            static size_t triggered = 0;
            //std::cout << triggered << std::endl;

            float speed = 2.5f;

            for (size_t i = 0; i < cameraTriggerPoints.size(); ++i)
            {
                //std::cout << cameraTriggerPoints.size() << std::endl;
                auto& point = cameraTriggerPoints[i];
                if (!point._triggered)
                {
                    triggered = i;

                    if (i < (cameraTriggerPoints.size() - 1) && t._position._x > point._playerPosition._x)
                    {
                        point._triggered = true;
                        CameraTriggerPoints::_nextCameraPosition = point._cameraPosition;
                        CameraTriggerPoints::_nextZoom = point._cameraZoom;
                    }

                    if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Level-1.1" && i == (cameraTriggerPoints.size() - 1))
                    {
                        CameraTriggerPoints::_nextCameraPosition._x = t._position._x - 6.0f;
                        if (t._position._y < 20.0f)
                            speed = 0.75f;

                        CameraTriggerPoints::_nextCameraPosition._y = 25.0f;

                        if (_backCollider >= 0)
                        {
                            ECS::compManager->GetEntityComponent<TransformComponent>((EntityID)_backCollider)._position = Vec2D{ -1000.0f, 0.0f };
                        }
                    }

                    break;
                }
            }

            p_Camera->SetPositionSmooth(
                CameraTriggerPoints::_nextCameraPosition._x,
                CameraTriggerPoints::_nextCameraPosition._y,
                CameraTriggerPoints::_nextZoom,
                speed);
        }
        else
        {
            p_Camera->SetPositionSmooth(t._position._x, t._position._y);
        }

        static float delay = 0.0f, immunityTimer = 0.0f;
        static std::string deathBy;
        //p_Camera->SetZoom(16.0f);
        if (!CheckInteractable(dt))
        {
            ChangeState_Jeff(JeffState::j_Idle);
            return;
        }
        
        // Cheat codes
        if (p_Input->KeyIsHeld(GLFW_KEY_0)) // Lose
        {
            GameLogic::GetInstance()->SetEndState(-1, deathBy);
        }

        if (p_Input->KeyIsHeld(GLFW_KEY_9)) // Win
        {
            Messaging::SceneMessage message(Messaging::SET_CHANGE, "", "", true, "");
            //Messaging::SceneMessage message4(Messaging::CHANGE_SCENE, nextScene, "", false, "");
            Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message);

            std::string _curr = Scene::SceneManager::sceneManager->GetCurrentSceneName();
            std::string _next;
            if (_curr == "Level-1.1")
                _next = "Level-2.1.json";
            else if (_curr == "Level-2.1" || _curr == "Level-2.2" || _curr == "Level-2.3")
                _next = "Level-3.1.json";
            else if (_curr == "Level-3.1" || _curr == "Level-3.2" || _curr == "Level-3.3")
                _next = "Outro.json";
            GameLogic::GetInstance()->SetEndState(1, _next);
            return;
        }

        if (p_Input->KeyIsHeld(GLFW_KEY_8)) // Skip to the end
        {

            std::string _curr = Scene::SceneManager::sceneManager->GetCurrentSceneName();
            if (_curr == "Level-1.1")
            {
                for (auto& c : cameraTriggerPoints)
                {
                    c._triggered = true;
                }
                CameraTriggerPoints::_nextCameraPosition._x = t._position._x - 6.0f;
                CameraTriggerPoints::_nextCameraPosition._y = 25.0f;
                t._position = { -21.0,22.0 };
            }
            else if (_curr == "Level-2.1")
                t._position = { 77.0,0 };
            else if (_curr == "Level-2.2")
                t._position = { 75.0,19.0 };
            else if (_curr == "Level-2.3")
                t._position = { 74,7 };
            else if (_curr == "Level-3.1")
                t._position = { -11,30 };
            else if (_curr == "Level-3.2")
                t._position = { 148.50,27.0 };
            if (_curr == "Level-3.3")
                t._position = { 84.0,23.0 };
        }

        if (p_Input->KeyIsPressed(GLFW_KEY_7)) // Immunity toggle
        {
            _isImmune = !_isImmune;
        }

        if (_currHP <= 0.0f)
        {
            if (sJeffState == JeffState::j_Fall)
                GameLogic::GetInstance()->SetEndState(-1, deathBy);

            ChangeState_Jeff(JeffState::j_Dead);
        }

        /*******************************************************************************
        * For each state, identify what the player can do and what states it can change to.
        * This is "simulating" what proper states would be like.
        * Make it WORK first, optimize LATER.
        *******************************************************************************/
        switch (sJeffState)
        {
        case JeffState::j_Idle:
            _SKey = false;
            rc.SetSizeAndOffset(_colliderScale[0]);
            // Move left
            if ((p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT)))
            {
                isFacingLeft = true;
                ChangeState_Jeff(JeffState::j_Walk);
            }
            // Move right
            else if ((p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT)))
            {
                isFacingLeft = false;
                ChangeState_Jeff(JeffState::j_Walk);
            }
            // Crawl
            if ((p_Input->KeyIsHeld(GLFW_KEY_S) || p_Input->KeyIsHeld(GLFW_KEY_DOWN)) && canJump)
            {
                ChangeState_Jeff(JeffState::j_Crawl);
            }
            // Jump
            if (p_Input->KeyIsHeld(GLFW_KEY_SPACE) && canJump)
            {
                canJump = false;
                // Jump action
                Jump(GetOwner());
            }
            if (p_Input->KeyIsHeld(GLFW_KEY_Q))
            {
                /*_stillClimbing = true;
                compManager->GetEntityComponent<RigidBodyComponent>(GetOwner()).AddForce({ climbForce, 1.0f });*/
                if (_climb)
                    ChangeState_Jeff(JeffState::j_Climb);
            }

            if (entityManager->FindEntity(0).GetName() == "Player")
            {
                Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _WALK2,
                    compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
            }
            if (sJeffState != JeffState::j_Climb)
                _climb = false;

            // For edge case where hit while flying
            if (rb.GetGravityScale() != _defaultGravity)
                rb.SetGravityScale(_defaultGravity);

            break;
            /*******************************************************************************
            *******************************************************************************/
        case JeffState::j_Walk:
        {
            if (isFacingLeft)
            {
                // Change back to idle
                if (!(p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT)))
                {
                    ChangeState_Jeff(JeffState::j_Idle);
                }
                // Change to right
                if ((p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT)) && !(p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT)))
                {
                    isFacingLeft = false;
                }
                // Move left action
                rb.AddForce(forceType::LEFT);
                if (entityManager->FindEntity(0).GetName() == "Player")
                {
                    // if in the air, no walk sound
                    if ((rc._collisionBitflag & RectColliderComponent::CollisionBitflag::DOWN_))
                    {
                        Messaging::AudioMessage message(Messaging::PLAY_COMPONENT, _WALK2,
                            compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                    }
                    else
                    {

                        Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _WALK2,
                            compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                    }
                }
            }
            else
            {
                // Change back to idle
                if (!(p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT)))
                {
                    ChangeState_Jeff(JeffState::j_Idle);
                }
                // Change to left
                if ((p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT)) && !(p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT)))
                {
                    isFacingLeft = true;
                }
                // Move right action
                rb.AddForce(forceType::RIGHT);
                if (entityManager->FindEntity(0).GetName() == "Player")
                {
                    // if in the air, no walk sound
                    if ((rc._collisionBitflag & RectColliderComponent::CollisionBitflag::DOWN_))
                    {
                        Messaging::AudioMessage message(Messaging::PLAY_COMPONENT, _WALK2,
                            compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                    }
                    else
                    {

                        Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _WALK2,
                            compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                        Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                    }
                }
            }

        }
        // Crawl
        if ((p_Input->KeyIsHeld(GLFW_KEY_S) || p_Input->KeyIsHeld(GLFW_KEY_DOWN)) && canJump)
        {
            ChangeState_Jeff(JeffState::j_Crawl);
        }
        // Jump
        if (p_Input->KeyIsHeld(GLFW_KEY_SPACE) && canJump)
        {
            canJump = false;
            // Jump action
            Jump(GetOwner());
        }
        break;
        /*******************************************************************************
        *******************************************************************************/
        case JeffState::j_Crawl:
            // The "false" used here means PAUSE ANIMATION ONLY, not don't change state
            ChangeState_Jeff(JeffState::j_Crawl, false);
            // Go faster
            rb.SetMovementSpeed(3.0f);
            rc.SetSizeAndOffset(_colliderScale[1]);

            // Move left
            if ((p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT)))
            {
                ChangeState_Jeff(JeffState::j_Crawl);
                isFacingLeft = true;
                // Move left action
                rb.AddForce(forceType::LEFT);

                if (entityManager->FindEntity(0).GetName() == "Player")
                {
                    Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _WALK2,
                        compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);

                    Messaging::AudioMessage message1(Messaging::PLAY_COMPONENT, _CRAWL,
                        compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_CRAWL), false, 0.2f, false, false, true, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
                }
            }
            // Move right
            if ((p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT)))
            {
                ChangeState_Jeff(JeffState::j_Crawl);
                isFacingLeft = false;
                // Move right action
                rb.AddForce(forceType::RIGHT);

                if (entityManager->FindEntity(0).GetName() == "Player")
                {
                    Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _WALK2,
                        compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_WALK2), false, 1.5f, false, false, true, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);

                    Messaging::AudioMessage message1(Messaging::PLAY_COMPONENT, _CRAWL,
                        compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_CRAWL), false, 0.2f, false, false, true, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
                }
            }

            if ((p_Input->KeyIsHeld(GLFW_KEY_S) || p_Input->KeyIsHeld(GLFW_KEY_DOWN)) &&
                (!(p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT))) &&
                (!(p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT))))
            {
                Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _CRAWL,
                    ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_CRAWL), false, 0.2f, false, false, true, std::string(""));
                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
            }


            if ((p_Input->KeyIsHeld(GLFW_KEY_S) || p_Input->KeyIsHeld(GLFW_KEY_DOWN)))
            {
                _SKey = false;
                _uncrawlCollisionBitflag = 0x00;
            }

            // Back to idle
            if (!(p_Input->KeyIsHeld(GLFW_KEY_S) || p_Input->KeyIsHeld(GLFW_KEY_DOWN)))
            {
                _SKey = true;
                if (_canUncrawl)
                {
                    // Go back to normal speed
                    rb.SetMovementSpeed(1.0f);
                    rc.SetSizeAndOffset(_colliderScale[0]);
                    ChangeState_Jeff(JeffState::j_Idle);

                    Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _CRAWL,
                        ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_CRAWL), false, 0.2f, false, false, true, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
                }
            }


            break;
            /*******************************************************************************
            *******************************************************************************/
        case JeffState::j_Climb:
            compManager->GetEntityComponent<RigidBodyComponent>(GetOwner()).SetGravityScale(0.03f);
            if (_climb)
            {
                /*_stillClimbing = true;*/
                compManager->GetEntityComponent<RigidBodyComponent>(GetOwner()).AddForce({ climbForce, 0.6f });
                
                if (entityManager->FindEntity(0).GetName() == "Player")
                {
                    Messaging::AudioMessage message(Messaging::PLAY_COMPONENT, _CRAWL,
                        compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_CRAWL), false, 0.2f, false, false, true, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }

                if (climbPathID != GetOwner())
                    if (!Collision::AABBToAABB(GetOwner(), climbPathID, dt))
                        _climb = false;
                
            }
            /*_stillClimbing = true;*/
            // compManager->GetEntityComponent<RigidBodyComponent>(GetOwner()).AddForce({ climbForce, 0.6f });
            
            if (!p_Input->KeyIsHeld(GLFW_KEY_Q) || !_climb)
            {
                rb.SetGravityScale(_defaultGravity);
                ChangeState_Jeff(JeffState::j_Idle);
                rb.SetAcceleration({ 0.0f, 0.0f });
                rb.SetAcceleration({ 0.0f, 0.1f });
                rb.SetVelocity ({ 0.0f, 0.0f });

                //if (_stillClimbing)
                //{
                //    compManager->GetEntityComponent<RigidBodyComponent>(GetOwner()).AddForce({ climbForce, 2.5f });
                //    _stillClimbing = false;
                //}

                Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _CRAWL,
                    ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_CRAWL), false, 0.2f, false, false, true, std::string(""));
                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
                _climb = false;
            }
            break;
            /*******************************************************************************
            *******************************************************************************/
        case JeffState::j_Float:
            rb.SetGravityScale(0.3f);

            if (entityManager->FindEntity(0).GetName() == "Player")
            {
                Messaging::AudioMessage message(Messaging::PLAY_LOOPSFX, _FLY,
                    compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_FLY), false, 1.0f, false, false, true, std::string(""));
                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
            }

            // Move left
            if ((p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT)))
            {
                // Move left action
                rb.AddForce(forceType::LEFT);
                isFacingLeft = true;

                Messaging::AudioMessage message(Messaging::PLAY_LOOPSFX, _FLY,
                    compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_FLY), false, 1.0f, false, false, true, std::string(""));
                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
            }
            // Move right
            else if ((p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT)))
            {
                // Move right action
                rb.AddForce(forceType::RIGHT);
                isFacingLeft = false;

            }
            if (!p_Input->KeyIsHeld(GLFW_KEY_SPACE) || (rc._collisionBitflag & RectColliderComponent::CollisionBitflag::DOWN_))
            {
                rb.SetGravityScale(_defaultGravity);

                Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _FLY,
                    ECS::compManager->GetEntityComponent<AudioComponent>(0).AudioCheck(_FLY), false, 1.0f, false, false, true, std::string(""));
                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
                
                ChangeState_Jeff(JeffState::j_Idle);
            }
            // Check if hurt while in the air; reset gravity if so
            if (_isHurt)
            {
                rb.SetGravityScale(_defaultGravity);
                _isHurt = false;
            }
            break;
            /*******************************************************************************
            *******************************************************************************/
        case JeffState::j_Squish:
            rc.SetSizeAndOffset(_colliderScale[2]);
            if (_immuneTimer >= 1.0f)
            {
                if (isFacingLeft)
                {
                    rb.AddForce(Force(Vec2(500.0f, 250.0f)));
                }
                else
                {
                    rb.AddForce(Force(Vec2(-500.0f, 250.0f)));
                }
                ChangeState_Jeff(JeffState::j_Unsquish);
            }
            break;
            /*******************************************************************************
            *******************************************************************************/
        case JeffState::j_Unsquish:
            rc.SetSizeAndOffset(_colliderScale[0]);
            BackToIdle(2.0f);
            break;
            /*******************************************************************************
            *******************************************************************************/
        case JeffState::j_Hurt:
            if (_immuneTimer >= 1.0f)
            {
                ChangeState_Jeff(JeffState::j_Idle);
            }

            // Move left
            if ((p_Input->KeyIsHeld(GLFW_KEY_A) || p_Input->KeyIsHeld(GLFW_KEY_LEFT)))
            {    
                // Move left action
                rb.AddForce(forceType::LEFT);
                isFacingLeft = true;
            }
            // Move right
            if ((p_Input->KeyIsHeld(GLFW_KEY_D) || p_Input->KeyIsHeld(GLFW_KEY_RIGHT)))
            {
                // Move right action
                rb.AddForce(forceType::RIGHT);
                isFacingLeft = false;
            }
            // Jump
            if (p_Input->KeyIsHeld(GLFW_KEY_SPACE) && canJump)
            {
                canJump = false;
                // Jump action
                Jump(GetOwner());
            }
            deathBy = "Death by Interactables";

            break;
            /*******************************************************************************
            *******************************************************************************/
        case JeffState::j_Shock:
            if (_immuneTimer >= 1.0f)
            {
                rb.SetMovementSpeed(1.0f);
                ChangeState_Jeff(JeffState::j_Idle);
            }
            deathBy = "Death by Shock Platform";
            break;
            /*******************************************************************************
            *******************************************************************************/
            // LOSE CONDITION
        case JeffState::j_Fall:
            deathBy = "Death From Falling";
            _currHP-= 2;
            return;
            /*******************************************************************************
            *******************************************************************************/
            // LOSE CONDITION
        case JeffState::j_Dead:
        {
            _deathtime -= Utility::Clock::GetInstance().GetGDT();
            if(_deathtime <= 0)
            {    
                _deathtime = 2.0f;
                GameLogic::GetInstance()->SetEndState(-1, deathBy);
                return;
            }
        }
        default:
            break;
        }

        static float blinkTimer = 0.0f;
        static float alpha = 0.5f;
        if (_isImmune && _immuneTimer < 5.0f)
        {
            _immuneTimer += dt;
            blinkTimer += dt;

            if (blinkTimer >= 0.25f)
            {
                blinkTimer = 0.0f;

                if (alpha == 0.5f)
                {
                    alpha = 1.0f;
                }
                else
                {
                    alpha = 0.5f;
                }

                ECS::compManager->GetEntityComponent<SpriteComponent>(Player::_instance->GetOwner())._color = { 1.0f, 1.0f, 1.0f, alpha };
            }

            if (_immuneTimer >= 5.0f)
            {
                _isImmune = false;
                _immuneTimer = 0.0f;
                ECS::compManager->GetEntityComponent<SpriteComponent>(Player::_instance->GetOwner())._color = { 1.0f, 1.0f, 1.0f, 1.0f };
            }
        }


        UpdateDirection(isFacingLeft);
        
        if (!canJump)
        {
            static bool canFloat = false;

            // The conditions for a single jump is important!
            if (delay >= 0.5f && (rc._collisionBitflag & RectColliderComponent::CollisionBitflag::DOWN_) && !p_Input->KeyIsHeld(GLFW_KEY_SPACE))
            {
                delay = 0.0f;
                canJump = true;
                canFloat = false;
            }

            if (!canJump && !canFloat && !p_Input->KeyIsHeld(GLFW_KEY_SPACE))
            {
                canFloat = true;
            }

            // Float
            if (p_Input->KeyIsHeld(GLFW_KEY_SPACE) && canFloat && rb.GetVelocity()._y < -0.5f)
            {

                ChangeState_Jeff(JeffState::j_Float);
            }

            delay += dt;
        }

        if (!(rc._collisionBitflag & RectColliderComponent::CollisionBitflag::DOWN_))
        {
            canJump = false;
        }



        // How to better check collision?
        if (sJeffState != JeffState::j_Hurt)
        {
            for (auto& e : entityManager->GetEntityList())
            {
                if (strcmp(e.GetTag().c_str(), "Shock Platform") == 0)
                {
                    if (Collision::AABBToAABB(GetOwner(), e.GetID(), dt))
                    {
                        /*_currHP -= compManager->GetEntityComponent<AttackComponent>(e.GetID())._damage;
                        ChangeState_Jeff(JeffState::j_Shock);*/
                    }
                }
            }
        }

        std::string currentName = Scene::SceneManager::sceneManager->GetCurrentSceneName();
        currPlayerHP = std::pair<std::string, int>{ currentName,_currHP };


    }

    void Player::IsHurt(const bool b) //for trap
    {
        if (b)
        {
            if (!Player::GetInstance()->_isImmune)
                Player::GetInstance()->_currHP -= 10;

            Player::GetInstance()->_isImmune = true;
            _immuneTimer = 0.0f;
            _isHurt = true;
        }
        else if (!b)
        {
            ChangeState_Jeff(JeffState::j_Idle);
            _isHurt = false;
            //compManager->GetEntityComponent<SpriteComponent>(_vacuumEntity->GetID())._renderable = true;
        }
    }

    void Player::UpdateWallMap()
    {
        if (!_MoveWallCheck.empty())
            _MoveWallCheck.clear();

        for (auto i = entityManager->GetEntityList().begin();
            i != entityManager->GetEntityList().end(); ++i)
        {
            if ((*i).GetTag() == "MovableWall")
            {
                _MoveWallCheck.insert({ (*i).GetID(),true });
            }
        }
    }

    void Player::UpdateDoorCoord()
    {
        if (!_doorCoord.empty())
            _doorCoord.clear();

        for (auto i = entityManager->GetEntityList().begin();
            i != entityManager->GetEntityList().end(); ++i)
        {
            if ((*i).GetTag() == "BlueDoor")
            {
                auto pos = compManager->GetEntityComponent<TransformComponent>((*i).GetID())._position._y;
                _doorCoord.insert({ (*i).GetID(),pos });
            }
        }
    }

    Player* Player::GetInstance()
    {
        if (!_instance)
        {
            _instance = new Player;
        }
        return _instance;
    }

    ScriptType* Player::Clone() const
    {
        return new Player{ *this };
    }

    void Player::UpdateHp()
    {
        std::string currentName = Scene::SceneManager::sceneManager->GetCurrentSceneName();
        std::string prev_scene = currPlayerHP.first;

        if (currentName == prev_scene) //same level - full hp
            currPlayerHP.second = _maxHP;
        else
        {
            int curr = atoi(&currentName[6]);
            int prev = atoi(&prev_scene[6]);
            
            int a;
            if (prev_scene == "Menu")
                a = 1;
            else 
                a = atoi(&currentName[6]);

            if (a == 1) 
                _currHP = _maxHP;
            else if (curr == prev)
                _currHP = currPlayerHP.second;
        }
    }

    void Player::End()
    {
        _MoveWallCheck.clear();
        _doorCoord.clear();
        _spawnObjs.clear();
        delete _instance;
    }
}