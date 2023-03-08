/*******************************************************************************
filename    Vacuumable.h
author(s)   Nicolette Chan, c.munlengnicolette
date        Jan 29, 2020
brief

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "IComponent.h"
#include "Factory.h"

namespace Engine
{
    namespace ECS
    {
        namespace Components
        {
            class Vacuumable : public IComponent, public Serializer::ISerializer
            {
                bool _isActive;
            public:
                Vacuumable() :
                    _isActive(false)
                {
                }

                virtual ~Vacuumable() = default;
                
                virtual std::string GetComponentName()
                {
                    std::string className = __FUNCTION__;
                    size_t found = className.find_last_of("::") - 1;
                    className = className.substr(0, found);
                    found = className.find_last_of("::") + 1;
                    className = className.substr(found);
                    return className;
                }

                // virtual void ToEditor();

                Vacuumable clone() const { return Vacuumable(*this); }

                // Getters
                bool GetActive() { return _isActive; }

                // Setters
                void ToggleActive() { _isActive = !_isActive ; }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Vacuumable"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["Vacuumable"];
                            Vacuumable v;
                            _isActive = AddType(obj, "isActive", v._isActive);

                            return true;
                        }
                        else
                        {
                            std::cout << "EntityID " << a <<
                                " : cannot find the entity and its Vacuumable." << std::endl;
                            return false;
                        }
                    }
                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("Vacuumable");
                    writer->StartObject();

                    writer->String("isActive");
                    writer->Bool(_isActive);

                    writer->EndObject();

                    return true;
                }
            };
        }
    }
}