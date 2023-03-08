/*******************************************************************************
filename    Scene.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "../Messaging/Messaging.h"
#include "../Serializer/RapidJsonBase.h"
#include <map>
#include <filesystem>
#include "../GameLogic/GameLogic.h"

namespace Engine
{
	namespace Scene
	{
		class SceneManager
		{
			bool _SceneStatus = false; //False to show that scene is not changing, true to show scene is changing
			static std::unordered_map<std::string, std::string> _SceneID; // Scene Name and Path
			std::string CurrentScene; // Scene Name
			std::string PreviousScene;// Scene Name

			void AddScene(std::string path, std::string name);
			void AddScenesFromFile(std::string path);
			void ResetScene();
			static void HandleMessage(Messaging::SceneMessage msg);

		public:
			static std::string initialScene;
			static std::unique_ptr<SceneManager> sceneManager;
			SceneManager(std::string initPath);
			static std::unordered_map<std::string, std::string>& GetScenes();
			void ClearScene();
			void ChangeScene(std::string newScene);
			~SceneManager() = default;
			void SetSceneStatus(bool);
			bool GetSceneStatus();
			const std::string GetCurrentSceneFilename();
			const std::string GetCurrentSceneName(); // Without extension
		};

		void Init(const std::string& path);
		void Quit();
	}
}