/* Start Header ************************************************************************/
/*!
\file Collision.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the declaration for the Collision system

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../System/Macro.h"
#include "../ECS/Components.h"
#include "../ECS/ComponentManager.h"
#include "../ECS/ECS.h"
#include "../ECS/Rigidbody.h"
#include "Physics.h"
#include "../Clock/Clock.h"
#include <cmath>

namespace Engine
{
    namespace Collision
    {
        class Manifold
        {
            float _penetration;
            float _velAlongNormal;
            float _massA;
            float _massB;
            float _invMassA;
            float _invMassB;
            float _restitutionA;
            float _restitutionB;
            Vec2D _normal;
            Vec2D _tan;

        public:
            Manifold();
            // getters
            float GetPenetration() const;
            float GetVelAlongNormal() const;
            float GetMassA() const;
            float GetMassB() const;
            float GetInvMassA() const;
            float GetInvMassB() const;
            float GetRestitutionA() const;
            float GetRestitutionB() const;
            Vec2D GetNormal() const;
            Vec2D GetTan() const;

            // setters
            void SetPenetration(const float p);
            void SetVelAlongNormal(const float n);
            void SetMassA(const float a);
            void SetMassB(const float b);
            void SetInvMassA(const float a);
            void SetInvMassB(const float b);
            void SetRestitutionA(const float a);
            void SetRestitutionB(const float b);
            void SetNormal(const Vec2D n);
            void SetTan(const Vec2D t);
        };

        // Suppose to be based on material
        static float STATICFRICTION = 1.0f;
        // 0.6 - 0.8
        static float DYNAMICFRICTION = 0.6f;

        // Update
        void Update(int currentNumberOfSteps);

        // Box to Box Collision
        // Use this for 2 Box collision, returns true or false
        // No Impulse calculated
        bool AABBToAABB(EntityID rect1, EntityID rect2,
            const float dt = 1.0f / 60.0f);

        // Impulse for Box to Box Collision
        bool AABBToAABBImpulse(EntityID r1, EntityID r2, bool& rb1, bool& rb2, const float dt, Manifold& manifold);

        // After collision
        void CollisionResponse(EntityID id1, EntityID id2, bool rb1, bool rb2, Manifold& manifold);
        // Circle to Box Collision
        bool AABBtoCircle(EntityID rect, EntityID circle);

        // Circle to Circle Collision
        bool CirclevsCircle(EntityID c1, EntityID c2);
        // Circle to Line Collision
        bool CircleToLine(EntityID circle, EntityID line, const float dt);

        // Circle to Center of Box Collision
        bool AABBCentertoCircle(EntityID rect, EntityID circle);

        // This is for collision, dont call this
        bool InternalAABBToAABB(EntityID rect1, EntityID rect2,
            bool rb1, bool rb2, const float dt = 1.0f / 60.0f);


        // No impulse Collision Response
        void CollisionResolution(EntityID id1, EntityID id2, Vec2D& id1POS, Vec2D& id2POS);
    
        unsigned PlayerCanUncrawl(EntityID r1, EntityID r2, Vec4D uncrawlSize);
    }

}