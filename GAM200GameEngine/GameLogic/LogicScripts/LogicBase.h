#pragma once
#include "../../System/Macro.h"
#include "../../ECS/Factory.h"
#include "../../Physics/Collision.h"
#include "../../Input/Input.h"

namespace Engine
{

	class ScriptType
	{
	protected:
		EntityID _owner;
		std::string _scriptName;

	public:
		void SetOwner(EntityID id) { _owner = id; }
		EntityID GetOwner() { return _owner; }
		virtual void SetScriptName() = 0;
		virtual std::string GetScriptName() = 0;

		ScriptType() :_owner{ static_cast<EntityID>(-1) } {}
		virtual ~ScriptType() = default;

		virtual ScriptType* Clone()const = 0;
		virtual void Init() = 0;
		virtual void Update(float) = 0;
		virtual void End() = 0;
	};
}