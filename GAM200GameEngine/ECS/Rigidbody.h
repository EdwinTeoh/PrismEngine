#pragma once
#include "IComponent.h"
#include "../Physics/Forces.h"

namespace Engine
{
    namespace ECS
    {
        namespace Components
        {
            class RigidBody2D : public IComponent, public Serializer::ISerializer
            {
                Vec2D _velocity;
                Vec2D _acceleration;
                Vec2D _totalForce;
                Vec2D _totalRotForce;
                bool _isGravity;
                float _gravityScale;
                float _maxVel;
                float _mass;
                float _restitution;
                float _dragScale;

                // Forces stuff
                Vec2D _leftForce;
                Vec2D _rightForce;
                Vec2D _jumpForce;
                float _movementSpeed;
                bool _moveLeft;
                bool _doJump;
                bool _moveRight;
                bool _climb;
                bool _crawl;

            public:
                RigidBody2D() :
                    _velocity(0),
                    _acceleration(0),
                    _totalForce(0),
                    _totalRotForce(0),
                    _isGravity(true),
                    _gravityScale{ 1 },
                    _maxVel{ 100.0f },
                    _mass{ 1.0f },
                    _restitution{ 0.5f },
                    _dragScale{ 0.5f },
                    _leftForce{ 0 },
                    _rightForce{ 0 },
                    _jumpForce{ 0 },
                    _movementSpeed{ 1 },
                    _moveLeft{ false },
                    _doJump{ false },
                    _moveRight{ false },
                    _climb{ false },
                    _crawl{ false }
                {
                    
                }

                virtual ~RigidBody2D()
                {
                    ClearAllForces();
                }

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

                // Give each Rigidbody their own force list
                std::vector<Force> _forceList;

                // Add additional forces
                void AddForce(Force force)
                {
                    force.ActivateForce();
                    _forceList.push_back(force);
                }

                // Activate the force base on type
                void AddForce(Force::ForceType type)
                {
                    _forceList[static_cast<int>(type)].ActivateForce();
                }

                // Clears force at a position
                // Checks if pos less than 2
                void ClearForce(int pos)
                {
                    if(pos >= 3)
                        _forceList.erase(_forceList.begin() + pos);
                }

                // Clears any added forces
                // without clearing the initial forces
                void ClearAddedForces()
                {
                    // If theres only 3 forces then
                    // no need to clear
                    if (_forceList.size() == 3)
                        return;

                    else
                    {
                        while (_forceList.size() != 3)
                            _forceList.pop_back();
                    }
                }

                void ClearAllForces()
                {
                    _forceList.clear();
                }

                // Getters
                Vec2D GetVelocity() { return _velocity; }
                Vec2D GetAcceleration() const { return _acceleration; }
                Vec2D GetTotalForce() const { return _totalForce; }
                Vec2D GetTotalRotForce() const { return _totalRotForce; }
                bool CheckIfGravity() const { return _isGravity; }
                float GetGravityScale() const { return _gravityScale; }
                float GetMaxVel() const { return _maxVel; }
                float GetMass() const { return _mass; }
                float GetRestitution() const { return _restitution; }
                float GetDragScale() const { return _dragScale; }

                // Force
                Vec2D GetLeftForce() const { return _leftForce; }
                Vec2D GetJumpForce() const { return _jumpForce; }
                Vec2D GetRightForce() const { return _rightForce; }
                float GetMovementSpeed() const { return _movementSpeed; }

                bool CheckIfLeft() const { return _moveLeft; }
                bool CheckIfJump() const { return _doJump; }
                bool CheckIfRight() const { return _moveRight; }
            

                // Setters
                void SetVelocity(const Vec2D v) { _velocity = v; };
                void SetAcceleration(const Vec2D a) { _acceleration = a; };
                void SetTotalForce(const Vec2D t) { _totalForce = t; }
                void SetTotalRotForce(const Vec2D rt) { _totalRotForce = rt; }
                void SetIsGravity(const bool b) { _isGravity = b; };
                void SetGravityScale(const float g) { _gravityScale = g; }
                void SetMaxVel(const float mV) { _maxVel = mV; };
                void SetMass(const float m) { _mass = m; };
                void SetRestitution(const float r) { _restitution = r; }

