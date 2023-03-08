/* Start Header ************************************************************************/
/*!
\file DeadState.h
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
        class DeadState : public AIState<Scripts>
        {
        public:
            DeadState();
            ~DeadState();
            void Enter(Scripts* enemy);
            void Execute(Scripts* enemy);
            void Exit(Scripts* enemy);
        };
    }
}
