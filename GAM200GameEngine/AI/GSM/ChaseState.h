/* Start Header ************************************************************************/
/*!
\file ChaseState.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the AI Patrolling system

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "AIStates.h"
#include "PatrolState.h"
#include "../Scripts/Scripts.h"
#include "../AISystem.h"

namespace Engine
{
    namespace AI
    {
        class ChaseState : public AIState<Scripts>
        {
        public:
            ChaseState();
            ~ChaseState();
            void Enter(Scripts* enemy);
            void Execute(Scripts* enemy);
            void Exit(Scripts* enemy);

            EntityID _playerID;
        };
    }
}
