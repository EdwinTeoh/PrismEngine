/* Start Header ************************************************************************/
/*!
\file Patrolling.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the declaration for the AI Patrolling system

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PatrolState.h"

namespace Engine
{
    namespace AI
    {
        PatrolState::PatrolState()
        {
            _stateName = "Patrol State";
        }

        PatrolState::~PatrolState()
        {
        }

        void PatrolState::Enter(Scripts* enemy)
        {
            enemy->_action = Behaviour::STATE_PATROL;
        }

        void PatrolState::Execute(Scripts* enemy)
        {
            // check if dead
            // change to dead state
            if (static_cast<Enemy*>(enemy)->_hp <= 0)
            {
                AIState<Scripts>* newState = new DeadState();
                enemy->GetStateMachine()->ChangeState(newState);
                return;
            }

            else if (static_cast<Enemy*>(enemy)->_isStunned)
            {
                static_cast<Enemy*>(enemy)->Stunned(2.0f);
            }

            else if (!static_cast<Enemy*>(enemy)->IsInRange())
            {
                static_cast<Enemy*>(enemy)->Patrol();
            }
            else
            {
                AIState<Scripts>* newState = new ChaseState();
                enemy->GetStateMachine()->ChangeState(newState);
                return;
            }
        }

        void PatrolState::Exit(Scripts* enemy)
        {
            UNREFERENCED_PARAMETER(enemy);
        }
    }
}