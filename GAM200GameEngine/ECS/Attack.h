/*******************************************************************************
filename    Attack.h
author(s)   Zhang Xinyu,xinyu.z@digipen.edu
date        Dec 5, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "IComponent.h"
#include "Transform.h"

namespace Engine
{
    namespace ECS
    {
        namespace Components
        {
            class Attack : public IComponent, public Serializer::ISerializer
            {
            public:
                int _damage;
                bool _isActive;
                bool _isEnemy;

                Attack() :
                    _damage{ 10 },
                    _isActive{ false },
                    _isEnemy{false}
                {}

                ~Attack() {}
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

                //   virtual void ToEditor();

                Attack clone() const { return Attack(*this); }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Attack"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["Attack"];
                            Attack attack;
                                
                            _damage = AddType(obj, "Damage", attack._damage);
                            _isActive = AddType(obj, "isActive", attack._isActive);
                            return true;
                        }
                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its Attack Component." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("Attack");
                    writer->StartObject();
                    writer->SetMaxDecimalPlaces(2);
                    writer->String("Damage");
                    writer->Double(_damage);

                    writer->String("isActive");
                    writer->Bool(_isActive);
         
                    writer->EndObject();

                    return true;
                }
            };
        }
    }
}

