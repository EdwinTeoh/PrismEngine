/* Start Header ************************************************************************/
/*!
\file Collision.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the definition for the Collision system

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include <iostream>
#include <map>
#include "Collision.h"
#include "../Graphics/GraphicsSys.h"
#include "../Window/Window.h"
#include "../GameLogic/Player_State.h"
#include "../GameLogic/LogicScripts/Player.h"

namespace Engine
{
    namespace Collision
    {
        // debugging
        #define DEBUG_NONE(str) \
            do {std::cout << str << std::endl; } while ( false )

        #define DEBUG_ONE(str, x) \
            do { std::cout << str << x << std::endl; } while( false )
                 
        #define DEBUG_TWO(str, x, y) \
            do { std::cout << str << x << " " << y << std::endl; } while( false )


        // defining max()
        #ifndef max
            #define max(a,b) (((a) > (b)) ? (a) : (b))
        #endif
        // defining min()
        #ifndef min
            #define min(a,b) (((a) < (b)) ? (a) : (b))
        #endif

        void Update(int currentNumberOfSteps)
        {
            (void)(currentNumberOfSteps);

            Utility::Clock::GetInstance().Start(Utility::SystemCode::COLLISION);
            Utility::Clock::GetInstance().GetDT(Utility::SystemCode::COLLISION);
            float dt = 1.0f / 60.0f;
            // for (int step = 0; step < currentNumberOfSteps; step++)
   
            if (!ECS::compManager->CheckForList<RectColliderComponent>())
                return;
            std::map<EntityID, RectColliderComponent>& list
                = ECS::compManager->GetComponentList<RectColliderComponent>().GetList();

            EntityID playerID = 0;

            for (auto i = list.begin(); i != list.end(); ++i)
            {
                auto j = i;
                for (++j; j != list.end(); ++j)
                {
                    RectColliderComponent& rect1Box = ECS::compManager->GetEntityComponent<RectColliderComponent>((*i).first);
                    (rect1Box._collisionBitflag = RectColliderComponent::CollisionBitflag::NONE_);
                    RectColliderComponent& rect2Box = ECS::compManager->GetEntityComponent<RectColliderComponent>((*j).first);
                    (rect2Box._collisionBitflag = RectColliderComponent::CollisionBitflag::NONE_);
                }
            }

            for (auto i = list.begin(); i != list.end(); ++i)
            {
                auto j = i;
                
                // Reset bitflag
              

                i->second.SetIsCollided(false);

                if ((ECS::entityManager->FindEntity((*i).first).GetTag() != "Wall") ||
                    (ECS::entityManager->FindEntity((*i).first).GetTag() != "ClimbPath") ||
                    (ECS::entityManager->FindEntity((*i).first).GetTag() != "Vacuum") ||
                    (ECS::entityManager->FindEntity((*i).first).GetTag() != "FallCollider"))
                {
                    // Vec2D id1POS = ECS::compManager->GetEntityComponent<TransformComponent>((*i).first)._position;
                    // std::cout << " RectColliderComponent List size: " << list.size() << std::endl;
                    for (++j; j != list.end(); ++j)
                    {
                        j->second.SetIsCollided(false);

                        if ((ECS::entityManager->FindEntity((*i).first).GetTag() == "Vacuum" ||
                            ECS::entityManager->FindEntity((*j).first).GetTag() == "Vacuum") ||
                            (ECS::entityManager->FindEntity((*j).first).GetTag() == "Vacuum" && 
                                ECS::entityManager->FindEntity((*i).first).GetTag() == "Enemy") ||
                            ((ECS::entityManager->FindEntity((*i).first).GetTag() == "Vacuum" &&
                                ECS::entityManager->FindEntity((*j).first).GetTag() == "Enemy")) ||
                            (ECS::entityManager->FindEntity((*j).first).GetTag() == "FallCollider") ||
                            ((ECS::entityManager->FindEntity((*i).first).GetTag() == "Shock Platform" &&
                                (ECS::entityManager->FindEntity((*j).first).GetTag() == "Wall" || 
                                ECS::entityManager->FindEntity((*j).first).GetTag() == "Shock Platform"))))
                        {
                            continue;
                        }

                        Manifold m;

                        // checks if the 2 boxes collided

                        // Check for rigidbody
                        bool rb1 = false;
                        bool rb2 = false;

                        if (AABBToAABBImpulse((*i).first, (*j).first, rb1, rb2, dt, m))
                        {
                            i->second.SetIsCollided(true);
                            j->second.SetIsCollided(true);
                           
                            if ((*i).first == EntityID(playerID) && ECS::entityManager->FindEntity((*j).first).GetTag() == "ClimbPath")
                            {
                                i->second.SetCollidedWithWall(true);
                                j->second.SetCollidedWithWall(true);
                            }

                            else if (!i->second.CheckIfTriggered() && !j->second.CheckIfTriggered())
                            {
                                i->second.SetIsCollided(true);
                                j->second.SetIsCollided(true);
                                if (!i->second.CheckIfTriggered() && !j->second.CheckIfTriggered())
                                {
                                    if ((ECS::entityManager->FindEntity((*i).first).GetTag() == "Player") ||
                                        (ECS::entityManager->FindEntity((*j).first).GetTag() == "Player"))
                                    {
                                        if (Player::GetInstance()->_SKey)
                                        {
                                            Player::GetInstance()->_uncrawlCollisionBitflag |= PlayerCanUncrawl((*i).first, (*j).first,
                                                Player::GetInstance()->_colliderScale[0]);

                                            if (Player::GetInstance()->_uncrawlCollisionBitflag == 3)
                                            {
                                                ChangeState_Jeff(JeffState::j_Crawl);
                                                Player::GetInstance()->_canUncrawl = false;
                                            }
                                            else
                                            {
                                                Player::GetInstance()->_canUncrawl = true;
                                            }
                                        }
                                    }

                                    if ((ECS::entityManager->FindEntity((*i).first).GetTag() == "Player") &&
                                        (ECS::entityManager->FindEntity((*j).first).GetTag() == "Enemy"))
                                    {
                                        if (ECS::compManager->CheckForEntity<PatrolComponent>((*j).first))
                                        {
                                            PatrolComponent& patrol
                                                = ECS::compManager->GetEntityComponent<PatrolComponent>((*j).first);
                                            if (patrol._belongsTo->_isStunned)
                                            {
                                                continue;
                                            }
                                            RigidBodyComponent& patrolBOX
                                                = ECS::compManager->GetEntityComponent<RigidBodyComponent>((*j).first);
                                            patrolBOX.SetVelocity(0.0f);
                                            patrolBOX.SetAcceleration(0.0f);
                                        }
                                    }

                                    if (((ECS::entityManager->FindEntity((*i).first).GetTag() == "Enemy")
                                        && (ECS::entityManager->FindEntity((*j).first).GetTag() == "Box")))
                                    {
                                        PatrolComponent& patrol
                                            = ECS::compManager->GetEntityComponent<PatrolComponent>((*i).first);
                                        if (patrol._belongsTo->_isStunned)
                                        {
                                            RectColliderComponent& rect2Box = ECS::compManager->GetEntityComponent<RectColliderComponent>((*j).first);
                                            (rect2Box._collisionBitflag = RectColliderComponent::CollisionBitflag::NONE_);
                                            continue;
                                        }
                                    }

                                    if (((ECS::entityManager->FindEntity((*j).first).GetTag() == "Enemy")
                                        && (ECS::entityManager->FindEntity((*i).first).GetTag() == "Box")))
                                    {
                                        PatrolComponent& patrol
                                            = ECS::compManager->GetEntityComponent<PatrolComponent>((*j).first);
                                        if (patrol._belongsTo->_isStunned)
                                        {
                                            RectColliderComponent& rect2Box = ECS::compManager->GetEntityComponent<RectColliderComponent>((*i).first);
                                            (rect2Box._collisionBitflag = RectColliderComponent::CollisionBitflag::NONE_);
                                            continue;
                                        }
                                    }

                                    CollisionResponse((*i).first, (*j).first, rb1, rb2, m);

                                    if (((ECS::entityManager->FindEntity((*i).first).GetTag() == "Player" ||
                                        ECS::entityManager->FindEntity((*j).first).GetTag() == "Enemy")
                                        && (ECS::entityManager->FindEntity((*j).first).GetTag() == "Box")))
                                    {
                                        RectColliderComponent& box = ECS::compManager->GetEntityComponent<RectColliderComponent>((*i).first);
                                        if (!(box._collisionBitflag - 1 >= 2))
                                        {
                                            RigidBodyComponent& boxRB
                                                = ECS::compManager->GetEntityComponent<RigidBodyComponent>((*j).first);
                                            boxRB.SetVelocity(0.0f);
                                            boxRB.SetAcceleration(0.0f);
                                        }
                                    }

                                    else if (((ECS::entityManager->FindEntity((*j).first).GetTag() == "Player" ||
                                        ECS::entityManager->FindEntity((*j).first).GetTag() == "Enemy")
                                        && (ECS::entityManager->FindEntity((*i).first).GetTag() == "Box")))
                                    {
                                        RectColliderComponent& box = ECS::compManager->GetEntityComponent<RectColliderComponent>((*i).first);
                                        if (!(box._collisionBitflag - 1 >= 2))
                                        {
                                            RigidBodyComponent& boxRB
                                                = ECS::compManager->GetEntityComponent<RigidBodyComponent>((*i).first);
                                            boxRB.SetVelocity(0.0f);
                                            boxRB.SetAcceleration(0.0f);
                                        }
                                    }

                                    // CollisionResolution((*i).first, (*j).first, id1POS, id2POS);
                                    //if (Window::_showDebugDrawing)
                                    //{
                                    //    std::cout << "E" << (*i).first << " & E" << (*j).first << " COLLIDED!!!!" << std::endl;
                                    //}

                                }
                            }
                        }
                    }
                }
            }

            Utility::Clock::GetInstance().Update(Utility::SystemCode::COLLISION);
        }

        // Manifold implementations
        // default ctor for struct Manifold
        Manifold::Manifold() :
            _penetration{ 0.0f },
            _velAlongNormal{ 0.0f },
            _massA{ 0.0f },
            _massB{ 0.0f },
            _invMassA{ 0.0f },
            _invMassB{ 0.0f },
            _restitutionA{ 0.0f },
            _restitutionB{ 0.0f },
            _normal( {0.0f, 0.0f} ),
            _tan({ 0.0f, 0.0f })
        {}

        // getters
        float Manifold::GetPenetration() const { return _penetration; }
        float Manifold::GetVelAlongNormal() const { return _velAlongNormal; }
        float Manifold::GetMassA() const { return _massA; }
        float Manifold::GetMassB() const { return _massB; }
        float Manifold::GetInvMassA() const { return _invMassA; }
        float Manifold::GetInvMassB() const { return _invMassB; }
        float Manifold::GetRestitutionA() const { return _restitutionA; }
        float Manifold::GetRestitutionB() const { return _restitutionB; }
        Vec2D Manifold::GetNormal() const { return _normal; }
        Vec2D Manifold::GetTan() const { return _normal; }

        // setters
        void Manifold::SetPenetration(const float p) { _penetration = p; }
        void Manifold::SetVelAlongNormal(const float n) { _velAlongNormal = n; }
        void Manifold::SetMassA(const float a) { _massA = a; }
        void Manifold::SetMassB(const float b) { _massB = b; }
        void Manifold::SetInvMassA(const float a) { _invMassA = a; }
        void Manifold::SetInvMassB(const float b) { _invMassB = b; }
        void Manifold::SetRestitutionA(const float a) { _restitutionA = a; }
        void Manifold::SetRestitutionB(const float b) { _restitutionB = b; }
        void Manifold::SetNormal(const Vec2D n) { _normal = n; }
        void Manifold::SetTan(const Vec2D t) { _tan = t; }

        bool AABBToAABB(EntityID r1, EntityID r2, const float dt)
        {
            bool rb1 = false;
            bool rb2 = false;
            
            if (ECS::compManager->CheckForList<RigidBodyComponent>())
            {
                std::map<EntityID, RigidBodyComponent>& list
                    = ECS::compManager->GetComponentList<RigidBodyComponent>().GetList();

                for (auto i = list.begin(); i != list.end(); ++i)
                {
                    if ((*i).first == r1)
                        rb1 = true;
                    if ((*i).first == r2)
                        rb2 = true;
                }
            }

            Pt aabb1Max, aabb1Min, aabb2Max, aabb2Min;

            float tFirst = 0.0f, tLast = dt, dFirst = 0.0f, dLast = 0.0f;

            //Getting the AABB box
            ECS::Components::RectCollider& rect1Box
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(r1);
            ECS::Components::RectCollider& rect2Box
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(r2);
  
            //Getting the entity's POS
            ECS::Components::Transform& rect1POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(r1);
            ECS::Components::Transform& rect2POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(r2);


            Vec2D relvel = { 0.0f, 0.0f };
            Vec2D POS1 = rect1POS._position;
            Vec2D POS2 = rect2POS._position;

            // Adding offset
            POS1 = POS1 + rect1Box.GetOffSet();
            POS2 = POS2 + rect2Box.GetOffSet();

            // Vector from A to B
            Vec2D AB = POS2 - POS1;

            // Calculate half extents along x axis for each object
            float a_extentx = rect1Box.GetWidth() / 2.0f;
            float a_extenty = rect1Box.GetHeight() / 2.0f;
            float b_extentx = rect2Box.GetWidth() / 2.0f;
            float b_extenty = rect2Box.GetHeight() / 2.0f;

            // Static Collision checks
            aabb1Max._x = POS1._x + a_extentx;
            aabb1Max._y = POS1._y + a_extenty;
            aabb1Min._x = POS1._x - a_extentx;
            aabb1Min._y = POS1._y - a_extenty;

            aabb2Max._x = POS2._x + b_extentx;
            aabb2Max._y = POS2._y + b_extenty;
            aabb2Min._x = POS2._x - b_extentx;
            aabb2Min._y = POS2._y - b_extenty;

            // x-axis not overlapping
            // y-axis not overlapping
            if ((aabb1Max._x < aabb2Min._x) || (aabb1Min._x > aabb2Max._x))
                return false;
            if((aabb1Max._y < aabb2Min._y) || (aabb1Min._y > aabb2Max._y))
                return false;

            // Check if entities have velocity, set vrel accordingly
            if (rb1 && rb2)
            {
                ECS::Components::RigidBody2D& rect1Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(r1);
                ECS::Components::RigidBody2D& rect2Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(r2);
                relvel = rect2Vel.GetVelocity() - rect1Vel.GetVelocity();
            }
            else if (rb1)
            {
                relvel = -ECS::compManager->GetEntityComponent<RigidBodyComponent>(r1).GetVelocity();
            }
            else if (rb2)
            {
                relvel = ECS::compManager->GetEntityComponent<RigidBodyComponent>(r2).GetVelocity();
            }
            else
            {
                return false;
            }

            // Dynamic collision check
            if (relvel._x < 0)
            {
                if (aabb1Min._x > aabb2Max._x)
                    return false;

                if (aabb1Max._x < aabb2Min._x)
                {
                    dFirst = aabb1Max._x - aabb2Min._x;
                    tFirst = max((dFirst / relvel._x), tFirst);
                }

                if (aabb1Min._x < aabb2Max._x)
                {
                    dLast = aabb1Min._x - aabb2Max._x;
                    tLast = min((dLast / relvel._x), tLast);
                }
            }

            if (relvel._x > 0)
            {
                if (aabb1Max._x < aabb2Min._x)
                    return false;

                if (aabb1Min._x > aabb2Max._x)
                {
                    dFirst = aabb1Min._x - aabb2Max._x;
                    tFirst = max((dFirst / relvel._x), tFirst);
                }

                if (aabb1Max._x > aabb2Min._x)
                {
                    dLast = aabb1Max._x - aabb2Min._x;
                    tLast = min((dLast / relvel._x), tLast);
                }
            }

            if (tFirst > tLast)
                return false;

            if (relvel._y < 0)
            {
                if (aabb1Min._y > aabb2Max._y)
                    return false;

                if (aabb1Max._y < aabb2Min._y)
                {
                    dFirst = aabb1Max._y - aabb2Min._y;
                    tFirst = max((dFirst / relvel._y), tFirst);
                }

                if (aabb1Min._y < aabb2Max._y)
                {
                    dLast = aabb1Min._y - aabb2Max._y;
                    tLast = min((dLast / relvel._y), tLast);
                }
            }

            if (relvel._y > 0)
            {
                if (aabb1Max._y < aabb2Min._y)
                    return false;

                if (aabb1Min._y > aabb2Max._y)
                {
                    dFirst = aabb1Min._y - aabb2Max._y;
                    tFirst = max((dFirst / relvel._y), tFirst);
                }

                if (aabb1Max._y > aabb2Min._y)
                {
                    dLast = aabb1Max._y - aabb2Min._y;
                    tLast = min((dLast / relvel._y), tLast);
                }
            }

            if (tFirst > tLast)
                return false;

            return true;
        }

        bool AABBToAABBImpulse(EntityID r1, EntityID r2, bool& rb1, bool& rb2, const float dt, Manifold& manifold)
        {
            if (ECS::compManager->CheckForList<RigidBodyComponent>())
            {
                // Hacky way to stablize the FPS. If theres more 
                // RigidBodyComponent than components, then need to change

                std::map<EntityID, RigidBodyComponent>& list
                    = ECS::compManager->GetComponentList<RigidBodyComponent>().GetList();

                for (auto i = list.begin(); i != list.end(); ++i)
                {
                    if ((*i).first == r1)
                        rb1 = true;
                    if ((*i).first == r2)
                        rb2 = true;
                }
            }

            Pt aabb1Max, aabb1Min, aabb2Max, aabb2Min;

            //Getting the AABB box
            RectColliderComponent& rect1Box
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(r1);
            RectColliderComponent& rect2Box
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(r2);

            //Getting the entity's POS
            TransformComponent& rect1POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(r1);
            TransformComponent& rect2POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(r2);

            if (InternalAABBToAABB(r1, r2, rb1, rb2, dt) == false)
            {
                // (rect1Box._collisionBitflag = RectColliderComponent::CollisionBitflag::NONE_);
                return false;
            }

            Vec2D relvel = { 0.0f, 0.0f };
            Vec2D POS1 = rect1POS._position;
            Vec2D POS2 = rect2POS._position;
            
            // Adding offset
            POS1 = POS1 + rect1Box.GetOffSet();
            POS2 = POS2 + rect2Box.GetOffSet();

            // Vector from A to B
            Vec2D AB = POS2 - POS1;

            // Calculate half extents along x axis for each object
            float a_extentx = rect1Box.GetWidth() / 2.0f;
            float a_extenty = rect1Box.GetHeight() / 2.0f;
            float b_extentx = rect2Box.GetWidth() / 2.0f;
            float b_extenty = rect2Box.GetHeight() / 2.0f;

            // Static Collision checks
            aabb1Max._x = POS1._x + a_extentx;
            aabb1Max._y = POS1._y + a_extenty;
            aabb1Min._x = POS1._x - a_extentx;
            aabb1Min._y = POS1._y - a_extenty;

            aabb2Max._x = POS2._x + b_extentx;
            aabb2Max._y = POS2._y + b_extenty;
            aabb2Min._x = POS2._x - b_extentx;
            aabb2Min._y = POS2._y - b_extenty;

            float x_overlap = 0.0f;
            float y_overlap = 0.0f;

            // Calculate overlap on x axis
            x_overlap = abs(AB._x) / (a_extentx + b_extentx);

            // Calculate overlap on y axis
            y_overlap = abs(AB._y) / (a_extenty + b_extenty);

            // Find out which axis is axis of least penetration
            if (x_overlap > y_overlap)
            {
                // Point towards B knowing that n points from A to B
                if (AB._x < 0)
                {
                    manifold.SetNormal({ -1.0f, 0.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::LEFT_);
                    (rect2Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::RIGHT);
                }
                else
                {
                    manifold.SetNormal({ 1.0f, 0.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::RIGHT);
                    (rect2Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::LEFT_);
                }
                x_overlap = 1.0f - abs(AB._x) / (a_extentx + b_extentx);
                manifold.SetPenetration(x_overlap);
            }
            else
            {
                // Point toward B knowing that n points from A to B
                if (AB._y < 0)
                {
                    manifold.SetNormal({ 0.0f, -1.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::DOWN_);
                    (rect2Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::UP___);
                }
                else
                {
                    manifold.SetNormal({ 0.0f, 1.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::UP___);
                    (rect2Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::DOWN_);
                }
                y_overlap = 1.0f - abs(AB._y) / (a_extenty + b_extenty);
                manifold.SetPenetration(y_overlap);
            }
            
            // if (manifold.GetNormal() == 0.0f)
            // {
            //     (rect1Box._collisionBitflag = RectColliderComponent::CollisionBitflag::NONE_);
            // }

            // Check if entities have velocity, set vrel accordingly
            if (rb1 && rb2)
            {
                ECS::Components::RigidBody2D& rect1Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(r1);
                ECS::Components::RigidBody2D& rect2Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(r2);
                relvel = rect2Vel.GetVelocity() - rect1Vel.GetVelocity();

                manifold.SetMassA(ECS::compManager->
                    GetEntityComponent<RigidBodyComponent>(r1).GetMass());
                manifold.SetMassB(ECS::compManager->
                    GetEntityComponent<RigidBodyComponent>(r2).GetMass());
                if (manifold.GetMassA() == 0.0f)
                    manifold.SetInvMassA(0.0f);
                else
                    manifold.SetInvMassA(1 / manifold.GetMassA());

                if (manifold.GetMassB() == 0.0f)
                    manifold.SetInvMassB(0.0f);
                else
                    manifold.SetInvMassB(1 / manifold.GetMassB());

                manifold.SetRestitutionA(ECS::compManager->
                    GetEntityComponent<RigidBodyComponent>(r1).GetRestitution());
                manifold.SetRestitutionB(ECS::compManager->
                    GetEntityComponent<RigidBodyComponent>(r2).GetRestitution());
            }
            else if (rb1)
            {
                relvel = -ECS::compManager->GetEntityComponent<RigidBodyComponent>(r1).GetVelocity();
                // Set Object A
                manifold.SetMassA(ECS::compManager->
                    GetEntityComponent<RigidBodyComponent>(r1).GetMass());
                manifold.SetRestitutionA(ECS::compManager->
                    GetEntityComponent<RigidBodyComponent>(r1).GetRestitution());
                if(manifold.GetMassA() == 0.0f)
                    manifold.SetInvMassA(0.0f);
                else 
                    manifold.SetInvMassA(1 / manifold.GetMassA());
                // Set Object B
                manifold.SetMassB(0.0f);
                manifold.SetInvMassB(0.0f);
                manifold.SetRestitutionB(0.0f);
            }
            else if (rb2)
            {
                relvel = ECS::compManager->GetEntityComponent<RigidBodyComponent>(r2).GetVelocity();
                manifold.SetMassB(ECS::compManager->GetEntityComponent<RigidBodyComponent>(r2).GetMass());
                manifold.SetRestitutionB(ECS::compManager->GetEntityComponent<RigidBodyComponent>(r2).GetRestitution());
                if (manifold.GetMassB() == 0.0f)
                    manifold.SetInvMassB(0.0f);
                else
                    manifold.SetInvMassB(1 / manifold.GetMassB());
                manifold.SetInvMassA(0.0f);
                manifold.SetMassA(0.0f);
                manifold.SetRestitutionA(0.0f);
            }
            else
            {
                return false;
            }

            manifold.SetVelAlongNormal(V2Dot(relvel, manifold.GetNormal()));

            return true;
        }

        void CollisionResolution(EntityID id1, EntityID id2, Vec2D& id1POS, Vec2D& id2POS)
        {
            (void)id1;
            (void)id2;
            (void)id1POS;
            (void)id2POS;

            ECS::Components::Transform& entity1POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(id1);
            ECS::Components::Transform& entity2POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(id2);

            std::cout << "Prev POS1: " << id1POS << std::endl;
            std::cout << "Prev POS2: " << id2POS << std::endl;

            entity1POS._position = id1POS;
            entity2POS._position = id2POS;

            std::cout << "Current POS1: " << entity1POS._position << std::endl;
            std::cout << "Current POS2: " << entity2POS._position << std::endl;

        }

        void CollisionResponse(EntityID id1, EntityID id2, bool rb1, bool rb2, Manifold& manifold)
        {
            //if (static_cast<int>(id1) == 0)
            //    std::cout << "EntityID id1: " << static_cast<int>(id1) << " &"
            //        << "EntityID id2: " << static_cast<int>(id2) << std::endl;
            ECS::Components::Transform& entity1POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(id1);
            ECS::Components::Transform& entity2POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(id2);

            // Calculate restitution
            float e = min(manifold.GetRestitutionA(), manifold.GetRestitutionB());

            float mass_sum = manifold.GetMassA() + manifold.GetMassB();
            float ratioA = manifold.GetMassA() / mass_sum;
            float ratioB = manifold.GetMassB() / mass_sum;
            //if (static_cast<int>(id1) == 0)
            //{
            //    std::cout << "ratioA: " << ratioA << std::endl;
            //    std::cout << "ratioB: " << ratioB << std::endl;
            //    std::cout << "e: " << e << std::endl;
            //    std::cout << "GetVelAlongNormal: " << manifold.GetVelAlongNormal() << std::endl;
            //}
            // Calculate impulse scalar
            float j = -(1.0f + e) * manifold.GetVelAlongNormal();
            //if (static_cast<int>(id1) == 0)
            //{
            //    std::cout << "Before j: " << j << std::endl;
            //}

            j /= manifold.GetInvMassA() + manifold.GetInvMassB();
            
            //if (static_cast<int>(id1) == 0)
            //{
            //    std::cout << "After j: " << j << std::endl;
            //    std::cout << "GetInvMassA: " << manifold.GetInvMassA() << std::endl;
            //    std::cout << "GetInvMassB: " << manifold.GetInvMassB() << std::endl;
            //    std::cout << "GetMassA: " << manifold.GetMassA() << std::endl;
            //    std::cout << "GetMassB: " << manifold.GetMassB() << std::endl;
            //    std::cout << "GetNormal: " << manifold.GetNormal() << std::endl;
            //}

            // Apply impulse
            Vec2D impulse = j * manifold.GetNormal();

            //if (static_cast<int>(id1) == 0)
            //{
            //    std::cout << "impulse: " << impulse << std::endl;
            //}

            //Vec2D relvel;
            //float dynamicFriction;

            // Set velocity after the collision
            if (rb1 && rb2)
            {
                ECS::Components::RigidBody2D& entity1Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id1);
                ECS::Components::RigidBody2D& entity2Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id2);
                entity1Vel.SetVelocity(entity1Vel.GetVelocity() - ratioA * impulse);
                entity2Vel.SetVelocity(entity2Vel.GetVelocity() + ratioB * impulse);
                //relvel = entity1Vel.GetVelocity() - entity2Vel.GetVelocity();
            }
            else if (rb1)
            {
                ECS::Components::RigidBody2D& entity1Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id1);
                //if (static_cast<int>(id1) == 0)
                //{
                //    std::cout << "Before entity1Vel: " << entity1Vel.GetVelocity() << std::endl;
                //}
                entity1Vel.SetVelocity(entity1Vel.GetVelocity() - ratioA * impulse);
                // relvel = entity1Vel.GetVelocity() - ratioA * impulse;
                //relvel = entity1Vel.GetVelocity();
                //if (static_cast<int>(id1) == 0)
                //{
                //    std::cout << "After entity1Vel: " << entity1Vel.GetVelocity() << std::endl;
                //}
            }
            else if (rb2)
            {
                ECS::Components::RigidBody2D& entity2Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id2);
                //if (static_cast<int>(id1) == 0)
                //{
                //    std::cout << "Before entity2Vel: " << entity2Vel.GetVelocity() << std::endl;
                //}

                entity2Vel.SetVelocity(entity2Vel.GetVelocity() + ratioB * impulse);
                // relvel = entity2Vel.GetVelocity() + ratioB * impulse;
                //if (static_cast<int>(id1) == 0)
                //    std::cout << "After entity2Vel: " << entity2Vel.GetVelocity() << std::endl;
            }
            /*std::cout << "relvel: " << relvel << std::endl;
            Vec2D tangent = relvel - (Math::V2Dot(relvel, manifold.GetNormal()) * manifold.GetNormal());
            if (tangent == 0)
              tangent = { 0.0f, 0.0f };
            else
              tangent = tangent.MakeNormal();

            float jt = -Math::V2Dot(relvel, tangent);
            jt = jt / (manifold.GetInvMassA() + manifold.GetInvMassB());

            float mu = std::sqrtf(std::pow(1.0f, 2.0f) + std::pow(1.0f, 2.0f));

            Vec2D frictionImpulse;
            if (abs(jt) < j * mu)
              frictionImpulse = jt * tangent;
            else
            {
              dynamicFriction = std::sqrtf(std::pow(0.6f, 2.0f) + std::pow(0.8f, 2.0f));
              frictionImpulse = -j * tangent * dynamicFriction;
            }

            std::swap(frictionImpulse._x, frictionImpulse._y);

            std::cout << "Impulse: " << impulse << std::endl;
            std::cout << "Friction Impulse: " << frictionImpulse << std::endl;*/

            // Set velocity after the collision
           /* if (rb1 && rb2)
            {
              ECS::Components::RigidBody2D& entity1Vel
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id1);
              ECS::Components::RigidBody2D& entity2Vel
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id2);
              entity1Vel.SetVelocity(entity1Vel.GetVelocity() - ratioA * frictionImpulse);
              entity2Vel.SetVelocity(entity2Vel.GetVelocity() + ratioB * frictionImpulse);
            }
            else if (rb1)
            {
              ECS::Components::RigidBody2D& entity1Vel
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id1);
              entity1Vel.SetVelocity(entity1Vel.GetVelocity() - ratioA * frictionImpulse);
            }
            else if (rb2)
            {
              ECS::Components::RigidBody2D& entity2Vel
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(id2);
              entity2Vel.SetVelocity(entity2Vel.GetVelocity() + ratioB * frictionImpulse);
            }*/

            // Set position after the collision response
            // 20% to 80%
            const float percent = 0.8f;
            // 0.01 to 0.1
            const float slop = 0.0001f;
            Vec2D correction = max((manifold.GetPenetration() - slop), 0.0f)
                / (manifold.GetInvMassA() + manifold.GetInvMassB()) * percent * manifold.GetNormal();

            entity1POS._position -= manifold.GetInvMassA() * correction;
            entity2POS._position += manifold.GetInvMassB() * correction;
        }

        bool InternalAABBToAABB(EntityID rect1, EntityID rect2, bool rb1, bool rb2, const float dt)
        {
            if (rb1 == false && rb2 == false)
                return false;

            Pt aabb1Max, aabb1Min, aabb2Max, aabb2Min;

            float tFirst = 0.0f, tLast = dt, dFirst = 0.0f, dLast = 0.0f;

            //Getting the AABB box
            ECS::Components::RectCollider& rect1Box
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(rect1);
            ECS::Components::RectCollider& rect2Box
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(rect2);

            //Getting the entity's POS
            ECS::Components::Transform& rect1POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(rect1);
            ECS::Components::Transform& rect2POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(rect2);


            Vec2D relvel = { 0.0f, 0.0f };
            Vec2D POS1 = rect1POS._position;
            Vec2D POS2 = rect2POS._position;

            // Adding offset
            POS1 = POS1 + rect1Box.GetOffSet();
            POS2 = POS2 + rect2Box.GetOffSet();

            // Vector from A to B
            Vec2D AB = POS2 - POS1;

            // Calculate half extents along x axis for each object
            float a_extentx = rect1Box.GetWidth() / 2.0f;
            float a_extenty = rect1Box.GetHeight() / 2.0f;
            float b_extentx = rect2Box.GetWidth() / 2.0f;
            float b_extenty = rect2Box.GetHeight() / 2.0f;

            // Static Collision checks
            aabb1Max._x = POS1._x + a_extentx;
            aabb1Max._y = POS1._y + a_extenty;
            aabb1Min._x = POS1._x - a_extentx;
            aabb1Min._y = POS1._y - a_extenty;

            aabb2Max._x = POS2._x + b_extentx;
            aabb2Max._y = POS2._y + b_extenty;
            aabb2Min._x = POS2._x - b_extentx;
            aabb2Min._y = POS2._y - b_extenty;

            // x-axis not overlapping
            // y-axis not overlapping
            if ((aabb1Max._x < aabb2Min._x) || (aabb1Min._x > aabb2Max._x))
                return false;
            if ((aabb1Max._y < aabb2Min._y) || (aabb1Min._y > aabb2Max._y))
                return false;

            // Check if entities have velocity, set vrel accordingly
            if (rb1 && rb2)
            {
                ECS::Components::RigidBody2D& rect1Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(rect1);
                ECS::Components::RigidBody2D& rect2Vel
                    = ECS::compManager->GetEntityComponent<RigidBodyComponent>(rect2);
                relvel = rect2Vel.GetVelocity() - rect1Vel.GetVelocity();
            }
            else if (rb1)
            {
                relvel = -ECS::compManager->GetEntityComponent<RigidBodyComponent>(rect1).GetVelocity();
            }
            else if (rb2)
            {
                relvel = ECS::compManager->GetEntityComponent<RigidBodyComponent>(rect2).GetVelocity();
            }
            else
            {
                return false;
            }

            // Dynamic collision check
            if (relvel._x < 0)
            {
                if (aabb1Min._x > aabb2Max._x)
                    return false;

                if (aabb1Max._x < aabb2Min._x)
                {
                    dFirst = aabb1Max._x - aabb2Min._x;
                    tFirst = max((dFirst / relvel._x), tFirst);
                }

                if (aabb1Min._x < aabb2Max._x)
                {
                    dLast = aabb1Min._x - aabb2Max._x;
                    tLast = min((dLast / relvel._x), tLast);
                }
            }

            if (relvel._x > 0)
            {
                if (aabb1Max._x < aabb2Min._x)
                    return false;

                if (aabb1Min._x > aabb2Max._x)
                {
                    dFirst = aabb1Min._x - aabb2Max._x;
                    tFirst = max((dFirst / relvel._x), tFirst);
                }

                if (aabb1Max._x > aabb2Min._x)
                {
                    dLast = aabb1Max._x - aabb2Min._x;
                    tLast = min((dLast / relvel._x), tLast);
                }
            }

            if (tFirst > tLast)
                return false;

            if (relvel._y < 0)
            {
                if (aabb1Min._y > aabb2Max._y)
                    return false;

                if (aabb1Max._y < aabb2Min._y)
                {
                    dFirst = aabb1Max._y - aabb2Min._y;
                    tFirst = max((dFirst / relvel._y), tFirst);
                }

                if (aabb1Min._y < aabb2Max._y)
                {
                    dLast = aabb1Min._y - aabb2Max._y;
                    tLast = min((dLast / relvel._y), tLast);
                }
            }

            if (relvel._y > 0)
            {
                if (aabb1Max._y < aabb2Min._y)
                    return false;

                if (aabb1Min._y > aabb2Max._y)
                {
                    dFirst = aabb1Min._y - aabb2Max._y;
                    tFirst = max((dFirst / relvel._y), tFirst);
                }

                if (aabb1Max._y > aabb2Min._y)
                {
                    dLast = aabb1Max._y - aabb2Min._y;
                    tLast = min((dLast / relvel._y), tLast);
                }
            }

            if (tFirst > tLast)
                return false;

            return true;
        }

        bool CirclevsCircle(EntityID c1, EntityID c2)
        {
            //Getting the circle radius
            ECS::Components::CircleCollider& c1Radius 
                = ECS::compManager->GetEntityComponent<CircleColliderComponent>(c1);
            ECS::Components::CircleCollider& c2Radius 
                = ECS::compManager->GetEntityComponent<CircleColliderComponent>(c2);

            //Getting the entity's POS
            ECS::Components::Transform& c1POS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(c1);
            ECS::Components::Transform& c2POS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(c2);

            float totalRadius = c1Radius.GetRadius() + c2Radius.GetRadius();
            float distSq = V2SqDistance(c1POS._position, c2POS._position);

            // Distance Square more than total radius Squared,
            // must not be colliding
            if (distSq > totalRadius * totalRadius)
            {
                // time based collision
                return false;
            }
            return true;
        }

        bool CircleToLine(EntityID c, EntityID l, const float dt)
        {
            // Getting the circle velocity
            ECS::Components::RigidBody2D& cVel
                = ECS::compManager->GetEntityComponent<RigidBodyComponent>(c);
            // Getting the circle radius
            ECS::Components::CircleCollider& cRad 
                = ECS::compManager->GetEntityComponent<CircleColliderComponent>(c);
            ECS::Components::LineCollider& lLength 
                = ECS::compManager->GetEntityComponent<LineColliderComponent>(l);
            // Getting the entity's POS
            ECS::Components::Transform& cPOS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(c);
            ECS::Components::Transform& lPOS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(l);
            //         if (lineLength._normal.Dot(circlePOS._position) - lineLength._normal.Dot(linePOS._position) <= circleRadius._radius)
            if (V2Dot(lLength.GetNormal(), cPOS._position)
                - V2Dot(lLength.GetNormal(), lPOS._position)
                <= cRad.GetRadius())
            {
                Vec2D p0prime, p1prime, velVec, nvelVec;
                // Get P0' and P1'
                p0prime = lPOS._position - cRad.GetRadius() * lLength.GetNormal();
                p1prime = lLength.GetEndPt() - cRad.GetRadius() * lLength.GetNormal();

                // Get velocity vector
                velVec = (cPOS._position + cVel.GetVelocity() * dt) - cPOS._position;
                // Get the normal of the velocity vector
                // normalvelVec = velVec.GetNormal();

                // Check if velocity vector is within the end points of line
                if (V2Dot(nvelVec, p0prime - cPOS._position) *
                    V2Dot(nvelVec, p1prime - cPOS._position) < 0)
                {
                    float interTime;
                    // Calculate Ti
                    interTime = (V2Dot(lLength.GetNormal(),
                        lPOS._position) - V2Dot(lLength.GetNormal(),
                            cPOS._position) - cRad.GetRadius()) /
                                V2Dot(lLength.GetNormal(), velVec);

                    // Collision detected
                    if (0 <= interTime && interTime <= 1)
                    {
                        return true;
                    }
                }
            }

            // imaginary line LNS1, distant by +R (same N dir)
            else if (V2Dot(lLength.GetNormal(),
                cPOS._position) - V2Dot(lLength.GetNormal(),
                    (lPOS._position))
                >= cRad.GetRadius())
            {
                Vec2D p0prime, p1prime, velVec, normalvelVec;

                // Get P0' and P1'
                p0prime = lPOS._position + cRad.GetRadius() * lLength.GetNormal();
                p1prime = lLength.GetEndPt() + cRad.GetRadius() * lLength.GetNormal();

                // Get velocity vector
                velVec = (cPOS._position + cVel.GetVelocity() * dt) - cPOS._position;

                // Check if velocity vector is within the end points of line
                if (V2Dot(normalvelVec,
                    p0prime - cPOS._position) * V2Dot(normalvelVec, p1prime - cPOS._position)
                    < 0)
                {
                    float interTime;
                    // Calculate Ti
                    interTime = (V2Dot(lLength.GetNormal(),
                        lPOS._position) - V2Dot(lLength.GetNormal(),
                            cPOS._position) + cRad.GetRadius()) 
                                / V2Dot(lLength.GetNormal(), velVec);

                    // Collision detected
                    if (0 <= interTime && interTime <= 1)
                    {
                        return true;
                    }
                }
            }
            return false; // no intersection
        }

        bool AABBtoCircle(EntityID r, EntityID c)
        {
            // Getting the rect box and circle radius
            ECS::Components::RectCollider& rBox 
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(r);
            ECS::Components::CircleCollider& cRad 
                = ECS::compManager->GetEntityComponent<CircleColliderComponent>(c);

            // Getting the entity's POS
            ECS::Components::Transform& rPOS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(r);
            ECS::Components::Transform& cPOS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(c);


            Pt aabbMax, aabbMin;

            aabbMax._x = rPOS._position._x + (rBox.GetWidth() / 2);
            aabbMax._y = rPOS._position._y + (rBox.GetHeight() / 2);
            aabbMin._x = rPOS._position._x - (rBox.GetWidth() / 2);
            aabbMin._y = rPOS._position._y - (rBox.GetHeight() / 2);

            if (cPOS._position._x + cRad.GetRadius() < aabbMin._x)
                return false;
            else if (cPOS._position._x - cRad.GetRadius() > aabbMax._x)
                return false;
            else if (cPOS._position._y + cRad.GetRadius() < aabbMin._y)
                return false;
            else if (cPOS._position._y - cRad.GetRadius() > aabbMax._y)
                return false;

            // Colliding
            return true;
        }

        bool AABBCentertoCircle(EntityID r, EntityID c)
        {
            // Getting the circle radius
            ECS::Components::CircleCollider& circleRadius 
                = ECS::compManager->GetEntityComponent<CircleColliderComponent>(c);

            // Getting the entity's POS
            ECS::Components::Transform& rectPOS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(r);
            ECS::Components::Transform& circlePOS 
                = ECS::compManager->GetEntityComponent<TransformComponent>(c);

            if (circlePOS._position._x + circleRadius.GetRadius()< rectPOS._position._x)
                return false;
            else if (circlePOS._position._x - circleRadius.GetRadius() > rectPOS._position._x)
                return false;
            else if (circlePOS._position._y + circleRadius.GetRadius() < rectPOS._position._y)
                return false;
            else if (circlePOS._position._y - circleRadius.GetRadius() > rectPOS._position._y)
                return false;

            // Collided
            return true;
        }

        unsigned PlayerCanUncrawl(EntityID r1, EntityID r2, Vec4D uncrawlSize)
        {
            Manifold m;

            Pt aabb1Max, aabb1Min, aabb2Max, aabb2Min;

            //Getting the AABB box
            RectColliderComponent& rect2Box
                = ECS::compManager->GetEntityComponent<RectColliderComponent>(r2);

            //Getting the entity's POS
            TransformComponent& rect1POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(r1);
            TransformComponent& rect2POS
                = ECS::compManager->GetEntityComponent<TransformComponent>(r2);

            RectColliderComponent rect1Box;
            rect1Box._width = uncrawlSize._x;
            rect1Box._height = uncrawlSize._y;
            rect1Box._offset._x = uncrawlSize._z;
            rect1Box._offset._y = uncrawlSize._w;

            Vec2D POS1 = rect1POS._position;
            Vec2D POS2 = rect2POS._position;

            // Adding offset
            POS1 = POS1 + rect1Box.GetOffSet();
            POS2 = POS2 + rect2Box.GetOffSet();

            // Vector from A to B
            Vec2D AB = POS2 - POS1;

            // Calculate half extents along x axis for each object
            float a_extentx = rect1Box.GetWidth() / 2.0f;
            float a_extenty = rect1Box.GetHeight() / 2.0f;
            float b_extentx = rect2Box.GetWidth() / 2.0f;
            float b_extenty = rect2Box.GetHeight() / 2.0f;

            // Static Collision checks
            aabb1Max._x = POS1._x + a_extentx;
            aabb1Max._y = POS1._y + a_extenty;
            aabb1Min._x = POS1._x - a_extentx;
            aabb1Min._y = POS1._y - a_extenty;

            aabb2Max._x = POS2._x + b_extentx;
            aabb2Max._y = POS2._y + b_extenty;
            aabb2Min._x = POS2._x - b_extentx;
            aabb2Min._y = POS2._y - b_extenty;

            float x_overlap = 0.0f;
            float y_overlap = 0.0f;

            // Calculate overlap on x axis
            x_overlap = abs(AB._x) / (a_extentx + b_extentx);

            // Calculate overlap on y axis
            y_overlap = abs(AB._y) / (a_extenty + b_extenty);

            // Find out which axis is axis of least penetration
            if (x_overlap > y_overlap)
            {
                // Point towards B knowing that n points from A to B
                if (AB._x < 0)
                {
                    m.SetNormal({ -1.0f, 0.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::LEFT_);
                }
                else
                {
                    m.SetNormal({ 1.0f, 0.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::RIGHT);
                }
                x_overlap = 1.0f - abs(AB._x) / (a_extentx + b_extentx);
                m.SetPenetration(x_overlap);
            }
            else
            {
                // Point toward B knowing that n points from A to B
                if (AB._y < 0)
                {
                    m.SetNormal({ 0.0f, -1.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::DOWN_);
                }
                else
                {
                    m.SetNormal({ 0.0f, 1.0f });
                    (rect1Box._collisionBitflag |= RectColliderComponent::CollisionBitflag::UP___);
                }
            }
            return rect1Box._collisionBitflag;
        }
        
        // void CircleBoxResponse(EntityID rect, EntityID circle, Manifold& m)
        // {
        // 
        // }
    }

}