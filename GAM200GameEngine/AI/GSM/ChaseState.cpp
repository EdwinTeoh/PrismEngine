/* Start Header ************************************************************************/
/*!
\file ChaseState.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the AI Chase State

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "ChaseState.h"
#include "../AISystem.h"
#include "../../Physics/Collision.h"
#include "../../GameLogic/GameLogic.h"

namespace Engine
{
    namespace AI
    {
        ChaseState::ChaseState()
        {
            _stateName = "Chase State";
        }

        ChaseState::~ChaseState()
        {
        }

        void ChaseState::Enter(Scripts* enemy)
        {
            enemy->_action = Behaviour::STATE_CHASE;
            _playerID = p_AISystem->FindPlayer();
        }

        void ChaseState::Execute(Scripts* enemy)
        {
            if (static_cast<Enemy*>(enemy)->_hp <= 0)
            {
                AIState<Scripts>* newState = new DeadState();
                enemy->GetStateMachine()->ChangeState(newState);
                return;
            }

            else if (static_cast<Enemy*>(enemy)->_isStunned)
            {
                static_cast<Enemy*>(enemy)->Stunned(4.0f);
            }

            else if (static_cast<Enemy*>(enemy)->DealDamage())
            {
                static_cast<Enemy*>(enemy)->Attack(0.8f);

                if (static_cast<Enemy*>(enemy)->_isAttacking)
                {
                    if (!Player::GetInstance()->_isImmune)
                    {
                        static_cast<Enemy*>(enemy)->_isAttacking = false;
                        static_cast<Enemy*>(enemy)->GetKnockBack();
                    }
                }
            }

            else if (!static_cast<Enemy*>(enemy)->IsInRange())
            {
                static_cast<Enemy*>(enemy)->Patrol();
            }

            // Target is still in circle
            else
            {
                static_cast<Enemy*>(enemy)->ChaseTarget();
            }
        }

        void ChaseState::Exit(Scripts* enemy)
        {
            UNREFERENCED_PARAMETER(enemy);
        }
    }
}