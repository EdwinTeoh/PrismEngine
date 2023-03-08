#pragma once
#include "IComponent.h"

namespace Engine
{
	namespace ECS
	{
		namespace Components
		{
            class LineCollider : public IComponent, public Serializer::ISerializer
            {
                float _length;
                Vec2D _endpoint;
                Vec2D _normal;
                bool _isCollided;
                bool _isTriggered;
            public:
                LineCollider(float l = 0.0f, Vec2D ep = {0.0f, 0.0f}, Vec2D n = { 0.0f, 0.0f }, bool c = false, bool t = false) 
                    : _length(l),
                      _endpoint(ep),
                      _normal(n),
                      _isCollided(c),
                      _isTriggered(t)
                { }

                virtual ~LineCollider() = default;

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
                float GetLength() const { return _length; }
                Vec2D GetEndPt() const { return _endpoint; }
                Vec2D GetNormal() const { return _normal; }
                bool CheckIfCollided() const { return _isCollided; }
                bool CheckIfTriggered() const { return _isTriggered; }
                // Setters
                void SetLength(const float l) { _length = l; }
                void SetEndPt(const Vec2D ep) { _endpoint = ep; }
                void SetNormal(const Vec2D n) { _normal = n; }
                void SetIsCollided(const bool b) { _isCollided = b; }
                void SetIsTriggered(const bool b) { _isTriggered = b; }

                LineCollider clone() const
                {
                    return LineCollider(*this);
                }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {

                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("LineColllider"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["LineColllider"];
                            LineCollider l;

                            _length = AddType(obj, "Length", l._length);
                            _endpoint = AddType(obj, "Endpoint", l._endpoint);
                            _normal = AddType(obj, "Normal", l._normal);
                            _isCollided = AddType(obj, "IsCollided", l._isCollided);
                            _isTriggered = AddType(obj, "IsTriggered", l._isTriggered);

                            return true;
                        }
                        //else
                        //{
                        //    std::cout << "EntityID " << a <<
                        //        " : No LineColllider." << std::endl;
                        //    return false;
                        //}

                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its LineColllider." << std::endl;
                        return false;
                    }

                    return false;
                }
                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("LineColllider");

                    writer->StartObject();
                    writer->SetMaxDecimalPlaces(2);
                    writer->String("Length");
                    writer->Double(_length);

                    writer->String("Length");
                    SerializeVector(writer, _length);

                    writer->String("Normal");
                    SerializeVector(writer, _normal);

                    writer->String("IsCollided");
                    writer->Bool(_isCollided);

                    writer->String("IsTriggered");
                    writer->Bool(_isTriggered);


                    writer->EndObject();
                  
                    return true;
                }
            };
		}
	}
}