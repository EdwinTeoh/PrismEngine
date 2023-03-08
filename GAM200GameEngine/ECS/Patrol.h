/* Start Header ************************************************************************/
/*!
\file Patrol.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the Patrol Component

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once

#include "IComponent.h"
#include "Transform.h"
#include "../AI/AISystem.h"
#include "../AI/Behaviour.h"
#include "../AI/Scripts/Enemy.h"

namespace Engine
{
    namespace ECS
    {
        namespace Components
        {
            class Patrol : public IComponent, public Serializer::ISerializer
            {
            public:
                Patrol() :
                    _type("patrol"),
                    _target(AI::Target::PLAYER),
                    _patrolTime(1.0f),
                    _internalTimer(_patrolTime),
                    _belongsTo(nullptr),
                    _patrolSpeed(1.0f),
                    _maxHP(100),
                    _currHP(_maxHP)
                {
                    _internalTimer = _patrolTime;
                }
                ~Patrol(){}

                virtual std::string GetComponentName()
                {
                    std::string className = __FUNCTION__;
                    size_t found = className.find_last_of("::") - 1;
                    className = className.substr(0, found);
                    found = className.find_last_of("::") + 1;
                    className = className.substr(found);
                    return className;
                }

                virtual void ToEditor();


                // Change to getters and setters later
                std::string _type;
                AI::Target _target;
                float _patrolTime;
                float _internalTimer;
                float _patrolSpeed;
                int _maxHP;
                int _currHP;

                AI::Enemy* _belongsTo;

                Patrol clone() const { return Patrol(*this); }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Patrol"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["Patrol"];
                            Patrol p;

                            _patrolTime = AddType(obj, "Timer", p._patrolTime);
                            _patrolSpeed = AddType(obj, "Speed", p._patrolSpeed);
                            _maxHP = AddType(obj, "HP", p._maxHP);

                            return true;
                        }
                        //else
                        //{
                        //    std::cout << "EntityID " << a <<
                        //        " : No Patrol." << std::endl;
                        //    return false;
                        //}
                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its Patrol Component." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("Patrol");

                    writer->StartObject();
                    writer->SetMaxDecimalPlaces(2);
                    writer->String("Timer");
                    writer->Double(_patrolTime);
                    writer->String("Speed");
                    writer->Double(_patrolSpeed);
                    writer->String("HP");
                    writer->Double(_maxHP);
                    writer->EndObject();

                    return true;
                }
            };
        }
    }
}