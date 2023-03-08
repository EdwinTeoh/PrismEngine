/*******************************************************************************
filename   GameLogic.cpp
author(s)   Zhang Xinyu,xinyu.z@digipen.edu
date        November 7, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "GameLogic.h"
#include "../Clock/Clock.h"
#include "../ECS/Rigidbody.h"
#include "../ECS/Transform.h"
#include "../ECS/RectCollider.h"
#include "../Physics/Collision.h"
#include "../UI/PE_UI.h"
#include <string>

using namespace Engine;
using namespace ECS;

namespace Engine
{
	GameLogic* GameLogic::_instance = nullptr;

	bool GameLogic::_gameIsPaused;

	bool GameLogic::_SceneReInit = false;

	void GameLogic::CreateScriptList()
	{
		_scriptsNames.push_back("Player");
		_scriptsNames.push_back("Trap");
		_scriptsNames.push_back("Obstacle");
		_scriptsNames.push_back("ExplodingBarrel");
		_scriptsNames.push_back("SwitchPlatform");
		_scriptsNames.push_back("Conveyor");
	}

	std::map<std::pair<EntityID, std::string>, ScriptType*>& GameLogic::GetScripts()
	{
		return _allBehaviour;
	}

	//Get list of scripts names
	std::vector<std::string>& GameLogic::GetScriptsName()
	{
		return _scriptsNames;
	}

	// Add new scipt to _allBehaviour
	void GameLogic::AddScript(std::string scriptName, EntityID id)
	{
		ScriptType* p;
		if (scriptName == "Player")
		{
			p = Player::GetInstance();
		}
		else if (scriptName == "SwitchPlatform")
		{
			p = SwitchPlatform::GetInstance();
		}
		else if (scriptName == "Trap")
		{
			p = Trap::GetInstance();
		}
		else if (scriptName == "Obstacle")
		{
			p = Obstacle::GetInstance();
		}
		else if (scriptName == "ExplodingBarrel")
		{
			p = ExplodingBarrel::GetInstance();
		}
		else if (scriptName == "Conveyor")
		{
			p = Conveyor::GetInstance();
		}
		_allBehaviour.insert({ std::pair<EntityID, std::string>
					(id, scriptName),p });
	}

	//// Delete scipt from _allBehaviour
	void GameLogic::DeleteScript(std::string scriptName, EntityID id)
	{
		_allBehaviour.erase(std::pair<EntityID, std::string>(id, scriptName));
	}

	//Pointer to the script for a specific entity
	ScriptType* GameLogic::GetScript(std::string scriptName, EntityID id)
	{
		auto type = _allBehaviour.find(std::pair<EntityID, std::string>(id, scriptName))->second;
		return type;
	}

	//Store all the logic components in _allBehaviour
	void GameLogic::InitScripts()
	{
		for (auto i = entityManager->GetEntityList().begin();
			i != entityManager->GetEntityList().end(); ++i)
		{
			for (auto it = _scriptsNames.begin();
				it != _scriptsNames.end(); ++it)
			{
				if ((*i).GetTag() == *it)
				{
					AddScript(*it, (*i).GetID());
					GetScript(*it, (*i).GetID())->SetOwner((*i).GetID());

					if ((*it) == "ExplodingBarrel")
					{
						ExplodingBarrel::GetInstance()->SetTag((*i).GetTag());
					}
				}
				else
					continue;
			}
		}
	}

	void GameLogic::LoadLevelFile(const std::string& filename, Factory* factory)
	{
		if (ECS::compManager->CheckForList<SpriteComponent>())
		{
			//ECS::compManager->ClearAllComponents();
		}
		factory->DeserializeFile(filename);
	}

	GameLogic* GameLogic::GetInstance()
	{
		if (!_instance)
		{
			_instance = new GameLogic;
		}

		return _instance;
	}

	void GameLogic::Init()
	{
		if (!_allBehaviour.empty())
			_allBehaviour.clear();

		if (_scriptsNames.empty())
			CreateScriptList();

		InitScripts();

		for (auto& [id, iter] : _allBehaviour)
		{
			iter->Init();
		}
		Player::GetInstance()->_endState = 0;
	}

	void GameLogic::ReInit()
	{
		_allBehaviour.clear();
		SetEndState(0);
		InitScripts();

		for (auto& [id, iter] : _allBehaviour)
		{
			iter->Init();
		}
	}

	void GameLogic::SetEndState(int endstate, std::string text)
	{
		Player::GetInstance()->_endState = endstate;

		switch (endstate)
		{
		case -1:
		{
			Player::GetInstance()->_endState = -1;
			Messaging::SceneMessage changeSceneMsg(Messaging::SET_CHANGE, "", "", true, "");
			Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(changeSceneMsg);
			std::string currentName = Scene::SceneManager::sceneManager->GetCurrentSceneName();
			nextLevel = currentName;
			Messaging::AudioMessage message1(Messaging::STOP_ALL_AUDIO, _NONE, "", false, 0.7f, false, false, false, std::string(""));
			Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
			// Lose state
			//p_UISystem->blackPanel->_enabled = true;
			//p_UISystem->loseText->_enabled = true;
			//p_UISystem->winText->_enabled = false;
			for (auto& e : p_UISystem->_canvas->_GameLevelUI)
			{
				e->_enabled = false;
			}
			p_UISystem->_canvas->_GameLevelUI[0]->_enabled = true;
			std::cout << text << std::endl;
			Messaging::AudioMessage message(Messaging::PLAY_SFX, _LOSE, "Lose.ogg", false, 0.7f, false, true, false, std::string(""));
			Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
			//p_Editor->_isPaused = true;
			//GameLogic::_gameIsPaused = true;
			Player::GetInstance()->UpdateHp();
			break;
		}
		case 0:
		{
			Player::GetInstance()->_endState = 0;
			p_UISystem->blackPanel->_enabled = false;
			p_UISystem->winText->_enabled = false;
			p_UISystem->loseText->_enabled = false;
			nextLevel = text;
			for (auto& e : p_UISystem->_canvas->_GameLevelUI)
			{
				e->_enabled = true;
			}
			p_UISystem->_canvas->_GameLevelUI[0]->_enabled = false;
			p_UISystem->_canvas->_GameLevelUI[1]->_enabled = false;
			break; 
		}
		case 1:
		{
			ChangeState_Jeff(JeffState::j_Idle);

			Player::GetInstance()->_endState = 1;
			
			nextLevel = text;
			Messaging::AudioMessage message(Messaging::PLAY_SFX, _WIN,
				"Win.ogg", false, 0.7f, false, true, false, std::string(""));
			Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);

			// Win state
			//p_UISystem->blackPanel->_enabled = true;
			//p_UISystem->winText->_enabled = true;
			//p_UISystem->loseText->_enabled = false;

			for (auto& e : p_UISystem->_canvas->_GameLevelUI)
			{
				e->_enabled = false;
			}
			p_UISystem->_canvas->_GameLevelUI[1]->_enabled = true;
			//p_Editor->_isPaused = true;
			//GameLogic::_gameIsPaused = true;
			break;

		}
		default:
			break;
		}
	}

	void GameLogic::Update()
	{
		Utility::Clock::GetInstance().Start(Utility::SystemCode::LOGIC);
		Utility::Clock::GetInstance().GetDT(Utility::SystemCode::LOGIC);

		if (Player::GetInstance()->_endState == 1)
		{
			if (ECS::compManager->CheckForEntity<TransformComponent>(Player::GetInstance()->_vacuumEntity))
			{
				auto& pos
					= ECS::compManager->GetEntityComponent<TransformComponent>(Player::GetInstance()->_vacuumEntity);

				auto& playerPOS
					= ECS::compManager->GetEntityComponent<TransformComponent>(Player::GetInstance()->GetOwner());
				pos._position._x = playerPOS._position._x;
				pos._position._y = playerPOS._position._y;
			}

			if (p_Input->KeyIsPressed(GLFW_KEY_SPACE))
			{
				Messaging::AudioMessage message5(Messaging::STOP_ALL_AUDIO, _NONE,
					"", false, 0.7f, false, false, false, std::string(""));
				Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message5);
				//Messaging::AudioMessage message1(Messaging::STOP_ALL_AUDIO, _NONE,
				//	"", false, 0.7f, false, false, false, std::string(""));
				//Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
				// Scene::SceneManager::sceneManager->SetSceneStatus(true);
				//	Scene::SceneManager::sceneManager->ChangeScene("Menu.json");  
				std::cout << "Next Level: " << nextLevel << std::endl;
				Messaging::SceneMessage message(Messaging::SET_CHANGE, "", "", true, "");
				Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message);
				Messaging::SceneMessage message4(Messaging::CHANGE_SCENE, nextLevel, "", false, "");
				Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message4);
				
				Messaging::AudioMessage message2(Messaging::PLAY_LOOPSFX, _LEVEL,
					"Level1.ogg", false, 1.5f, true, false, false, std::string(""));
				Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message2);
				SetEndState(0, "");
			}
			return;
		}

		if (Player::GetInstance()->_endState == -1)
		{
		
			std::string currentName = Scene::SceneManager::sceneManager->GetCurrentSceneName();

			if (p_Input->KeyIsPressed(GLFW_KEY_SPACE))
			{
				Messaging::AudioMessage message1(Messaging::STOP_ALL_AUDIO, _NONE,
					"", false, 0.7f, false, false, false, std::string(""));
				Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message1);
				// Scene::SceneManager::sceneManager->SetSceneStatus(true);
				//	Scene::SceneManager::sceneManager->ChangeScene("Menu.json");  
				Messaging::SceneMessage message(Messaging::SET_CHANGE, "", "", true, "");
				Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message);
				Messaging::SceneMessage message4(Messaging::RESET_SCENE, nextLevel, "", false, "");
				Messaging::msgManager->GetSystem<Messaging::SceneMessage>().CreateMessage(message4);
				SetEndState(0, "");
				Messaging::AudioMessage message2(Messaging::PLAY_LOOPSFX, _LEVEL,
					"Level1.ogg", false, 1.5f, true, false, false, std::string(""));
				Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message2);
			}
			return;
		}

		if (Scene::SceneManager::sceneManager->GetCurrentSceneName() != "Menu" &&
			Scene::SceneManager::sceneManager->GetCurrentSceneName() != "Intro" &&
			Scene::SceneManager::sceneManager->GetCurrentSceneName() != "Outro" &&
			p_Input->KeyIsPressed(GLFW_KEY_ESCAPE))
		{
			_gameIsPaused = true;
		}

		if (ECS::compManager->CheckForList<RectColliderComponent>())
		{
			float dt = 1.0f / 60.0f;
			for (auto& [id, iter] : _allBehaviour)
			{
				iter->Update(dt);
			}
		}

		Utility::Clock::GetInstance().Update(Utility::SystemCode::LOGIC);
	}

	void GameLogic::Exit()
	{
		for (auto& [id, iter] : _allBehaviour)
		{
			if (entityManager->EntityExist(iter->GetOwner()))
			{
				iter->End();
			}
		}
		delete _instance;
	}
}
