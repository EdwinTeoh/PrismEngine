/*******************************************************************************
filename    ExplodingBarrel.cpp
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        Jan 29, 2021
brief

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "ExplodingBarrel.h"
#include "../../AI/AISystem.h"
#include "../../Physics/Collision.h"
#include "Player.h"
#include "../../ECS/Components.h"
#include "../../GameLogic/NPC_States.h"

namespace Engine
{
    // 2 Colliders
    // 1 rect collider to check if hit the barrel
    // 1 cicle collider for explosion radius
    // explode after a certain timer
    ExplodingBarrel* ExplodingBarrel::_instance = nullptr;
    bool exploded = false;

    ExplodingBarrel::ExplodingBarrel() :
        _isHit(false),
        _isActive(false),
        _exploded(false),
        _explodingBarrelTag(),
        _timer(0.5f),
        _currTimer(0.0f),
        _hardcodeTimer{1.0f},
        _maxRadius(3.5f)
    {
    }

    void ExplodingBarrel::SetScriptName()
    {
        _scriptName = "ExplodingBarrel";
    }

    std::string ExplodingBarrel::GetScriptName()
    {
        return _scriptName;
    }

    void ExplodingBarrel::SetTag(std::string s)
    {
        _explodingBarrelTag = s;
    }

    std::string ExplodingBarrel::GetTag()
    {
        return _explodingBarrelTag;
    }

    ExplodingBarrel* ExplodingBarrel::GetInstance()
    {
        /*if (!_instance)
        {*/
            _instance = new ExplodingBarrel;
       /* }*/

        return _instance;
    }

    ScriptType* ExplodingBarrel::Clone() const
    {
        return new ExplodingBarrel{ *this };
    }

    void ExplodingBarrel::Init()
    {
    }

    // Check if hit the rect collider
    bool ExplodingBarrel::CheckInRange(EntityID id1, float dt)
    {
        if (ECS::compManager->CheckForEntity<RectColliderComponent>(GetOwner()))
            return (Collision::AABBToAABB(id1, GetOwner(), dt));
        return false;
    }

    // Kaboom?
    // Yes Rico, Kaboom
    void ExplodingBarrel::Explode(float dt)
    {
        auto& barrelSprite = ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner());
        auto& barrelTransform = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner());
       // auto& barrelRectCol = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner());
        

        // Trying to make a circular explosion damage that gets bigger over time
        if (_currTimer < _timer)
        {
            _currTimer += dt;
        }
        else if (!_isActive)
        {
            _isActive = true;
            _exploded = true;
            barrelTransform._z = 101;
            barrelTransform._scale = 10;
            barrelSprite._texName = "explosion.png";
            barrelSprite._isMultiple = true;
            barrelSprite._animatorIndex = 9;
            Animator::_animatorCont[9].Restart();
            std::cout << Animator::_animatorCont[9]._currentFrame << std::endl;
            Animator::_animatorCont[9].Play();

            Messaging::AudioMessage message(Messaging::PLAY_SFX, _EXPLOSION, "Explosion.ogg", false, 0.2f, false, true, false, std::string(""));
            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
        }

        if (_isActive)
        {
            if (ECS::compManager->CheckForList<CircleColliderComponent>())
            {
                if (!ECS::compManager->CheckForEntity<CircleColliderComponent>(GetOwner()))
                {
                    _isActive = false;
                    _isHit = false;
                    return;
                }

                CircleColliderComponent& c
                    = ECS::compManager->GetEntityComponent<CircleColliderComponent>(GetOwner());

                if (c.GetRadius() <= _maxRadius)
                {
                    c.SetRadius(c.GetRadius() + 2.0f * dt);
                }

                else
                {
                    // Basically destroy exploding barrels
                    _isActive = false;
                    if (_hardcodeTimer > 0)
                    {
                        _hardcodeTimer -= Utility::Clock::GetInstance().GetGDT();
                        barrelSprite._renderable = false;
                        barrelTransform._position._y = -10000.0f;
                    }
                    else
                    {
                        _hardcodeTimer = 1.0f;
                        barrelSprite._texName = "ExplodingBarrel.png";
                    }
                    _isHit = false;
                }

                if (_isActive)
                {
                    // Check if enemies are in range
                    // Change this to HP Component PLEASE FOR MY SANITY
                    if (ECS::compManager->CheckForList<AttackComponent>())
                    {
                        AttackComponent& attack
                            = ECS::compManager->GetEntityComponent<AttackComponent>(GetOwner());

                        if (ECS::compManager->CheckForList<PatrolComponent>())
                        {
                            std::map<EntityID, PatrolComponent>& list
                                = ECS::compManager->GetComponentList<PatrolComponent>().GetList();

                            for (auto i = list.begin(); i != list.end(); ++i)
                            {
                                if (CheckInExplosionRange(i->first))
                                {
                                    i->second._currHP -= attack._damage;
                                    // NEED TO FIX
                                    //ChangeState_Pirate(i->second._previousState, PirateState::p_HURT, ECS::compManager->GetEntityComponent<SpriteComponent>(i->first)._animatorIndex);
                                    //KnockBack(i->first);
                                    //barrelSprite._texName = "Explosion1.png";
                                }
                            }
                        }

                        if (CheckInExplosionRange(Player::GetInstance()->GetOwner()))
                        {
                            if (sJeffState != JeffState::j_Hurt /*!Player::GetInstance()->_isImmune*/)
                            {
                                Player::GetInstance()->_currHP -= attack._damage;
                                KnockBack(Player::GetInstance()->GetOwner());
                                ChangeState_Jeff(JeffState::j_Hurt);
                                Player::GetInstance()->IsHurt(true);
                                //barrelSprite._texName = "Explosion1.png";
                            }
                        }
                    }
                }
            }
        }
    }

    // Don't stand in here or you get the kaboom
    bool ExplodingBarrel::CheckInExplosionRange(EntityID id1)
    {
        if (ECS::compManager->CheckForEntity<CircleColliderComponent>(GetOwner()))
        {
            return Collision::AABBtoCircle(id1, GetOwner());
        }

        return false; // return false if there is no circle collider    }
    }

    void ExplodingBarrel::ApplyDamage(EntityID id1)
    {
        (void)id1;
        if (ECS::compManager->CheckForList<AttackComponent>())
        {

        }

    }

    // Knockback effect
    void ExplodingBarrel::KnockBack(EntityID id1)
    {
        if (ECS::compManager->CheckForList<RigidBodyComponent>() && 
            ECS::compManager->CheckForList<TransformComponent>())
        {
            if (ECS::compManager->CheckForEntity<RigidBodyComponent>(id1) &&
                ECS::compManager->CheckForEntity<TransformComponent>(id1))
            {
                RigidBodyComponent& playerRb
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id1);

                TransformComponent& playerPos
                    = ECS::compManager->GetEntityComponent<TransformComponent>(id1);

                TransformComponent& barrelPos
                    = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner());

                // Damage Player
                    // Some function
                    // Dont move the barrel

                float distance = playerPos._position._x - barrelPos._position._x;

                // Player is on the right of the barrel
                if (distance >= 0)
                {
                    // Add knockback force
                    Vec2D force = { 200.0f, 50.0f };
                    float lifetime = 0.1f;
                    playerRb.AddForce({ force, lifetime });
                }
                // Player is on the left of the barrel
                else if (distance < 0)
                {
                    // Add knockback force
                    Vec2D force = { -200.0f, 50.0f };
                    float lifetime = 0.1f;
                    playerRb.AddForce({ force, lifetime });
                }
               // Player::GetInstance()->_isImmune = true;
            }
        }
        // ChangeState_Jeff(JeffState::j_Idle);
    }

    void ExplodingBarrel::Update(float dt)
    {
        if (Player::GetInstance()->_vacuumEntity)
        {
            if (ECS::compManager->CheckForEntity<VacuumComponent>(Player::GetInstance()->_vacuumEntity))
            {
                VacuumComponent& vacuum
                    = ECS::compManager->GetEntityComponent<VacuumComponent>(Player::GetInstance()->_vacuumEntity);

                if (vacuum._holdObjID == GetOwner())
                {
                    return;
                }
            }
        }
        
        if (CheckInRange(Player::GetInstance()->GetOwner(), dt))
        {
            _isHit = true;
        }
        std::map<EntityID, RectColliderComponent>& list
            = ECS::compManager->GetComponentList<RectColliderComponent>().GetList();

        for (auto i = list.begin(); i != list.end(); ++i)
        {
            if (ECS::entityManager->FindEntity((*i).first).GetTag() == "Box" || ECS::entityManager->FindEntity((*i).first).GetTag() == "Obstacle")
            {
                if (Collision::AABBToAABB(GetOwner(), (*i).first, dt))
                {
                    _isHit = true;
                }
            }
            else if (ECS::entityManager->FindEntity((*i).first).GetTag() == "Enemy")
            {
                if (Collision::AABBToAABB(GetOwner(), (*i).first, dt))
                {
                    _isHit = true;
                }
            }
        }

        // Check if particle component exist with the entity and enable the emitter
        if (ECS::compManager->CheckForEntity<ParticleComponent>(GetOwner()))
        {
            ParticleComponent& emitter = ECS::compManager->GetEntityComponent<ParticleComponent>(GetOwner());
            if (_isHit)
            {
                emitter.isActive = true;
            }
            else
                emitter.isActive = false;
        }

        if (_isHit)
        {
            if (!exploded)
            {
                Explode(dt);
            }
            exploded = false;
        }
    }

    void ExplodingBarrel::End()
    {
        delete _instance;
    }
}