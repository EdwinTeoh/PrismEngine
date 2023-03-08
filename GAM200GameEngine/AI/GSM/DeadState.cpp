/* Start Header ************************************************************************/
/*!
\file DeadState.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Nov 12, 2020
\brief
    This file contains the declaration for the AI Patrol State

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "DeadState.h"
#include "ChaseState.h"

namespace Engine
{
    namespace AI
    {
        DeadState::DeadState()
        {
            _stateName = "Dead State";
        }
        
        DeadState::~DeadState()
        {
        
        }
        
        void DeadState::Enter(Scripts* enemy)
        {
            enemy->_action = Behaviour::STATE_DEAD;
        }
        
        void DeadState::Execute(Scripts* enemy)
        {
            // ===== Change this to dead animation sprite ===== //
            static_cast<Enemy*>(enemy)->Dead();
            enemy->GetStateMachine()->ChangeState(nullptr);
            enemy->GetStateMachine()->SetPreviousState(nullptr);
        }
        
        void DeadState::Exit(Scripts* enemy)
        {
            // Delete from Factory ???
            UNREFERENCED_PARAMETER(enemy);
        }
    }
}

