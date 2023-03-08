/*******************************************************************************
filename    Systems.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "Macro.h"
#include "../Animation/Animation.h"
#include "../Clock/Clock.h"
#include "../ECS/ECS.h"
#include "../ECS/Factory.h"

#include "../Graphics/ParticleSystem/Particle.h"

#include "../Graphics/GraphicsSys.h"
#include "../Scene/Scene.h"
#include "../Input/Input.h"
#include "../GameLogic/GameLogic.h"
#include "../Editor/PE_Editor.h"
#include "../Sound/SoundManager.h"
#include "../Logger/PE_Logger.h"

#include <iostream>

namespace Engine
{
	class ISystem
	{
	public:
		static std::vector<ISystem*> _systems;

		ISystem() = default;
		~ISystem() = default;

		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Exit() = 0;
	};

	void SystemsInit();
	void SystemsUpdate();
	void SystemsExit();
}