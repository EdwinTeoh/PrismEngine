/*******************************************************************************
filename    ExplodingBarrel.h
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        Jan 29, 2021
brief

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once
#include "LogicBase.h"
#include "../Player_State.h"
namespace Engine
{
    class ExplodingBarrel : public ScriptType
    {
        static ExplodingBarrel* _instance;
        bool _isHit;
        bool _isActive;
        bool _exploded;
        std::string _explodingBarrelTag;
        float _timer;
        float _currTimer;
        float _hardcodeTimer;
        float _maxRadius;
    public:
        ExplodingBarrel();
        ~ExplodingBarrel() {};

        virtual ScriptType* Clone() const;

        static ExplodingBarrel* GetInstance();
        std::string GetTag();

        void SetScriptName();
        std::string GetScriptName();

        void SetTag(std::string s);

        bool CheckInRange(EntityID id1, float dt);
        void Explode(float dt);
        void ApplyDamage(EntityID id1);
        bool CheckInExplosionRange(EntityID id1);
        void KnockBack(EntityID id1);

        void Init();
        void Update(float dt);
        void End();
    };
 
}