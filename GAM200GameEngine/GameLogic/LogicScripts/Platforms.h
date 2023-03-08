#pragma once

#include "../../System/Macro.h"
#include "LogicBase.h"
#include "Player.h"

using namespace Engine::ECS;
namespace Engine
{
	class SwitchPlatform :public ScriptType
	{
		static SwitchPlatform* _instance;
		Vec2D _originalPos;
		bool _buttonPressed;
		//float _travelDistance;
	public:
		bool _movingTo;
		//float _hardcodeTimer;
		bool _movingBack;
		float _timer;
		int _triggered; //0 is original place 1 for moving up 2 for moving down 
		SwitchPlatform();
		~SwitchPlatform() {}

		void SetButton(const bool b);
		bool ButtonPressed();

		virtual void SetScriptName();
		virtual std::string GetScriptName();

		static SwitchPlatform* GetInstance();
		virtual ScriptType* Clone() const;

		void Init();
		void Update(float dt);
		void End();
	};
	bool IsAbove(EntityID, EntityID);
}