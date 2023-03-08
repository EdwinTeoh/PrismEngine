/* Start Header ************************************************************************/
/*!
\file StateMachines.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
	This file contains the declaration for the State Machine

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "../../ECS/Entity.h"
#include "AIStates.h"

namespace Engine
{
	namespace AI
	{
		template<class character_type>
		class StateMachine
		{
			AIState<character_type>* currentState;
			AIState<character_type>* previousState;
			AIState<character_type>* globalState;
			character_type* owner;

		public:
			AIState<character_type>* getCurrentState() { return currentState; }
			StateMachine(character_type* owner) :
				currentState{ nullptr },
				previousState{ nullptr },
				globalState{ nullptr },
				owner{ owner }
			{
			}

			~StateMachine()
			{
				if (globalState != nullptr)
				{
					delete globalState;
					globalState = 0;
				}
				if (currentState != nullptr)
				{
					delete currentState;
					currentState = 0;
				}
				if (previousState != nullptr)
				{
					delete previousState;
					previousState = 0;
				}
			}

			void Update()
			{
				if (currentState == nullptr)
				{
					globalState->Execute(owner);
				}
				else if (currentState)
				{
					currentState->Execute(owner);
				}
			}

			void ChangeState(AIState<character_type>* state)
			{
				previousState = currentState;
				if (currentState)
				{
					currentState->Exit(owner);
					delete currentState;
					currentState = 0;
				}
				currentState = state;
				if (currentState)
				{
					currentState->Enter(owner);
				}
			}

			void RevertPreviousState()
			{
				ChangeState(previousState);
			}

			void SetGlobalState(AIState<character_type>* state)
			{
				if (globalState)
				{
					delete globalState;
					globalState = 0;
				}
				globalState = state;
				if (globalState)
				{
					globalState->Enter(owner);
				}
			}

			void SetPreviousState(AIState<character_type>* state)
			{
				previousState = state;
			}

			void SetOwner(character_type* set) { owner = set; }
			character_type* GetOwner() const { return owner; }
		};
	}
}
