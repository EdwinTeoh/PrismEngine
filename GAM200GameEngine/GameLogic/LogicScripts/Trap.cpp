/*******************************************************************************
filename   GameLogic.cpp
author(s)   Zhang Xinyu,xinyu.z@digipen.edu
date        November 25, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Trap.h"
#include "Platforms.h"
#define trapTimer 0.1f;

namespace Engine
{
	Trap* Trap::_instance = nullptr;

	Trap::Trap() :
		_buttonPressed(false),
		_lowerYCoord{ 0.0f },
		_movingDown{ false },
		_movingUp{ false }
	{}

	void Trap::SetScriptName()
	{
		_scriptName = "Trap";
	}
	std::string Trap::GetScriptName()
	{
		return _scriptName;
	}

	void Trap::SetButton(const bool b)
	{
		_buttonPressed = b;
	}
	bool Trap::ButtonPressed()
	{
		return _buttonPressed;
	}

	Trap* Trap::GetInstance()
	{
		_instance = new Trap;
		return _instance;
	}

	ScriptType* Trap::Clone() const
	{
		return new Trap{ *this };
	}

	void Trap::Init()
	{
		_originalCoord = compManager->GetEntityComponent<TransformComponent>(GetOwner())._position._y;
		auto platform = compManager->GetEntityComponent<PlatformComponent>(GetOwner());
		_lowerYCoord = _originalCoord + platform.getDistance()._y + 1.0f;
	}

	void Trap::Update(float dt)
	{
		if (!compManager->CheckForEntity<PlatformComponent>(GetOwner()))
		{
			return;
		}

		auto& platform = compManager->GetEntityComponent<PlatformComponent>(GetOwner());

		if (!compManager->CheckForEntity<TransformComponent>(EntityID(platform.getTrapBar()._x))
			|| !compManager->CheckForEntity<TransformComponent>(EntityID(platform.getTrapBar()._y)))

		{
			return;
		}

		auto & bar1 = compManager->GetEntityComponent<TransformComponent>(EntityID(platform.getTrapBar()._x));
		auto& bar1_rc = compManager->GetEntityComponent<RectColliderComponent>(EntityID(platform.getTrapBar()._x));
		auto& bar2 = compManager->GetEntityComponent<TransformComponent>(EntityID(platform.getTrapBar()._y));
		EntityID _buttonID = platform._buttonID;

		if (Collision::AABBToAABB(Player::GetInstance()->GetOwner(), _buttonID, dt))
		{
			if (IsAbove(Player::GetInstance()->GetOwner(), _buttonID))
			{
				_buttonPressed = true;

				auto& sprite = compManager->GetEntityComponent<SpriteComponent>(_buttonID);
				sprite._texName = "button-red-1.png";
			}
		}
		else
		{
			auto& sprite = compManager->GetEntityComponent<SpriteComponent>(_buttonID);
			sprite._texName = "button-red-0.png";

		}

		if (_buttonPressed)
		{
			TransformComponent& pos = compManager->GetEntityComponent<TransformComponent>(GetOwner());
			if (pos._position._y == _originalCoord)
			{
				_movingDown = true;
				platform._timer = trapTimer;
			}
			if (_movingDown)
			{
				if (pos._position._y > _lowerYCoord)
				{
					pos._position._y -= (platform._speed * dt);
					bar1._scale._y += (platform._speed * dt *2);
					bar1_rc._height = bar1._scale._y;
					bar2._position._y -= (platform._speed * dt);

					if (Collision::AABBToAABB(Player::GetInstance()->GetOwner(), GetOwner(), dt))
					{
						Player::GetInstance()->IsHurt(true);
						if (sJeffState == JeffState::j_Squish)
							ChangeState_Jeff(JeffState::j_Idle);
					}
				}
				else if (pos._position._y <= _lowerYCoord)
				{
					pos._position._y = _lowerYCoord;
					_movingDown = false;
					_movingUp = true;
				}
			}

			if (_movingUp)
			{
				if (platform._timer > 0)
				{
					platform._timer -= dt;
				}
				else if (platform._timer <= 0)
				{
					if (pos._position._y >= (_originalCoord))
					{
						pos._position._y = _originalCoord;
						_movingUp = false;
						_buttonPressed = false;
					}
					else
					{
						if (sJeffState == JeffState::j_Float && Collision::AABBToAABB(Player::GetInstance()->GetOwner(), GetOwner(), dt))
							ChangeState_Jeff(JeffState::j_Squish);
						pos._position._y += (platform._speed * dt);
						if (bar1._scale._y == 3)
							return;
						bar1._scale._y -= (platform._speed * dt *2);
						bar1_rc._height = bar1._scale._y;
						bar2._position._y += (platform._speed * dt);
					}
						
				}
			}
		}
	}

	void Trap::End()
	{
		delete _instance;
	}
}