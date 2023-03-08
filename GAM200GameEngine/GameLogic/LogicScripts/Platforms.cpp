#include "Platforms.h"
#include "../../Scene/Scene.h"

#define trapTimer 3.0f;
namespace Engine
{
	SwitchPlatform* SwitchPlatform::_instance = nullptr;
	bool startplayed = false;
	bool stopplayed = false;

	SwitchPlatform::SwitchPlatform() :
		_originalPos{ 0.0f,0.0f },
		_buttonPressed(false),
		_triggered{ 0 },
		_movingTo{ false },
		_timer{ 2.0f },
		_movingBack{ false }{}

	void SwitchPlatform::SetScriptName()
	{
		_scriptName = "SwitchPlatform";
	}
	std::string SwitchPlatform::GetScriptName()
	{
		return _scriptName;
	}

	void SwitchPlatform::SetButton(const bool b)
	{
		_buttonPressed = b;
	}
	bool SwitchPlatform::ButtonPressed()
	{
		return _buttonPressed;
	}

	bool IsAbove(EntityID player, EntityID obj)
	{
		auto& player_pos = compManager->GetEntityComponent<TransformComponent>(player)._position;
		auto& gameObj_pos = compManager->GetEntityComponent<TransformComponent>(obj)._position;

		auto player_scale = compManager->GetEntityComponent<TransformComponent>(player)._scale;
		auto gameObj_scale = compManager->GetEntityComponent<TransformComponent>(obj)._scale;
		if (fabs(gameObj_pos._x - player_pos._x) >= (gameObj_scale._x / 2))
			return false;

		if (fabs(gameObj_pos._y - player_pos._y) >= (gameObj_scale._y / 2 + player_scale._y/2 + 0.4f))
			return false;

		return true;
	}

	SwitchPlatform* SwitchPlatform::GetInstance()
	{
		_instance = new SwitchPlatform;
		return _instance;
	}

	ScriptType* SwitchPlatform::Clone() const
	{
		return new SwitchPlatform{ *this };
	}

	void SwitchPlatform::Init()
	{
		_originalPos = compManager->GetEntityComponent<TransformComponent>(GetOwner())._position;
		PlatformComponent pf = compManager->GetEntityComponent<PlatformComponent>(GetOwner());
		_timer = pf._timer;
	}

