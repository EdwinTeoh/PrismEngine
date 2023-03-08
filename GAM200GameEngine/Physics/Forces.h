/*!
\file Forces.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the declaration for the Force class

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include "../ECS/IComponent.h"
#include "../ECS/ComponentManager.h"
#include "../ECS/ECS.h"

namespace Engine
{
    class Force
    {
    public:
        enum class ForceType
        {
            JUMP, // 0
            LEFT, // 1
            RIGHT // 2
        };

        Vec2D _force;
        float _age;
        float _lifetime;
        bool _isActive;
        ForceType _type;

        Force(Vec2D force, float lifetime = 1.0f / 60.0f) :
            _force{ force },
            _lifetime{ lifetime },
            _age{ 0 },
            _isActive{ false },
            _type{ForceType::LEFT}
        { }

        void ActivateForce()
        {
            if (!_isActive)
                _isActive = true;
        }

        void DectivateForce()
        {
            if (_isActive)
                _isActive = false;
        }
    };

    class ForceList
    {
        friend void Integrate(const float dt, EntityID id);
        friend void UpdateForce(const float dt);
    };
}

#define forceType Engine::Force::ForceType