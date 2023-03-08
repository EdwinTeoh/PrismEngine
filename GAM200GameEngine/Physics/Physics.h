/*!
\file Physics.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the declaration for the Physics system

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
#pragma once
#include "../System/Macro.h"
#define p_PhysicsSystem Engine::PhysicsSystem::GetInstance()

namespace Engine
{
    class PhysicsSystem
    {
        static PhysicsSystem* s_instance;

        //const variable for gravity
        const float GRAVITY = -9.8f;

        void Integrate(const float dt, EntityID id);
        void UpdatePOS(const float dt, EntityID id);
        void UpdateForce(const float dt);

    public:
        static PhysicsSystem* GetInstance();

        void Init();
        void Update(int currentNumberOfSteps);
        void Exit();
    };
}