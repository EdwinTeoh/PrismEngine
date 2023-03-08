/* Start Header ************************************************************************/
/*!
\file AISystem.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the declaration for the AI System

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once

// STL
#include <unordered_map>
#include <vector>

// Components
#include "../ECS/ComponentManager.h"
#include "../ECS/Patrol.h"
#include "../ECS/RigidBody.h"
#include "../ECS/ECS.h"
#include "Scripts/Scripts.h"

#define p_AISystem Engine::AI::AISystem::GetInstance()

namespace Engine
{
    namespace AI
    {   
        class AISystem
        {
            static AISystem* s_instance;

            int _totalEnemyNum;
            int _lastScript;

        public:
            AISystem();
            ~AISystem();
            void Init();
            void Update(int currentNumberOfSteps);
            void Exit();

            static AISystem* GetInstance();

            void AddScript(ScriptManager* new_script) 
            { 
                _scriptList.push_back(new_script);
            }
            void AI_Init();

            void DeleteAll();
            void ClearContainer();
            void ReInit();
            EntityID FindPlayer();

            std::vector<ScriptManager*> _scriptList;    // Scripts
            // std::vector<NewEnemy*> _scriptEnemyList;    // Scripts
            std::vector<ECS::Entity*> _entities;        // Throw all entities with AI inside here
        };

    }
}

