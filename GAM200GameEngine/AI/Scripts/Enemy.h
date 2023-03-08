/*******************************************************************************
filename    Enemy.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Scripts.h"
#include "../../GameLogic/NPC_States.h"

namespace Engine
{
    namespace AI
    {
        // By default will patrol
        // Player in "Chase Range" (Circle)
        // Chases player until player is out of circle
        class Enemy : public Scripts
        {
            virtual Enemy* Clone() const { return new Enemy(*this); }

        public:
            bool alert = false;
            Enemy();
            virtual ~Enemy();

            void Init();
            void Update(float dt);

            void ChaseTarget();
            void Patrol();
            void GetKnockBack();

            bool IsInRange();
            bool DealDamage();
            void Attack(float attackTimer);
            bool IsHit(EntityID obj);
            void NoMovement();

            void ChangeTime(float patrolTime);
            void Stunned(float stunnedTimer);
            void Dead();

            // Data members
            float _patrolTime = 0.0f;         // Total Patrol Time
            float _currentTimer = 0.0f;       // Current internal time
            float _dt;                        // Current dt of game
            int _hp;
            EntityID _target;                 // Target
            float _deathTimer;                // Take away once theres animation

            // New stuff from Edwin
            //PirateState curr = PirateState::p_IDLE;
            //PirateState prev = PirateState::p_IDLE;
            int prev;
            float _timer = 0.0f;
            float _confusedTimer = 0.0f;
            float _attackTimer = 0.0f;
            bool _isConfused = false;
            bool _isStunned = false;
            bool _isAttacking = false;

            // isHit -> isInRange -> Patrol

            PirateState _currentState;
            PirateState _previousState;
        };
    }
}

#endif // _ENEMY_H_