                void SetIsLeft(const bool b) { _moveLeft = b; }
                void SetIsJump(const bool b) { _doJump = b; }
                void SetIsRight(const bool b) { _moveRight = b; }
                void SetJumpForce(const Vec2D jumpForce) { _jumpForce = jumpForce; };
                void SetLeftForce(const Vec2D leftForce) { _jumpForce = leftForce; };
                void SetRightForce(const Vec2D rightForce) { _jumpForce = rightForce; };
                void SetMovementSpeed(const float s) { _movementSpeed = s; }
          
                RigidBody2D clone() const
                {
                    return RigidBody2D(*this);
                }

                bool Deserialize(const rapidjson::Document& doc, int id)
                {
                    std::string a = std::to_string(id);
                    if (doc.HasMember(a.c_str()))
                    {
                        if (doc[a.c_str()].HasMember("RigidBody2D"))
                        {
                            const rapidjson::Value& obj = doc[a.c_str()]["RigidBody2D"];
                            RigidBody2D r;

                            _velocity = AddType(obj, "Velocity", r._velocity);
                            _acceleration = AddType(obj, "Acceleration", r._acceleration);
                            _totalRotForce = AddType(obj, "TotalRotForce", r._totalRotForce);
                            _totalForce = AddType(obj, "TotalForce", r._totalForce);
                            _isGravity = AddType(obj, "Gravity", r._isGravity);
                            _maxVel = AddType(obj, "MaxVel", r._maxVel);
                            _mass = AddType(obj, "Mass", r._mass);
                            _gravityScale = AddType(obj, "GravityScale", r._gravityScale);
                            _restitution = AddType(obj, "Restitution", r._restitution);
                            _dragScale = AddType(obj, "DragScale", r._dragScale);
                            _leftForce = AddType(obj, "LeftForce", r._leftForce);
                            _rightForce = AddType(obj, "RightForce", r._rightForce);
                            _jumpForce = AddType(obj, "JumpForce", r._jumpForce);
                            _movementSpeed = AddType(obj, "MovementSpeed", r._movementSpeed);

                            _forceList.push_back({ _jumpForce, 0.1f });     // 0
                            _forceList[0]._type = forceType::JUMP;
                            _forceList.push_back({ _leftForce, 1.0f / 60.0f });     // 1
                            _forceList[1]._type = forceType::LEFT;
                            _forceList.push_back({ _rightForce, 1.0f / 60.0f });    // 2
                            _forceList[2]._type = forceType::RIGHT;

                            return true;
                        }
                        //else
                        //{
                        //    std::cout << "EntityID " << a <<
                        //        " : No RigidBody2D." << std::endl;
                        //    return false;
                        //}

                    }
                    else
                    {
                        std::cout << "EntityID " << a <<
                            " : cannot find the entity and its RigidBody2D." << std::endl;
                        return false;
                    }

                    return false;
                }
                bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
                {
                    writer->Key("RigidBody2D");

                    writer->StartObject();
                    writer->SetMaxDecimalPlaces(3);
                    writer->String("Velocity");
                    SerializeVector(writer, _velocity);

                    writer->String("Acceleration");
                    SerializeVector(writer, _acceleration);

                    writer->String("Gravity");
                    writer->Bool(_isGravity);

                    writer->String("MaxVel");
                    writer->Double(_maxVel);

                    writer->String("Mass");
                    writer->Double(_mass);

                    writer->String("TotalRotForce");
                    SerializeVector(writer, _totalRotForce);

                    writer->String("TotalForce");
                    SerializeVector(writer, _totalForce);

                    writer->String("GravityScale");
                    writer->Double(_gravityScale);
                    writer->String("Restitution");
                    writer->Double(_restitution);
                    writer->String("DragScale");
                    writer->Double(_dragScale);

                    writer->String("LeftForce");
                    SerializeVector(writer, _leftForce);
                    writer->String("RightForce");
                    SerializeVector(writer, _rightForce);
                    writer->String("JumpForce");
                    SerializeVector(writer, _jumpForce);
                  

                    writer->String("MovementSpeed");
                    writer->Double(_movementSpeed);

                    writer->EndObject();

                    return true;
                }

            };
        }
    }
}