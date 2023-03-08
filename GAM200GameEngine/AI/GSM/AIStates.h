/* Start Header ************************************************************************/
/*!
\file AIStates.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the AI states 

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include <string>
#include "../Behaviour.h"

namespace Engine
{
    namespace AI
    {
        template<class T>
        class AIState
        {
        protected:
            std::string _stateName;
        public:
            AIState() : 
                _stateName{ "" }
            {}
            virtual ~AIState() {}
            virtual void Enter(T* character) = 0;
            virtual void Execute(T* character) = 0;
            virtual void Exit(T* character) = 0;
            std::string getStateName() { return _stateName; }
        };
    }
}

