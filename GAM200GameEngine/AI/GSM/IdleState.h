/*******************************************************************************
filename    IdleState.h
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        November 19, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once

#include "AIStates.h"
#include "../Scripts/Scripts.h"
#include "../../ECS/Patrol.h"
#include "../../ECS/RigidBody.h"
#include "../../ECS/CircleCollider.h"
#include "../../Physics/Collision.h"
#include "DeadState.h"

namespace Engine
{
    namespace AI
    {
        class IdleState : public AIState<Scripts>
        {
        public:
            IdleState()
            {
                _stateName = "Idle State";
            }

            ~IdleState()
            {
            }

            void Enter(Scripts* enemy)
            {
                enemy->_action = Behaviour::STATE_IDLE;
            }

            void Execute(Scripts* enemy)
            {
                if (static_cast<Enemy*>(enemy)->_hp <= 0)
                {
                    AIState<Scripts>* newState = new DeadState();
                    enemy->GetStateMachine()->ChangeState(newState);
                    return;
                }
            }
            
            void Exit(Scripts* enemy)
            {
                UNREFERENCED_PARAMETER(enemy);
            }
        };
    }
}
