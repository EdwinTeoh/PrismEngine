/*******************************************************************************
filename    Platform.h
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
            enum struct Direction
            {
                NONE,
                UP,
                DOWN,
                LEFT,
                RIGHT,
            };

            class Platform : public IComponent, public Serializer::ISerializer
            {
                Vec2D _direction;
                Vec2D _trapBar;
            public:
                float _timer;
                bool _moveBack;
                float _speed;
                EntityID _buttonID;

                Platform() :_speed{ 0.8f },
                    _trapBar{ 0,0 },
                    _timer{ 2.0f },
                    _moveBack{ true },
                    _direction({0,0}),
                    _buttonID(static_cast<EntityID>(-1)){}

                ~Platform() {}
                void setDistance(Vec2D x) { _direction = x; }
                void setTrapBar(Vec2D x) { _trapBar = x; }
                void setTimer(float t) { _timer = t; }

                Vec2D getDistance() { return _direction; }
                Vec2D getTrapBar() { return _trapBar; }
                float getTimer() { return _timer; }

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

                Platform clone() const { return Platform(*this); }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("Platform"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["Platform"];
                            Platform p;
                     
                            _timer = AddType(obj, "Timer", p._timer);
                            _speed = AddType(obj, "Speed", p._speed); 
                            _direction = AddType(obj, "Direction", p._direction);
                            _moveBack = AddType(obj, "MoveBack", p._moveBack);
                            _buttonID = (EntityID)AddType(obj, "ButtonID", p._buttonID);
                            _trapBar = AddType(obj, "TrapBarID", p._trapBar);
                            return true;
                        }

                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its Platform Component." << std::endl;
                        return false;
                    }

                    return false;
                }

                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {

                    writer->Key("Platform");
                    writer->StartObject();

                    writer->SetMaxDecimalPlaces(2);
                 
                    writer->String("Timer");
                    writer->Double(_timer);
                    writer->String("Speed");
                    writer->Double(_speed);
                    writer->String("Direction");
                    SerializeVector(writer, _direction);
                    writer->String("MoveBack");
                    writer->Bool(_moveBack);
                    writer->String("ButtonID");
                    writer->Int(_buttonID);
                    writer->String("TrapBarID");
                    SerializeVector(writer, _trapBar);

                    writer->EndObject();

                    return true;
                }
            };
        }
    }
}
