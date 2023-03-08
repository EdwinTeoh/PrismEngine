#pragma once
#include "IComponent.h"

namespace Engine
{
	namespace ECS
	{
		namespace Components
		{
            class CircleCollider : public IComponent, public Serializer::ISerializer
            {
                float _rad;
                bool _isCollided;
                bool _isTriggered;
            public:
                CircleCollider()
                    : _rad{ 1.0f },
                    _isCollided{ false },
                    _isTriggered{ false }
                {}

                virtual ~CircleCollider() = default;

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

                // Getters
                float GetRadius() { return _rad; }
                bool CheckIfCollided() const { return _isCollided; }
                bool CheckIfTriggered() const { return _isTriggered; }

                // Setters
                void SetRadius(float r) { _rad = r; }
                void SetIsCollided(const bool b) { _isCollided = b; }
                void SetIsTriggered(const bool b) { _isTriggered = b; }

                CircleCollider clone() const
                {
                    return CircleCollider(*this);
                }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {

                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("CircleCollider"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["CircleCollider"];
                            CircleCollider c;

                            _rad = AddType(obj, "Rad", c._rad);

                            return true;
                        }
                        //else
                        //{
                        //    std::cout << "EntityID " << a <<
                        //        " : No CircleCollider." << std::endl;
                        //    return false;
                        //}

                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its circleCollider." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("CircleCollider");
                    writer->SetMaxDecimalPlaces(2);
                    writer->StartObject();
                    writer->String("Rad");
                    writer->Double(_rad);
                    writer->EndObject();
                    return true;
                }
            };
		}
	}
}