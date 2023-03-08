#include "Conveyor.h"
namespace Engine
{
	Conveyor* Conveyor::_instance = nullptr;

	Conveyor::Conveyor() :
		_newPos{ 0.0f }
	{}

	void Conveyor::SetScriptName()
	{
		_scriptName = "Conveyor";
	}
	std::string Conveyor::GetScriptName()
	{
		return _scriptName;
	}

	Conveyor* Conveyor::GetInstance()
	{
		/*if (!_instance)
		{*/
			_instance = new Conveyor;
		/*}*/

		return _instance;
	}

	ScriptType* Conveyor::Clone() const
	{
		return new Conveyor{ *this };
	}

	void Conveyor::Init()
	{
		auto pos = compManager->GetEntityComponent<TransformComponent>(GetOwner())._position;
		PlatformComponent pf = compManager->GetEntityComponent<PlatformComponent>(GetOwner());
		_newPos = pf.getDistance()._y / 2 + pos._y;
		
	}

	void Conveyor::Update(float dt)
	{
		
		if (!compManager->CheckForEntity<PlatformComponent>(GetOwner()))
		{
			
			return;
		}

		auto& platform = compManager->GetEntityComponent<PlatformComponent>(GetOwner());

		for (auto i : entityManager->GetEntityList())
		{
			if ((i.GetTag() == "canVacuum") || (i.GetName() == "Player")
				|| (i.GetTag() == "ExplodingBarrel"))
			{
				if (Collision::AABBToAABB(GetOwner(), i.GetID(), dt))
				{
					auto& updatePos = compManager->GetEntityComponent<TransformComponent>(i.GetID());
					
						if (updatePos._position._y < _newPos)
							updatePos._position._y += platform._speed;
						else if (updatePos._position._y >= _newPos)
							updatePos._position._y = _newPos;
						break;
				}
			}
			else
				continue;
		}

	}
	void Conveyor::End()
	{
		delete _instance;
	}
}