	void SwitchPlatform::Update(float dt)
	{
		if (!compManager->CheckForEntity<PlatformComponent>(GetOwner()))
		{
			return;
		}

		auto& platform = compManager->GetEntityComponent<PlatformComponent>(GetOwner());
		EntityID _buttonID = platform._buttonID;

		if (entityManager->FindEntity(GetOwner()).GetName() == "Conveyor")
		{
			_buttonID = EntityID(1);
			if (_triggered == 0)
			{
				if (_timer > 0)
				{
					_timer -= Utility::Clock::GetInstance().GetGDT();
					if (!startplayed && _timer < 1.0f)
					{
						startplayed = true;
						stopplayed = false;
					}
				}
				else
				{
					Messaging::AudioMessage message(Messaging::PLAY_LOOPSFX, _ELEVATORMOVE, "ElevatorMoveLR.ogg", false, 2.0f, false, true, false, std::string(""));
					Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
					_buttonPressed = true;
					_movingTo = true;
					_timer = compManager->GetEntityComponent<PlatformComponent>(GetOwner())._timer;
					startplayed = false;
				}
			}
			else if (_triggered == 2)
			{
				Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _ELEVATORMOVE, "ElevatorMoveLR.ogg", false, 2.0f, false, true, false, std::string(""));
				Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
				_buttonPressed = true;
				_movingBack = true;
			}
		}
		else if (entityManager->FindEntity(GetOwner()).GetName() == "Elevator")
		{
			if (Collision::AABBToAABB(Player::GetInstance()->GetOwner(), GetOwner(), dt))
			{
				if (IsAbove(Player::GetInstance()->GetOwner(), GetOwner()))
				{
					if (!_buttonPressed && _triggered == 0)
					{
						_buttonPressed = true;
						_movingTo = true;
						Messaging::AudioMessage message(Messaging::PLAY_LOOPSFX, _ELEVATORMOVE, "ElevatorMoveUD.ogg", false, 2.0f, false, true, false, std::string(""));
						Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
					}	
					else if (!_buttonPressed && _triggered == 2)
					{
						Messaging::AudioMessage message(Messaging::STOP_LOOPED_SOUND, _ELEVATORMOVE, "ElevatorMoveUD.ogg", false, 2.0f, false, true, false, std::string(""));
						Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
						_buttonPressed = true;
						_movingBack = true;

					}
				}

			}
		}
		//open the door in lvl3 which need something on its button
		else if (entityManager->FindEntity(GetOwner()).GetName() == "Door")
		{
			std::map<EntityID, RectColliderComponent>& list
				= compManager->GetComponentList<RectColliderComponent>().GetList();


			for (auto i = list.begin(); i != list.end(); ++i)
			{
				//auto& buttonCollider = ECS::compManager->GetEntityComponent<RectColliderComponent>(_buttonID);

				if (entityManager->FindEntity((*i).first).GetTag() == "NOT TAGGED")
					continue;
				else if (Collision::AABBToAABB(entityManager->FindEntity((*i).first).GetID(), _buttonID, dt))
				{
					if (IsAbove(entityManager->FindEntity((*i).first).GetID(), _buttonID))
					{
						_buttonPressed = true;
						_movingTo = true;

						auto& sprite = compManager->GetEntityComponent<SpriteComponent>(_buttonID);
						sprite._texPath = "Assets/Textures/button-green-1.png";
						std::filesystem::path s_path(sprite._texPath);
						sprite._texName = s_path.filename().string();
						break;
					}
				}
				else
				{
					_buttonPressed = true;
					_movingTo = false;
					_movingBack = true;
					auto& sprite = compManager->GetEntityComponent<SpriteComponent>(_buttonID);
					sprite._texPath = "Assets/Textures/button-green-0.png";
					std::filesystem::path s_path(sprite._texPath);
					sprite._texName = s_path.filename().string();

					//buttonCollider._offset._y = 0.25f;
				}

			}
		}
		else if (Collision::AABBToAABB(Player::GetInstance()->GetOwner(), _buttonID, dt))
		{
			if (!_buttonPressed && _triggered == 0)
			{
				_buttonPressed = true;
				_movingTo = true;
			}
			else if (!_buttonPressed && _triggered == 2)
			{
				_buttonPressed = true;
				_movingBack = true;
			}
		}

		TransformComponent& pos = compManager->GetEntityComponent<TransformComponent>(GetOwner());
		TransformComponent& pos_0 = compManager->GetEntityComponent<TransformComponent>(Player::GetInstance()->GetOwner());
		Vec2D speed, destination;
		destination = _originalPos + platform.getDistance();
		float speedWithTime = platform._speed * dt;
		bool isX = (platform.getDistance()._x != 0) ? true : false;
		float distance = fabs(Math::V2Distance(pos._position, destination));
		if (isX)
		{
			speed = { speedWithTime,0.0f };
			if (speed._x < 0 && pos._position._x <= destination._x) //move left
				distance = -1;
			else if (speed._x > 0 && pos._position._x >= destination._x) //move right
				distance = -1;
		}
		else
		{
			speed = { 0.0,speedWithTime };
			if (speed._y < 0 && pos._position._y <= destination._y) //down
				distance = -1;
			else if (speed._y > 0 && pos._position._y >= destination._y) //up
				distance = -1;
		}
			
		

