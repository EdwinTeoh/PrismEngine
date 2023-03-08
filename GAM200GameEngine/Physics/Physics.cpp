/*!
\file Physics.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the definition for the Physics system

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
#pragma once

#include "Physics.h"
#include "Forces.h"
#include "../Clock/Clock.h"
#include "../ECS/Transform.h"
#include "../ECS/Rigidbody.h"

#define Transform Engine::ECS::Components::Transform
#define Rigidbody Engine::ECS::Components::RigidBody2D

namespace Engine
{
    PhysicsSystem* PhysicsSystem::s_instance;

    PhysicsSystem* PhysicsSystem::GetInstance()
    {
        if (!s_instance)
        {
            s_instance = new PhysicsSystem;
        }

        return s_instance;
    }

    void PhysicsSystem::Init()
    {
        // Nothing yet
    }

    void PhysicsSystem::Update(int currentNumberOfSteps)
    {
        Utility::Clock::GetInstance().Start(Utility::SystemCode::PHYSICS);
        Utility::Clock::GetInstance().GetDT(Utility::SystemCode::PHYSICS);
        float dt = 1.0f / 60.0f;
        for (int step = 0; step < currentNumberOfSteps; step++)
        {
            if (ECS::compManager->CheckForList<Rigidbody>())
            {
                auto& list
                    = ECS::compManager->GetComponentList<Rigidbody>().GetList();

                for (auto i = list.begin(); i != list.end(); ++i)
                {
                    Integrate(dt, (*i).first);

                    if (ECS::compManager->CheckForEntity<Transform>((*i).first))
                    {
                        UpdatePOS(dt, (*i).first);
                    }

                }
                UpdateForce(dt);
            }
        }
        Utility::Clock::GetInstance().Update(Utility::SystemCode::PHYSICS);
    }

    void PhysicsSystem::UpdatePOS(const float dt, EntityID id)
    {
        Rigidbody& entityVel
            = ECS::compManager->GetEntityComponent<Rigidbody>(id);
        Transform& entityPOS
            = ECS::compManager->GetEntityComponent<Transform>(id);

        entityPOS._position = entityVel.GetVelocity() * dt + (entityPOS._position);
    }

    void PhysicsSystem::Integrate(const float dt, EntityID id)
    {
        ECS::Components::RigidBody2D& eRb
            = ECS::compManager->GetEntityComponent<Rigidbody>(id);

        for (size_t i = 0; i < eRb._forceList.size(); i++)
        {
            if (eRb._forceList[i]._isActive)
            {
                float ms = 1; // Movement speed
                 // Only need to add movement speed if its left and right
                 // movement speed does not affect jump
                if (eRb._forceList[i]._type != forceType::JUMP)
                    ms = eRb.GetMovementSpeed();

                eRb.SetTotalForce(eRb.GetTotalForce() + eRb._forceList[i]._force * ms);
            }
        }

        // Compute gravity force
        Vec2D gravityForce = { 0, eRb.GetMass() * GRAVITY * eRb.GetGravityScale() * eRb.CheckIfGravity() };
        
        eRb.SetTotalForce({ eRb.GetTotalForce()._x,
                            eRb.GetTotalForce()._y + gravityForce._y });

        // Determine the acceleration
        Vec2D acceleration = { eRb.GetTotalForce()._x / eRb.GetMass(),
                               eRb.GetTotalForce()._y / eRb.GetMass() };

        // Integrate the velocity
        eRb.SetVelocity(eRb.GetVelocity() + acceleration * dt);

        // Dampen the velocity for numerical stability and soft drag
        eRb.SetVelocity({ eRb.GetVelocity()._x * std::pow(eRb.GetDragScale(), dt), eRb.GetVelocity()._y * std::pow(0.99f, dt) });

        // Clamp to velocity max for numerical stability
        if (Math::V2Dot(eRb.GetVelocity(), eRb.GetVelocity())
        > (eRb.GetMaxVel() * eRb.GetMaxVel()))
        {
            eRb.SetVelocity(eRb.GetVelocity().MakeNormal());
            eRb.SetVelocity(eRb.GetVelocity() * eRb.GetMaxVel());
        }

        // Checking the epsilon
        if (abs(eRb.GetVelocity()._x) < 0.001f)
        {
            // Set x value to 0.0f
            float y = eRb.GetVelocity()._y;
            Vec2D v = { 0.0f, y };
            eRb.SetVelocity(v);
        }

        if (abs(eRb.GetVelocity()._y) < 0.001f)
        {
            // Set y value to 0.0f
            float x = eRb.GetVelocity()._x;
            Vec2D v = { x, 0.0f };
            eRb.SetVelocity(v);
        }

        // Clear the force
        eRb.SetTotalForce({ 0.0f, 0.0f });
    }

    void PhysicsSystem::UpdateForce(const float dt)
    {
        auto& list
            = ECS::compManager->GetComponentList<Rigidbody>().GetList();

        for (auto i = list.begin(); i != list.end(); ++i)
        {
            for (unsigned k = 0; k < (*i).second._forceList.size(); k++)
            {
                if ((*i).second._forceList[k]._age < (*i).second._forceList[k]._lifetime)
                {
                    (*i).second._forceList[k]._age += dt;
                }
                else
                {
                    if (k < 3)
                    {
                        (*i).second._forceList[k]._age = 0;
                        (*i).second._forceList[k].DectivateForce();
                    }
                    else
                    {
                        (*i).second._forceList[k].DectivateForce();
                        (*i).second.ClearForce(k);
                        k--;
                    }
                }
            }
        }
    }

    void PhysicsSystem::Exit()
    {
        delete s_instance;
    }
}