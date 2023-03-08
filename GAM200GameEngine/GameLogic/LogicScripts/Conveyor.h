#pragma once
#include "../../System/Macro.h"
#include "LogicBase.h"
#include "Player.h"

using namespace Engine::ECS;
namespace Engine
{
	class Conveyor : public ScriptType
	{
		static Conveyor* _instance;
		float _newPos;
	public:
		Conveyor();
		~Conveyor() {}

		virtual void SetScriptName();
		virtual std::string GetScriptName();

		static Conveyor* GetInstance();
		virtual ScriptType* Clone() const;

		void Init();
		void Update(float dt);
		void End();
	};
}