		if (_buttonPressed & _movingTo)
		{
			_triggered = 1;
			if (distance > 0)
				{
					pos._position += speed;
					for (auto i : entityManager->GetEntityList())
					{
						auto& pos_ = compManager->GetEntityComponent<TransformComponent>(i.GetID());
						if ((i.GetName() == "Player") || (i.GetTag() == "Box") || (i.GetTag() == "ExplodingBarrel"))
							if (Collision::AABBToAABB(GetOwner(), i.GetID(), dt))
							{
								if (i.GetName() == "Player") //update vacuum pos
									compManager->GetEntityComponent<TransformComponent>(Player::GetInstance()->_vacuumEntity)._position += speed;
								pos_._position += speed;
							}
								
					}
				}
				else if (distance < 0)
				{
					pos._position = destination;
					_movingTo = false;
					_triggered = 2;
					_buttonPressed = false;
				}
		}
		else if (_movingBack && _buttonPressed)//move back to original pos
		{
			if (compManager->GetEntityComponent<PlatformComponent>(GetOwner())._moveBack == false)
				return;

			if (_timer > 0)
			{
				_timer -=  Utility::Clock::GetInstance().GetGDT();
			}
			else
			{
				float Backdistance = Math::V2Distance(_originalPos, pos._position);
				if (isX)
				{
					if (speed._x < 0 && pos._position._x >= _originalPos._x) //move left
						Backdistance *= -1;
					else if (speed._x > 0 && pos._position._x <= _originalPos._x) //move right
						Backdistance *= -1;
				}
				else
				{
					if (speed._y < 0 && pos._position._y >= _originalPos._y) //down
						Backdistance *= -1;
					else if (speed._y > 0 && pos._position._y <= _originalPos._y) //up
						Backdistance *= -1;
				}
				speed *= -1;
				if (Backdistance > 0)
				{
					pos._position += speed;
			
					for (auto i : entityManager->GetEntityList())
					{
						auto& pos_ = compManager->GetEntityComponent<TransformComponent>(i.GetID());
						if ((i.GetName() == "Player") || (i.GetTag() == "Box") || (i.GetTag() == "ExplodingBarrel"))
							if (Collision::AABBToAABB(GetOwner(), i.GetID(), dt))
							{
								if (i.GetName() == "Player") //update vacuum pos
								{
									compManager->GetEntityComponent<TransformComponent>(Player::GetInstance()->_vacuumEntity)._position += speed;
								}
									
								pos_._position += speed;

							}
					}
				}
				else if (Backdistance < 0)
				{
					pos._position = _originalPos;
					_movingBack = false;
					_triggered = 0;
					_buttonPressed = false;
					_timer = compManager->GetEntityComponent<PlatformComponent>(GetOwner())._timer;
				}
			}
		}
		if (!_movingTo && _triggered == 2 &&
			entityManager->FindEntity(GetOwner()).GetLayer() == "ToNextLevel")
		{
			Messaging::AudioMessage message1(Messaging::STOP_LOOPED_SOUND, _ELEVATORMOVE, "ElevatorMoveUD.ogg", false, 1.5f, false, true, false, std::string(""));
			Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
			Messaging::AudioMessage message2(Messaging::STOP_LOOPED_SOUND, _ELEVATORMOVE, "ElevatorMoveLR.ogg", false, 1.5f, false, true, false, std::string(""));
			Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message2);

			//if (Collision::AABBToAABB(GetOwner(), Player::GetInstance()->GetOwner(),dt))
			if (pos_0._position._y > pos._position._y)
			{
				Messaging::SceneMessage message(Messaging::SET_CHANGE, "", "", true, "");
				Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message);

				std::string currentName = Scene::SceneManager::sceneManager->GetCurrentSceneName();
				std::string nextScene;
				Scene::SceneManager::sceneManager->SetSceneStatus(true);
				if (currentName == "Level-2.1")
				{
					Scene::SceneManager::sceneManager->ChangeScene("Level-2.2.json");
				}
				if (currentName == "Level-2.2")
				{
					Scene::SceneManager::sceneManager->ChangeScene("Level-2.3.json");
				}
				if (currentName == "Level-3.1")
				{
					Scene::SceneManager::sceneManager->ChangeScene("Level-3.2.json");
				}
				if (currentName == "Level-3.2")
				{
					Scene::SceneManager::sceneManager->ChangeScene("Level-3.3.json");
				}
				//Messaging::SceneMessage message2(Messaging::CHANGE_SCENE, nextScene, "", false, "");
				//Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message2);
			}
			else
			{
				_buttonPressed = true;
				_movingBack = true;
				_triggered = 0;
			}
		}

	}
	void SwitchPlatform::End()
	{
		delete _instance;
	}
}