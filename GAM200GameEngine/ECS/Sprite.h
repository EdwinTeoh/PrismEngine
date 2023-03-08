#pragma once
#include "IComponent.h"

namespace Engine
{
	namespace ECS
	{
		namespace Components
		{
            class Sprite : public IComponent, public Serializer::ISerializer
            {
            public:
                bool _static;
                bool _renderable;
                bool _set;	// Turns to true when is transform has been calculated
                float _u;
                float _v;
                bool _isMultiple;
                bool _isRepeating;
                int _animatorIndex;
                unsigned _rowCount;
                unsigned _colCount;
                // Color
                Vec4D _color;
                // Texture
                std::string _texName;
                std::string _texPath;

                Sprite() : _u{ 0.f }, _v{ 0.f }, _static{ false }, _renderable{ true }, _set{ false },
                    _isMultiple{ false }, _isRepeating{ false }, _animatorIndex{ -1 }, _rowCount{ 1 }, _colCount{ 1 }
                {
                    _color = Vec4D(1.0f, 1.0f, 1.0f, 1.0f);
                    _texName = "No Texture";
                    _texPath = "No Texture";
                }

                void operator=(const Sprite& rhs)
                {
                    _u = rhs._u;
                    _v = rhs._v;
                    _color = rhs._color;
                    _texName = rhs._texName;
                    _texPath = rhs._texPath;
                }
                Sprite(const Sprite& rhs) = default;
                Sprite& operator=(Sprite&& rhs) = default;
                ~Sprite() {};

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

                Vec2D getOffset() { return Vec2D(_u, _v); }
                Sprite clone() const { return Sprite(*this); }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Sprite"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["Sprite"];
                            Sprite s;

                            _static = AddType(obj, "Static", s._static);
                            _renderable = AddType(obj, "Render", s._renderable);
                            _set = AddType(obj, "Set", s._set);
                            _u = AddType(obj, "U", s._u);
                            _v = AddType(obj, "V", s._v);

                            _isMultiple = AddType(obj, "Is Multiple", s._isMultiple);
                            _isRepeating = AddType(obj, "Is Repeating", s._isRepeating);
                            _animatorIndex = AddType(obj, "Animator", s._animatorIndex);
                            //_rowCount = AddType(obj, "Row Count", s._rowCount);
                            //_colCount = AddType(obj, "Column Count", s._colCount);

                            _color = AddType(obj, "Color", s._color);
                            //_texName = AddType(obj, "TexName", s._texName);
                            _texPath = AddType(obj, "TexPath", s._texPath);

                            return true;
                        }
                        //else
                        //{
                        //    std::cout << "EntityID " << a <<
                        //        " : No Sprite." << std::endl;
                        //    return false;
                        //}

                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its Sprite." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
                    writer->SetMaxDecimalPlaces(8);

                    writer->Key("Sprite");
                    writer->StartObject();

                    writer->String("Static");
                    writer->Bool(_static);

                    writer->String("Render");
                    writer->Bool(_renderable);

                    writer->String("Set");
                    writer->Bool(_set);

                    writer->String("U");
                    writer->Double(_u);

                    writer->String("V");
                    writer->Double(_v);

                    writer->String("Is Multiple");
                    writer->Bool(_isMultiple);

                    writer->String("Is Repeating");
                    writer->Bool(_isRepeating);

                    writer->String("Animator");
                    writer->Int(_animatorIndex);

                    //writer->String("Row Count");
                    //writer->Uint(_rowCount);

                    //writer->String("Column Count");
                    //writer->Uint(_colCount);

                    writer->String("Color");
                    Serialize4DVector(writer, _color);

                    //writer->String("TexName");
                    //writer->String(_texName.c_str());

                    writer->String("TexPath");
                    writer->String(_texPath.c_str());

                    writer->EndObject();
                 
                    return true;
                }
            };
		}
	}
}