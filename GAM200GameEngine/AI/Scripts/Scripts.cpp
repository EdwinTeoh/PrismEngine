/* Start Header ************************************************************************/
/*!
\file Scripts.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the Scripts class

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Scripts.h"
#include "Enemy.h"
#include "../AISystem.h"
#include "Vacuum.h"

namespace Engine
{
    namespace AI
    {
        ScriptFactory* ScriptFactory::SF;

        void ScriptManager::Update(float dt)
        {
            if (GetScriptName() == "patrol" &&
                _script->GetStateMachine() == nullptr)
            {
                _script->Init();
            }

            else if (GetScriptName() == "vacuum" &&
                _script->GetStateMachine() == nullptr)
            {
                _script->Init();
            }

            _script->Update(dt);
        }

        ScriptManager::ScriptManager()
            :_scriptName{ std::string {"Uninitialize" } }, _script{ nullptr }
        {
        }

        ScriptManager::ScriptManager(std::string script_name)
        {
            _scriptName = script_name;
            if (_scriptName.compare("Uninitialize") == 0)
            {
                _script = nullptr;
                return;
            }

            _script = ScriptFactory::Instance()->GetScript(script_name);
            if (_script->GetOwner() != nullptr)
            {
                p_AISystem->AddScript(this);
            }
        }

        ScriptManager::~ScriptManager()
        {
            delete _script;
        }

        void ScriptManager::Init()
        {
            _script->Init();
        }

        void ScriptFactory::Init()
        {
            _scriptMap["patrol"] = new Enemy();
            _scriptMap["vacuum"] = new Vacuum();
        }

        bool ScriptManager::SetScriptName(std::string script_name)
        {
            _script = ScriptFactory::Instance()->GetScript(script_name);
            if (_script != nullptr)
            {
                _scriptName = script_name;

                p_AISystem->AddScript(this);
                p_AISystem->AI_Init();
                return true;
            }
            else
                return false;
        }

        Scripts* ScriptFactory::GetScript(std::string script_name)
        {

            std::map<std::string, Scripts*>::iterator found = _scriptMap.find(script_name);
            if (found != _scriptMap.end())
            {
                Scripts* correctScript = _scriptMap[script_name];
                return correctScript->Clone();
            }
            else
            {
                return nullptr;
            }
        }

        void ScriptFactory::Destroy()
        {
            for (std::map<std::string, Scripts*>::iterator itr =
                _scriptMap.begin(); itr != _scriptMap.end(); itr++)
            {
                delete itr->second; // deletes the memory of the Scripts
            }
        }
    }
}
