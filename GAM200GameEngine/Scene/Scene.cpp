/*******************************************************************************
filename    Scene.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "Scene.h"

namespace Engine
{
	namespace Scene
	{
		std::unique_ptr<SceneManager> SceneManager::sceneManager;
		std::unordered_map<std::string, std::string> SceneManager::_SceneID;
		std::string SceneManager::initialScene;

		SceneManager::SceneManager(std::string initPath)
		{
			_SceneID.clear();
			CurrentScene = "";

			if (initPath != "")
			{
				AddScenesFromFile(initPath);
				Messaging::msgManager->GetSystem<Messaging::SceneMessage>().AddReceiver(
					std::function<void(Messaging::SceneMessage)>(HandleMessage));
			}
		}

		void SceneManager::AddScene(std::string path, std::string name)
		{
			_SceneID[name] = path;
		}

		void SceneManager::AddScenesFromFile(std::string path)
		{
			//std::cout << "Loading scene files..." << std::endl;

			for (auto& B : std::filesystem::directory_iterator(path + "/SceneData"))
			{
				if (!B.is_regular_file())
				{
					continue;
				}

				//std::cout << "\t- " << B.path().filename().string() << std::endl;
				const std::string Bdir = B.path().string();
				_SceneID.insert(std::make_pair(B.path().filename().string(), B.path().string()));
			}

			std::cout << "Scene files loaded." << std::endl;
		}

		void SceneManager::SetSceneStatus(bool b)
		{
			_SceneStatus = b;
		}

		bool SceneManager::GetSceneStatus()
		{
			return _SceneStatus;
		}

		const std::string SceneManager::GetCurrentSceneFilename()
		{
			return CurrentScene;
		}

		const std::string SceneManager::GetCurrentSceneName()
		{
			size_t pos = CurrentScene.find(".json");
			return CurrentScene.substr(0, pos);
		}

		void SceneManager::HandleMessage(Messaging::SceneMessage msg)
		{
			switch (msg.EventType)
			{
			case Messaging::RESET_SCENE:
				sceneManager->ResetScene();
				break;
			case Messaging::CHANGE_SCENE:
				sceneManager->ChangeScene(msg._SceneName);
				break;
			case Messaging::ADD_SCENE:
				sceneManager->AddScene(msg._ScenePath, msg._SceneName);
				break;
			case Messaging::SET_CHANGE:
				sceneManager->SetSceneStatus(msg._change);
				break;
			default:
				break;
			}
		}

		void SceneManager::ResetScene()
		{
			Messaging::SerializerMessage message(Messaging::DESERIALIZE_SCENE, _SceneID[CurrentScene], std::string("Deserializing Scene"));
			Messaging::msgManager->GetSystem<Messaging::SerializerMessage>().CreateMessage(message);
		}

		void SceneManager::ChangeScene(std::string newScene)
		{
			_ASSERT(_SceneID.find(newScene) != _SceneID.end());
			if (GetSceneStatus())
			{
				Messaging::SerializerMessage message(Messaging::DESERIALIZE_SCENE, _SceneID[newScene], std::string("Deserializing scene"));
				Messaging::msgManager->GetSystem<Messaging::SerializerMessage>().CreateMessage(message);
				//GameLogic::GetInstance()->ReInit(); //***
				sceneManager->PreviousScene = sceneManager->CurrentScene;
				sceneManager->CurrentScene = newScene;
			}
		}

		void SceneManager::ClearScene()
		{
			_SceneID.clear();
		}
		
		std::unordered_map<std::string, std::string>& SceneManager::GetScenes()
		{
			return _SceneID;
		}

		void Init(const std::string& path)
		{
			SceneManager::sceneManager = std::make_unique<SceneManager>(path);
		}

		void Quit()
		{	

			SceneManager::sceneManager->ClearScene();
			SceneManager::sceneManager.reset();

		}
	}
}