/*******************************************************************************
filename    Vacuum.h
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        November 19, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once
#include "Scripts.h"

namespace Engine
{
    namespace AI
    {
        enum class VacuumState
        {
            OFF,
            SUCK,
            ISHOLDING,
            SHOOT,
            BLOW
        };

        class Vacuum : public Scripts
        {
            virtual Vacuum* Clone() const { return new Vacuum(*this); }

            // Data members
            EntityID _parentID;                 // Parent ID 
            Vec2D _suckForce;                   // Vacuum suction force
            Vec2D _blowForce;                   // Vacuum blowing force
            float _dt;                          // Current dt of game
            bool _isHolding;                    // Check if vacuum is holding
            EntityID _holdObjID;                // The id of the obj
            VacuumState _currentState;          // The state of the vacuum
            float _initialRectHeight;
            float _initialRectWidth;
            Vec2D _mousePos;
            bool _isSucking;
            EntityID _suckObjID;
            unsigned _spriteIndex = 0;

        public:
            Vacuum();
            ~Vacuum();

            void Init();
            void Update(float dt);
            void UpdateInput();

            bool IsInRange(EntityID target);
            bool IsInVacuum(EntityID vacBox, EntityID objectBox);

            void Suck(EntityID targetID);
            void Hold(EntityID targetID);
            void Shoot(EntityID targetID);
            void Blow(EntityID targetID);

            void SetSuckForce(Vec2D suckForce) { _suckForce = suckForce; }
            void SetBlowForce(Vec2D blowForce) { _blowForce = blowForce; }

            VacuumState GetVacuumState() { return _currentState; }
            Vec2D GetSuckForce() { return _suckForce; }
            Vec2D GetBlowForce() { return _blowForce; }

            void ChangeBlowFactor(float b) { _blowFactor = b; }

            float _blowFactor;
            float _arcForce;
            float _shootForce;
            void HideVacuum();
            void ShowVacuum();
            int GetHoldObjID();

            void ReleaseHoldObj();

            EntityID GetCloserObj();

            bool TrigSuck = false;
            bool TrigBlow = false;
        };
    }
}
