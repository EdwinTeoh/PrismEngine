/*******************************************************************************
filename   GameLogic.cpp
author(s)   Zhang Xinyu,xinyu.z@digipen.edu
date        November 25, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "../../System/Macro.h"
#include "LogicBase.h"
#include "Player.h"

using namespace Engine::ECS;


namespace Engine
{

	class Trap :public ScriptType
	{
		static Trap* _instance;
		bool _buttonPressed;
		float _originalCoord;
		float _lowerYCoord;
	public:
		bool _movingDown;
		bool _movingUp;
		Trap();
		~Trap() {}

		virtual void SetScriptName();
		virtual std::string GetScriptName();
		void SetButton(const bool b);
		bool ButtonPressed();

		static Trap* GetInstance();
		virtual ScriptType* Clone() const;

		void Init();
		void Update(float dt);
		void End();
	};
}