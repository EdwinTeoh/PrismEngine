/*******************************************************************************
filename    Obstacle.h
author(s)   Chan Mun Leng, Nicolette, c.munlengnicolette@digipen.edu
date        December 09, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once
#include "LogicBase.h"

namespace Engine
{
    // Opens only when a vacuumed up object
    // hits with the obstacle

    class Obstacle : public ScriptType
    {
        static Obstacle* _instance;
        bool _isOpened;
        std::string _obstacleTag;
        Vec2D _pos;
        EntityID _vacuumID;
        EntityID _objThatHitsID;
        float _timer;
        EntityID topDoorID = (EntityID)-1;
        EntityID bottomDoorID = (EntityID)-1;

    public:
        Obstacle();
        ~Obstacle() {};

        virtual ScriptType* Clone() const;


        void SetScriptName();
        std::string GetScriptName();

        static Obstacle* GetInstance();

        bool IsHitByObject(EntityID objID, float dt);
        void OpenDoor();

        EntityID AddTopDoor(EntityID objID);
        EntityID AddBottomDoor(EntityID objID);

        void Init();
        void Update(float dt);
        void End();
    };
}