/*******************************************************************************
filename    ObstacleComponent.h
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        December 09, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once
#include "IComponent.h"

namespace Engine
{
    namespace ECS
    {
        namespace Components
        {
            class ObstacleComponent : public IComponent, public Serializer::ISerializer
            {
            public:
                ObstacleComponent() {}

                ~ObstacleComponent() {}
          

                virtual std::string GetComponentName()
                {
                    std::string className = __FUNCTION__;
                    size_t found = className.find_last_of("::") - 1;
                    className = className.substr(0, found);
                    found = className.find_last_of("::") + 1;
                    className = className.substr(found);
                    return className;
                }

                //   virtual void ToEditor();

                ObstacleComponent clone() const { return ObstacleComponent(*this); }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Obstacle"))
                        {
                            return true;
                        }
                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its Obstacle Component." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("Obstacle");
                    writer->StartObject();

                    writer->EndObject();

                    return true;
                }

            };
        }
    }
}