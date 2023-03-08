/* Start Header ************************************************************************/
/*!
\file IScript.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the AI Enemy Component

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "IComponent.h"
#include "../AI/AISystem.h"
#include "../AI/Behaviour.h"
#include "../AI/Scripts/Vacuum.h"

namespace Engine
{
    namespace ECS
    {
        namespace Components
        {
            class VacuumC : public IComponent, public Serializer::ISerializer
            {
            public:
                std::string _type;
                Vec2D _suckForce;                   // Vacuum suction force
                Vec2D _blowForce;                   // Vacuum blowing force
                AI::VacuumState _vacuumState;
                bool _isHolding;                    // Check if vacuum is holding
                EntityID _holdObjID;                // The id of the obj
                EntityID _parent;

                VacuumC() :
                    _type("vacuum"),
                    _suckForce({1.0f, 1.0f}),
                    _blowForce({ 1.0f, 1.0f }),
                    _vacuumState(AI::VacuumState::OFF),
                    _isHolding(false),
                    _holdObjID((EntityID)-1),
                    _parent((EntityID)-1)
                {
                }

                VacuumC clone() const { return VacuumC(*this);  };

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

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Vacuum"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["Vacuum"];
                            VacuumC v;

                            _suckForce = AddType(obj, "SuckForce", v._suckForce);
                            _blowForce = AddType(obj, "BlowForce", v._blowForce);
                            _parent = (EntityID) AddType(obj, "Parent", v._parent);

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
                    writer->Key("Vacuum");

                    writer->StartObject();

                    writer->String("SuckForce");
                    SerializeVector(writer, _suckForce);
                    writer->String("BlowForce");
                    SerializeVector(writer, _blowForce);
                    writer->String("Parent");
                    writer->Int(_parent);

                    writer->EndObject();

                    return true;
                }
            };
        }
    }
}