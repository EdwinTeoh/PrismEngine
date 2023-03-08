/* Start Header ************************************************************************/
/*!
\file PatrolState.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the AI Patrol State

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "AIStates.h"
#include "../Scripts/Scripts.h"
#include "DeadState.h"
#include "ChaseState.h"

namespace Engine
{
    namespace AI
    {
        class PatrolState : public AIState<Scripts>
        {
        public:
            PatrolState();
            ~PatrolState();
            void Enter(Scripts* enemy);
            void Execute(Scripts* enemy);
            void Exit(Scripts* enemy);
        };
    }
}
