/* Start Header ************************************************************************/
/*!
\file AISystem.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the AI System

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "../Clock/Clock.h"
#include "AISystem.h"
#include "../ECS/Factory.h"
#include "../ECS/VacuumC.h"
#include "../GameLogic/GameLogic.h"

namespace Engine
{
    namespace AI
    {
        AISystem* AISystem::s_instance;

        AISystem::AISystem() :
            _totalEnemyNum(0),
            _lastScript(0),
            _scriptList(0), 
            _entities(0)
        {
        }

        AISystem* AISystem::GetInstance()
        {
            if (!s_instance)
            {
                s_instance = new AISystem;
            }

            return s_instance;
        }

        void AISystem::Init()
        {
            ScriptFactory::Instance()->Init();
        }

        AISystem::~AISystem()
        {
        }

        void AISystem::AI_Init()
        {
            _lastScript = (int)_scriptList.size() - 1;

            for (size_t i = 0; i < _scriptList.size(); i++)
            {
                if (_scriptList[i]->GetScript() != nullptr)
                {
                    if (_scriptList[i]->GetScriptName() == "patrol")
                    {
                        _scriptList[i]->Init();
                    }
                    if (_scriptList[i]->GetScriptName() == "vacuum")
                    {
                        _scriptList[i]->Init();
                    }
                }
            }
        }

        void AISystem::ReInit()
        {
            _scriptList.clear();
            if (ECS::compManager->CheckForList<ECS::Components::Patrol>())
            {
                if (ECS::compManager->GetComponentList<ECS::Components::Patrol>().GetList().size() > 0)
                {
                    std::map<EntityID, PatrolComponent>& list
                        = ECS::compManager->GetComponentList<PatrolComponent>().GetList();

                    for (auto i = list.begin(); i != list.end(); ++i)
                    {
                        ECS::Entity& e = ECS::entityManager->FindEntity(i->first);
                        auto script = new AI::ScriptManager(i->second._type);
                        script->GetScript()->SetOwner(&e);
                        p_AISystem->AddScript(script);
                    }
                }
            }

            if (ECS::compManager->CheckForList<VacuumComponent>())
            {
                if (ECS::compManager->GetComponentList<VacuumComponent>().GetList().size() > 0)
                {
                    std::map<EntityID, VacuumComponent>& list
                        = ECS::compManager->GetComponentList<VacuumComponent>().GetList();

                    for (auto i = list.begin(); i != list.end(); ++i)
                    {
                        ECS::Entity& e = ECS::entityManager->FindEntity(i->first);
                        auto script = new AI::ScriptManager(i->second._type);
                        script->GetScript()->SetOwner(&e);
                        p_AISystem->AddScript(script);
                    }
                }
            }

            AI_Init();
        }

        void AISystem::Update(int currentNumberOfSteps)
        {
            Utility::Clock::GetInstance().Start(Utility::SystemCode::AI);
            Utility::Clock::GetInstance().GetDT(Utility::SystemCode::AI);
            float dt = 1.0f / 60.0f;

            if (Player::GetInstance()->_endState != 0)
                return;

            if (ECS::compManager->CheckForList<ECS::Components::Patrol>())
            {
                if (ECS::compManager->GetComponentList<ECS::Components::Patrol>().GetList().size() > 0)
                {
                    for (int step = 0; step < currentNumberOfSteps; step++)
                    {
                        for (int k = 0; k < _scriptList.size(); ++k)
                        {
                            if(_scriptList[k]->GetScriptName() == "patrol")
                                _scriptList[k]->Update(dt);
                        }
                    }
                }
                
            }

            if (ECS::compManager->CheckForList<ECS::Components::VacuumC>())
            {
                if (ECS::compManager->GetComponentList<ECS::Components::VacuumC>().GetList().size() > 0)
                {
                    for (int step = 0; step < currentNumberOfSteps; step++)
                    {
                        for (int k = 0; k < _scriptList.size(); ++k)
                        {
                            if (_scriptList[k]->GetScriptName() == "vacuum")
                                _scriptList[k]->Update(dt);
                        }
                    }
                }
            }

            Utility::Clock::GetInstance().Update(Utility::SystemCode::AI);
        }



        void AISystem::DeleteAll()
        {
            for (size_t i = 0; i < _scriptList.size(); ++i)
            {
                if (_scriptList[i])
                {
                    delete _scriptList[i];
                    _scriptList[i] = nullptr;
                }
            }
            _scriptList.clear();
            _entities.clear();
        }

        void AISystem::ClearContainer()
        {
            for (auto it = _scriptList.begin(); it != _scriptList.end(); ++it)
            {
                if (*it == nullptr)
                {
                    _scriptList.erase(it);
                }
            }
        }

        EntityID AISystem::FindPlayer()
        {
            std::vector<ECS::Entity>& list = ECS::entityManager->GetEntityList();
            for (int i = 0; i < list.size(); ++i)
            {
                ECS::Entity& tmp = ECS::entityManager->FindEntity(static_cast<EntityID>(i));
                if (tmp.GetName() == "Player")
                {
                    return static_cast<EntityID>(i);
                }
            }
            return 0;
        }

        void AISystem::Exit()
        {
            if (ScriptFactory::Instance() != nullptr)
                delete ScriptFactory::Instance();
            DeleteAll();;
            delete s_instance;
        }

    }
}