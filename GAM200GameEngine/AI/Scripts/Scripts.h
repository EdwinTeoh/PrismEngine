/* Start Header ************************************************************************/
/*!
\file Scripts.h
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

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <queue>

#include "../../ECS/IComponent.h"
#include "../../ECS/Entity.h"
#include "../Behaviour.h"
#include "../GSM/StateMachines.h"

namespace Engine
{
    namespace AI
    {
        class Scripts
        {
        private:
            //private variables
            ECS::Entity* _owner;

        protected:
            //only for descendents to inherit
            StateMachine<Scripts>* _stateMachine;

        public:
            Scripts() :
                _owner{ nullptr },
                _stateMachine{ nullptr },
                _action{ Behaviour::STATE_IDLE },
                _currDir{ Direction::NONE },
                _isAlive{ true }
            {
            }

            virtual Scripts* Clone() const { return new Scripts{ *this }; }

            virtual ~Scripts() { delete _stateMachine; }

            enum class Direction
            {
                NONE,
                LEFT,
                RIGHT
            };

            // state its in
            Behaviour _action;
            Direction _currDir;        // Current Direction

            // check if enemy is alive
            bool _isAlive;

            virtual ECS::Entity* GetOwner()
            {
                return _owner;
            }

            virtual void SetOwner(ECS::Entity* owner)
            {
                _owner = owner;
            }

            //gets protected statemachine
            virtual StateMachine<Scripts>* GetStateMachine()
            {
                return _stateMachine;
            }

            virtual void Init() {}

            // Set Timer and Radius for patrol units
            virtual void Init(float time, float radius)
            {
                UNREFERENCED_PARAMETER(time);
                UNREFERENCED_PARAMETER(radius);
            };

            // inherit by the user 
            virtual void Update(float dt)
            {
                UNREFERENCED_PARAMETER(dt);
            }

        };

        class ScriptManager : public ECS::Components::IComponent
        {
        public:
            ScriptManager();
            ScriptManager(std::string scriptType);
            ~ScriptManager();

            virtual std::string GetComponentName()
            {
                std::string className = __FUNCTION__;
                size_t found = className.find_last_of("::") - 1;
                className = className.substr(0, found);
                found = className.find_last_of("::") + 1;
                className = className.substr(found);
                return className;
            }


            void Init();


            void Update(float dt);

            bool SetScriptName(std::string s);
            std::string GetScriptName() { return _scriptName; }
            Scripts* GetScript() const { return _script; }

        private:
            std::string _scriptName;
            Scripts* _script;
        };

        class ScriptFactory
        {
        public:
            void Init();
            Scripts* GetScript(std::string scriptname);
            void Destroy();
            ~ScriptFactory() { Destroy(); }

            static ScriptFactory* Instance()
            {
                if (!SF)
                    SF = new ScriptFactory();
                return SF;
            }
        private:
            static ScriptFactory* SF;
            ScriptFactory() {}
            std::map <std::string, Scripts* > _scriptMap;
        };
    }
}

