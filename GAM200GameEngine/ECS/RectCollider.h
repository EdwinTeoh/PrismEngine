#pragma once
#include "IComponent.h"
#include "Factory.h"
#include <bitset>

namespace Engine
{
	namespace ECS
	{
		namespace Components
		{
            class RectCollider : public IComponent, public Serializer::ISerializer
            {
                bool _isCollided;
                bool _isTriggered;
                bool _collidedWithClimbWall;
            public:
                Vec2D _offset;
                float _height;
                float _width;

                RectCollider() :
                    _height{ 1.0f },
                    _width{ 1.0f },
                    _offset(),
                    _isCollided{ false },
                    _isTriggered{ false },
                    _collidedWithClimbWall{false}
                {}

                virtual ~RectCollider() = default;

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
                float GetHeight() const { return _height; }
                float GetWidth() const { return _width; }
                Vec2D GetOffSet() const { return _offset; }
                bool CheckIfCollided() const { return _isCollided; }
                bool CheckIfTriggered() const { return _isTriggered; }
                bool CollidedWithClimbWall(){ return _collidedWithClimbWall; }

                // Setters
                void SetSizeAndOffset(const Vec4D& v)
                {
                    _width = v._x;
                    _height = v._y;
                    _offset._x = v._z;
                    _offset._y = v._w;
                }
                void SetSize(const Vec2D& size)
                {
                    _width = size._x;
                    _height = size._y;
                }
                void SetSize(const float& w, const float& h)
                {
                    _width = w;
                    _height = h;
                }
                void SetHeight(const float h) { _height = h; }
                void SetWidth(const float w) { _width = w; }
                void SetOffSet(const Vec2D offset) { _offset = offset; }
                void SetIsCollided(const bool b) { _isCollided = b; }
                void SetIsTriggered(const bool b) { _isTriggered = b; }
                void SetCollidedWithWall(const bool b) { _collidedWithClimbWall = b; }

                enum class Direction
                {
                    NONE,
                    LEFT,
                    RIGHT
                };
                Direction _direction = Direction::NONE;

                enum CollisionBitflag
                {
                    NONE_ = 0x00,
                    UP___ = 0x01,
                    DOWN_ = 0x02,
                    LEFT_ = 0x04,
                    RIGHT = 0x08
                };
                unsigned _collisionBitflag = CollisionBitflag::NONE_;

                std::string CollisionBitflagString()
                {
                    return std::bitset<8>(_collisionBitflag).to_string();
                }

                // Check to see if need to ignore collision
                bool _isIgnored;

                RectCollider clone() const
                {
                    return RectCollider(*this);
                }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("RectCollider"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["RectCollider"];
                            
                            RectCollider r;

                            _height = AddType(obj, "Height", r._height);
                            _width = AddType(obj, "Width", r._width);
                            _offset = AddType(obj, "Offset", r._offset);
                            _isCollided = AddType(obj, "Collided", r._isCollided);
                            _isTriggered = AddType(obj, "Triggered", r._isTriggered);
                            
                            return true;
                        }
                        //else
                        //{
                        //    std::cout << "EntityID " << a <<
                        //        " : No RectCollider." << std::endl;
                        //    return false;
                        //}

                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its RectCollider." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("RectCollider");
                    writer->StartObject();

                    writer->SetMaxDecimalPlaces(3);
                    writer->String("Height");
                    writer->Double(_height);

                    writer->String("Width");
                    writer->Double(_width);

                    writer->String("Offset");
                    SerializeVector(writer, _offset);

                    writer->String("Collided");
                    writer->Bool(_isCollided);

                    writer->String("Triggered");
                    writer->Bool(_isTriggered);

                    writer->EndObject();
                  
                    return true;
                }
            };
		}
	}
}