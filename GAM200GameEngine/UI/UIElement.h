/*******************************************************************************
filename    UIElement.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        February, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "../framework.h"
#include "../Serializer/RapidJsonBase.h"
#include <random>

#define toStr(x) std::to_string(x)

namespace Engine
{
    using namespace Math;

    class UIElement
    {
    protected:
        std::string _uid;
        std::string _type;
        std::string _name;
        //Vec2D _pivot; // 0.0f to 1.0f on both axis
        Vec2D _rectSize;
        Vec2D _rectPos;
        float _rotation;

        static const float GetViewportWidth();
        static const float GetViewportHeight();
        static const std::string GenerateUID();

        const std::string GenerateType();
    public:
        float _color[4];
        bool _interactable;
        bool _enabled;

        UIElement() :
            _uid{ GenerateUID() },
            _color{ 1.0f, 1.0f, 1.0f, 1.0f },
            _interactable{ true },
            _enabled{ true }
        {
            // None
        }
        virtual ~UIElement() {}

        virtual void Create() {}
        virtual void ToEditor() = 0;

        virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) = 0;
        virtual void Deserialize(const rapidjson::Value& obj) = 0;

        const std::string GetUID() const { return _uid; };
        const std::string GetType() const { return _type; }
        const std::string GetName() const { return _name; }
        const Vec2D GetRectSizeV() const { return _rectSize; }
        const glm::vec3 GetRectSizeGLM() const {
            return glm::vec3{
                _rectSize._x / GetViewportWidth() * 2.0f,
                _rectSize._y / GetViewportHeight() * 2.0f,
                0.0f };
        }
        const glm::vec3 GetRectPosGLM() const {
            return glm::vec3{
                _rectPos._x / GetViewportWidth() * 2.0f,
                _rectPos._y / GetViewportHeight() * 2.0f,
                0.0f };
        }
        const Vec2D GetRectPosV() const { return _rectPos; }
        const std::string GetRectPosS() const
        {
            return "(" + toStr(_rectPos._x) + ", " + toStr(_rectPos._y) + ")";
        }
        const float GetRotation() { return _rotation; }
        virtual const float* GetColor() { return _color; }
        virtual const glm::vec4 GetColorGLM() { return { _color[0],_color[1],_color[2],_color[3] }; }

        void SetUID(const std::string uid) { _uid = uid; }
        void SetName(const std::string name) { _name = name; }
        void SetRectSize(const Vec2D size) { _rectSize = size; }
        void SetRectPos(const Vec2D pos) { _rectPos = pos; }
        void SetRotation(const float rotation) { _rotation = rotation; }
        void SetColor(const float r = 1.0f, const float g = 1.0f, const float b = 1.0f, const float a = 1.0f)
        {
            _color[0] = r;
            _color[1] = g;
            _color[2] = b;
            _color[3] = a;
        }
    };
}