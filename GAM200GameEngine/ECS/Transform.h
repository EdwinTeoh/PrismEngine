#pragma once
#include "IComponent.h"
#include "Factory.h"

namespace Engine
{
	namespace ECS
	{
		namespace Components
		{
            class Transform : public IComponent, public Serializer::ISerializer
            {
                bool _edited;
                Matx4 resultantMatrix;

            public:
                float _rotate;
                Vec2D _scale;
                Vec2D _position;
                float _z;
                EditOperation _CurrentEditOp;

                void calModelMatrix()
                {
                    if (_rotate == 0.0f)
                    {
                        //CHECK IF GOT PROBLEM
                        resultantMatrix = Matx4(1.0f);
                        Matx4 transM = Matx4();
                        Matx4 scaleM = Matx4();
                        Mtx44Translate(transM, _position._x, _position._y);
                        Mtx44Scale(scaleM, _scale._x, _scale._y);
                        resultantMatrix = transM * scaleM * resultantMatrix;
                    }
                    else
                    {
                        //CHECK IF GOT PROBLEM
                        resultantMatrix = Matx4(1.0f);
                        Matx4 transM = Matx4();
                        Matx4 scaleM = Matx4();
                        Matx4 rotM = Matx4();
                        Mtx44Translate(transM, _position._x, _position._y);
                        Mtx44RotRad(rotM, _rotate);
                        Mtx44Scale(scaleM, _scale._x, _scale._y);
                        resultantMatrix = transM * rotM * scaleM * resultantMatrix;
                    }
                }

                inline Matx4& getMTX() { calModelMatrix(); return resultantMatrix; }
                inline Vec3D getPos() const { return Vec3D(_position._x, _position._y, _z); }

                /// <summary>
                /// Default ctor for transform component
                /// </summary>
                Transform() :
                    _scale(),
                    _rotate{ 0 },
                    _position(),
                    _z{ 0.0f },
                    _edited{ true },
                    _CurrentEditOp{ EditOperation::Nil }
                {
                    _scale._x = 1.0f;
                    _scale._y = 1.0f;
                    resultantMatrix = Matx4(1.0f);
                }

                Transform(float zIndex) :
                    _scale(),
                    _rotate{ 0 },
                    _position(),
                    _z{ zIndex },
                    _edited{ true }
                {
                    _scale._x = 1.0f;
                    _scale._y = 1.0f;
                    resultantMatrix = Matx4(1.0f);
                }

                /// <summary>
                /// default dctor for transform component
                /// </summary>
                virtual ~Transform() = default;

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

                /// <summary>
                /// returns a clone copy of transform componente
                /// </summary>
                /// <returns></returns>
                Transform clone() const
                {
                    return Transform(*this);
                }

               
                EditOperation DeserializeEditOp(const rapidjson::Value& val)
                {
                    EditOperation e = EditOperation::Nil;
                    std::string getValue = val.GetString();

                    if (getValue == "Translate")
                    {
                        e = EditOperation::Translate;
                    }
                    else if (getValue == "Rotate")
                    {
                        e = EditOperation::Rotate;
                    }
                    else if (getValue == "Scale")
                    {
                        e = EditOperation::Scale;
                    }
                    return e;
                }

                bool SerializeEditOp(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer,
                    const EditOperation eo)const
                {
                    std::string value;

                    if (eo == EditOperation::Nil)
                    {
                        value = "Nil";
                    }
                    else if (eo == EditOperation::Translate)
                    {
                        value = "Translate";
                    }
                    else if (eo == EditOperation::Rotate)
                    {
                        value = "Rotate";

                    }
                    else if (eo == EditOperation::Scale)
                    { 
                        value = "Scale";
                    }
                    writer->String(value.c_str());
                    return true;
                }
                
                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Transform"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["Transform"];
                            Transform t;

                            _edited = AddType(obj, "Edited", t._edited);
                            _rotate = AddType(obj, "Rot", t._rotate);
                            _scale = AddType(obj, "Scale", t._scale);
                            _position = AddType(obj, "Pos", t._position);
                            _z = AddType(obj, "Z", t._z);

                            if (obj.HasMember("CurrEditOp"))
                            {
                                _CurrentEditOp = DeserializeEditOp(obj["CurrEditOp"]);
                            }
                            else
                            {
                                // Wrong name or forget to fill in
                                _CurrentEditOp = t._CurrentEditOp;
                                return true;
                            }
                            return true;
                        }
                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its Transform." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer)const
                {

                    writer->String("Transform");

                    writer->StartObject();
                    writer->String("Edited");
                    writer->Bool(_edited);

                  /*  writer->String("resultantMatrix");
                    SerializeMatrix(writer, resultantMatrix);*/

                    writer->String("Rot");
                    writer->Double(_rotate);

                    writer->String("Scale");
                    SerializeVector(writer, _scale);

                    writer->String("Pos");
                    SerializeVector(writer, _position);

                    writer->String("Z");
                    writer->Double(_z);

                    writer->String("CurrEditOp");
                    SerializeEditOp(writer, _CurrentEditOp);

                    writer->EndObject();
                   
                    return true;

                }

            };
		}
	}
}