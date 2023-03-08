/*******************************************************************************
filename    Enemy.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "Enemy.h"
#include "../GSM/PatrolState.h"
#include "../GSM/IdleState.h"
#include "../../Physics/Collision.h"
#include "../../ECS/Components.h"
#include "../../GameLogic/GameLogic.h"
#include "../../GameLogic/NPC_States.h"

#include <iostream>

#define ALERT_TIME 1.25f
#define CONFUSE_TIME 2.0f

namespace Engine
{
    namespace AI
    {
        bool stuntrigger = false;

        Enemy::Enemy() :
            _patrolTime(0.0f),
            _currentTimer(0.0f),
            _hp (100),
            _dt (1.0f/60.0f),
            _target(NULL)
        {
        }

        Enemy::~Enemy()
        {
            //delete _stateMachine;
        }

        void Enemy::Init()
        {
            ECS::Entity* tmp = Scripts::GetOwner();
            if (tmp != nullptr)
            {
                if(_stateMachine == nullptr)
                    _stateMachine = new StateMachine<Scripts>((Scripts*)tmp);

                this->SetOwner(tmp);

                if (ECS::compManager->CheckForEntity<PatrolComponent>(tmp->GetID()))
                {
                    PatrolComponent &patrol
                        = ECS::compManager->GetEntityComponent<PatrolComponent>(tmp->GetID());

                    patrol._belongsTo = this;
                    _patrolTime = _currentTimer = patrol._patrolTime;
                }

                _stateMachine->SetOwner(this);
                _stateMachine->SetGlobalState(new PatrolState());

                _currDir = Direction::NONE;
                _target = p_AISystem->FindPlayer();
                _deathTimer = 3.0f;
                _isAlive = true;
                _hp = 100;
                _timer = 0.0f;

                _currentState = PirateState::p_IDLE;

                /*******************************************************************************
                * Hard-coded values to ensure consistency between scenes
                *******************************************************************************/
                TransformComponent& t = ECS::compManager->GetEntityComponent<TransformComponent>(tmp->GetID());
                RectColliderComponent& rc = ECS::compManager->GetEntityComponent<RectColliderComponent>(tmp->GetID());
                //t._scale = { 5.0f, 6.0f };
                if (t._scale._x < 0)
                    _currDir = Direction::RIGHT;
                rc.SetSizeAndOffset({ 2.5f, 5.0f, -0.05f, -0.4f });

                ChangeState_Pirate(_currentState, PirateState::p_WALK, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
            }
        }

        void Enemy::Update(float dt)
        {
            _dt = dt;
            
            if (_isAlive)
            {
                if (ECS::compManager->CheckForEntity<PatrolComponent>(GetOwner()->GetID()))
                {
                    PatrolComponent& patrol
                        = ECS::compManager->GetEntityComponent<PatrolComponent>(GetOwner()->GetID());

                    if (ECS::compManager->CheckForList<AttackComponent>())
                    {
                        std::map<EntityID, AttackComponent>& list
                            = ECS::compManager->GetComponentList<AttackComponent>().GetList();
                        
                        for (auto i = list.begin(); i != list.end(); ++i)
                        {
                            // Change this later
                            // Don't attack other enemies, thats just rude
                            // Don't attack itself in confusion
                            if (i->first != GetOwner()->GetID() &&
                                ECS::entityManager->FindEntity((*i).first).GetTag() != "Enemy" &&
                                ECS::entityManager->FindEntity((*i).first).GetTag() != "ExplodingBarrel")
                            {
                                if (IsHit(i->first))
                                {
                                    patrol._currHP -= i->second._damage;
                                }
                            }
                        }
                    }

                    // Check if get stunned by box
                    if (ECS::compManager->CheckForList<VacuumableComponent>())
                    {
                        std::map<EntityID, VacuumableComponent>& list
                            = ECS::compManager->GetComponentList<VacuumableComponent>().GetList();

                        for (auto i = list.begin(); i != list.end(); ++i)
                        {
                            // Get Stunned by the box beauty
                            // Stun = no attack & no move
                            if (ECS::entityManager->FindEntity((*i).first).GetTag() == "Box"
                                || ECS::entityManager->FindEntity((*i).first).GetTag() == "ExplodingBarrel")
                            {
                                VacuumComponent& vacuum
                                    = ECS::compManager->GetEntityComponent<VacuumComponent>(Player::GetInstance()->_vacuumEntity);
                                if ((vacuum._vacuumState != AI::VacuumState::SUCK) &&
                                    vacuum._vacuumState != AI::VacuumState::ISHOLDING)
                                {
                                    RigidBodyComponent& boxRB
                                        = ECS::compManager->GetEntityComponent<RigidBodyComponent>((*i).first);

                                    if (IsHit(i->first))
                                    {
                                        if ((ECS::entityManager->FindEntity((*i).first).GetTag() == "Box"
                                            &&  boxRB.GetVelocity() != 0.0f) ||
                                            (boxRB.GetVelocity() != Vec2D{ 0.0f,0.0f } &&
                                                ECS::entityManager->FindEntity((*i).first).GetTag() == "ExplodingBarrel"))
                                        {
                                            _isStunned = true;
                                            ChangeState_Pirate(_currentState, PirateState::p_STUNNED, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);

                                            if (!stuntrigger)
                                            {
                                                stuntrigger = true;
                                                Messaging::AudioMessage message(Messaging::PLAY_SFX, _WOBBLE,
                                                    "Wobble.ogg", false, 1.0f, false, true, false, std::string(""));
                                                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                                            }

                                            boxRB.SetVelocity(0.0f);
                                            boxRB.SetAcceleration(0.0f);
                                       }
                                       
                                    }
                                }
                            }   

                            
                        }
                    }

                    
                    _hp = patrol._currHP;
                }
                std::map<EntityID, RectColliderComponent>& list
                    = compManager->GetComponentList<RectColliderComponent>().GetList();

                for (auto i = list.begin(); i != list.end(); ++i)
                {
                    if (entityManager->FindEntity((*i).first).GetTag() == "Trap")
                        if (Collision::AABBToAABB(entityManager->FindEntity((*i).first).GetID(), GetOwner()->GetID(), dt))
                        {
                            _hp = 0;
                            _isAlive = false;
                            break;
                        }
                                 
                }
            }
            _stateMachine->Update();

            //std::cout << _timer << std::endl;
         
            if (!_isAlive)
            {
                Dead();
            }

            //std::cout << "Exit update" << std::endl;
        }

        void Enemy::ChaseTarget()
        {
            if (Player::GetInstance()->_currHP <= 0)
                return;

            if (_currentState == PirateState::p_ALERTED)
            {
                _timer += p_Clock.GetGDT();

                if (_timer >= ALERT_TIME)
                {
                    _timer = 0.0f;
                    _previousState = _currentState;
                    ChangeState_Pirate(_currentState, PirateState::p_CHASE, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
                }
            }

            if (_currentState == PirateState::p_ATTACK)
            {
                if (_attackTimer >= 1.0f)
                {
                    _attackTimer = 0.0f;
                    ChangeState_Pirate(_currentState, PirateState::p_CHASE, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
                }
                else
                {
                    _attackTimer += _dt;
                    return;
                }
            }

            if (_currentState != PirateState::p_CHASE)
            {
                return;
            }
            //std::cout << "Enter Chase state function" << std::endl;
            PatrolComponent& patrol
                = ECS::compManager->GetEntityComponent<PatrolComponent>(GetOwner()->GetID());

            TransformComponent& playerPos
                = ECS::compManager->GetEntityComponent<TransformComponent>(_target);
            TransformComponent& enemyPos
                = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID());

            float distance = playerPos._position._x - enemyPos._position._x;

            int direction = (int)(distance / abs(distance));

            if (direction == 1)
            {
                _currDir = Direction::RIGHT;
                enemyPos._scale._x = -abs(enemyPos._scale._x);
            }
            else
            {
                _currDir = Direction::LEFT;
                enemyPos._scale._x = abs(enemyPos._scale._x);
            }

            enemyPos._position._x += direction * 2.5f * patrol._patrolSpeed * 0.05f;
        }

        void Enemy::Patrol()
        {

            if (_currentState == PirateState::p_ALERTED)
            {
                _timer += p_Clock.GetGDT();

                if (_timer >= ALERT_TIME)
                {
                    _timer = 0.0f;
                    _previousState = _currentState;
                    // std::cout << "Change to walking!" << std::endl;
                    ChangeState_Pirate(_currentState, PirateState::p_WALK, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
                }
            }
            else if (_currentState == PirateState::p_CHASE)
            {
                _previousState = _currentState;
                // std::cout << "Change to walking!" << std::endl;
                ChangeState_Pirate(_currentState, PirateState::p_WALK, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
            }

            TransformComponent& pos =
                ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID());

            PatrolComponent& patrol
                = ECS::compManager->GetEntityComponent<PatrolComponent>(GetOwner()->GetID());

            _patrolTime = patrol._patrolTime;

            if (_currentTimer > 0)
            {
                _currentTimer -= _dt;
                if (_currDir == Direction::LEFT)
                {
                    pos._position._x -= patrol._patrolSpeed * 0.05f;
                    if(pos._scale._x < 0)
                        pos._scale._x *= -1;
                }
                else if (_currDir == Direction::RIGHT)
                {
                    pos._position._x += patrol._patrolSpeed * 0.05f;
                    if(pos._scale._x > 0)
                        pos._scale._x *= -1;
                }
            }
            else
            {
                // Change direction
                if (_currDir == Direction::RIGHT)
                    _currDir = Direction::LEFT;
                else
                    _currDir = Direction::RIGHT;

                _currentTimer = _patrolTime; // reset internal patrol
            }
            patrol._internalTimer = _currentTimer;

            if ((_previousState == PirateState::p_CHASE) && (_isConfused))
            {
                // Set to confused
                //std::cout << "Confused" << std::endl;

                _confusedTimer += _dt;

                if (_confusedTimer >= CONFUSE_TIME)
                    _isConfused = false;
            }
            else if (_confusedTimer >= CONFUSE_TIME)
            {
                // Reset confused time
                _confusedTimer = 0.0f;
            }
        }

        void Enemy::GetKnockBack()
        {
            EntityID vacuumID = (EntityID)-1;

            std::map<EntityID, VacuumComponent>& list
                = ECS::compManager->GetComponentList<VacuumComponent>().GetList();

            for (auto i = list.begin(); i != list.end(); ++i)
            {
                if ((*i).second._parent == _target)
                {
                    vacuumID = (*i).first;
                    break;
                }
            }

            //    VacuumComponent& vacuum =
            //        ECS::compManager->GetEntityComponent<VacuumComponent>(vacuumID);

            //if (Collision::AABBToAABB(vacuumID, GetOwner()->GetID(), _dt))
            //    return;

            RigidBodyComponent& playerRb
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(_target);

            TransformComponent& playerPos
                = ECS::compManager->GetEntityComponent<TransformComponent>(_target);
            TransformComponent& enemyPos
                = ECS::compManager->GetEntityComponent<TransformComponent>(GetOwner()->GetID());
            RigidBodyComponent& eRb
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(GetOwner()->GetID());

            // Damage Player
                // Some function
                // Dont mvoe the Enemy
            eRb.SetTotalForce(0.0f);

            float distance = playerPos._position._x - enemyPos._position._x;


            // Player is on the right of the enemy
            if (distance >= 0)
            {
                // Add knockback force
                Vec2D force = { 250.0f, 60.0f };
                float lifetime = 0.1f;
                playerRb.AddForce({ force, lifetime });
            }
            // Player is on the left of the enemy
            else if (distance < 0)
            {
                // Add knockback force
                Vec2D force = { -250.0f, 60.0f };
                float lifetime = 0.1f;
                playerRb.AddForce({ force, lifetime });
            }

            if (Player::GetInstance()->_currHP > 0 && (ECS::entityManager->FindEntity(_target).GetTag() == "Player"))
            {
                if (!Player::GetInstance()->_isImmune)
                {
                    // Find the attack damage of the enemy
                    if (ECS::compManager->CheckForEntity<AttackComponent>(GetOwner()->GetID()))
                    {
                        AttackComponent& a 
                            = ECS::compManager->GetEntityComponent<AttackComponent>(GetOwner()->GetID());
                        Player::GetInstance()->_currHP -= a._damage;
                    }
                    // Else, set it to 10
                    else
                        Player::GetInstance()->_currHP -= 10;
						
                    Player::GetInstance()->_isImmune = true;
                    Player::GetInstance()->_immuneTimer = 0.0f;
                    ChangeState_Jeff(JeffState::j_Hurt);
                }

                if (Player::GetInstance()->_currHP <= 0)
                    ChangeState_Pirate(_currentState, PirateState::p_WALK, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
            }
        }

        bool Enemy::IsInRange()
        {
            //std::cout << "Enter in range function" << std::endl;
            if (ECS::compManager->CheckForEntity<CircleColliderComponent>(GetOwner()->GetID()))
            {
                if (Collision::AABBtoCircle(_target, GetOwner()->GetID()))
                {
                    if (Player::GetInstance()->_currHP > 0 && _currentState == PirateState::p_WALK)
                    {
                        _timer = 0.0f;
                        //std::cout << "Change to alerted!" << std::endl;
                        ChangeState_Pirate(_currentState, PirateState::p_ALERTED, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);

                        if (ECS::compManager->CheckForEntity<AudioComponent>(GetOwner()->GetID()))
                        {
                            Messaging::AudioMessage message(Messaging::PLAY_COMPONENT, _PLAYERSPOTTED,
                                compManager->GetEntityComponent<AudioComponent>(GetOwner()->GetID()).AudioCheck(_PLAYERSPOTTED), false, 0.5f, false, true, false, std::string(""));
                            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                        }
                    }

                    return true;
                }
            }
            return false; // return false if there is no circle collider
        }

        bool Enemy::DealDamage()
        {
            return (Collision::AABBToAABB(_target, GetOwner()->GetID(), _dt));
        }

        void Enemy::Attack(float attackTimer)
        {
            if (_attackTimer >= attackTimer)
            {
                _attackTimer = 0.0f;
                _isAttacking = true;
            }
            else
            {
                NoMovement();
                _attackTimer += _dt;
                ChangeState_Pirate(_currentState, PirateState::p_ATTACK, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
            }
        }

        bool Enemy::IsHit(EntityID obj)
        {
            //std::cout << "Enter is hit function" << std::endl;

            if (Collision::AABBToAABB(obj, GetOwner()->GetID(), _dt))
            {

                //ChangeState_Pirate(_currentState, PirateState::p_HURT);
                return true;
            }
            return false;
        }

        void Enemy::ChangeTime(float patrolTime)
        {
            _patrolTime = patrolTime;
            _currentTimer = patrolTime;
        }

        void Enemy::NoMovement()
        {

        }

        // p_STUNNED
        void Enemy::Stunned(float stunnedTimer)
        {
            if (_timer >= stunnedTimer)
            {
                _isStunned = false;
                _timer = 0.0f;

                if (stuntrigger)
                {
                    stuntrigger = false;
                }

                ChangeState_Pirate(_currentState, PirateState::p_WALK, ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID())._animatorIndex);
                Patrol();
            }
            else
            {
                NoMovement();
                _timer += _dt;
            }
        }

        void Enemy::Dead()
        {
            int turncount = 0;
            SpriteComponent& enemySprite = ECS::compManager->GetEntityComponent<SpriteComponent>(GetOwner()->GetID());
            RectColliderComponent& enemyRectCol = ECS::compManager->GetEntityComponent<RectColliderComponent>(GetOwner()->GetID());

            TransformComponent& enemyPOS
                = ECS::compManager->
                GetEntityComponent<TransformComponent>(GetOwner()->GetID());

            if (_isAlive)
            {
                _isAlive = false;
                _deathTimer = 3.0f;
                turncount++;
                if (ECS::compManager->CheckForEntity<AudioComponent>(GetOwner()->GetID()))
                {
                    Messaging::AudioMessage message(Messaging::PLAY_COMPONENT, _ENEMYDEAD,
                        compManager->GetEntityComponent<AudioComponent>(GetOwner()->GetID()).AudioCheck(_ENEMYDEAD), false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }
            }
            if (!_isAlive)
            {
                turncount++;

                if (_deathTimer >= 0)
                {
                    turncount++;
                    enemyPOS
                        = ECS::compManager->
                        GetEntityComponent<TransformComponent>(GetOwner()->GetID());
                  //  TransformComponent& playerPOS
                  //      = ECS::compManager->
                  //      GetEntityComponent<TransformComponent>(_target);
                    
                    // Play dead animation?
                    //enemyPOS._position._y -= 0.1f;
                    enemyPOS._rotate += 8.0f;

                   // // Player on the left of the enemy
                   // if (playerPOS._position._x < enemyPOS._position._x)
                   //     enemyPOS._position._x += 0.1f;
                   // else
                   //     enemyPOS._position._x -= 0.1f;
                }

                if (_deathTimer <= 0)
                {
                    enemyRectCol.SetHeight(0.0f);
                    enemyRectCol.SetWidth(0.0f);
                    enemySprite._renderable = false;
                    ECS::compManager->GetEntityComponent<AttackComponent>(GetOwner()->GetID())._damage = 0;
                }

                _deathTimer -= _dt;

                
            }
        }
    }
}