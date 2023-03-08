/*******************************************************************************
filename    GameLogic.h
author(s)   Zhang Xinyu,xinyu.z@digipen.edu
date        November 7, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "../ECS/IComponent.h"
#include "../ECS/Factory.h"
#include "LogicScripts/Player.h"
#include "LogicScripts/Trap.h"
#include "LogicScripts/Obstacle.h"
#include "LogicScripts/ExplodingBarrel.h"
#include "LogicScripts/Platforms.h"
#include "LogicScripts/Conveyor.h"

#define p_GameLogic Engine::GameLogic::GetInstance()

namespace Engine
{
	class GameLogic
	{
		std::map<std::pair<EntityID, std::string>, ScriptType*> _allBehaviour;
		std::vector<std::string> _scriptsNames;
		static GameLogic* _instance;
	public:
		static GameLogic* GetInstance();

		std::string nextLevel;

		static bool _gameIsPaused;

		static bool _SceneReInit;

		void LoadLevelFile(const std::string& file, Engine::ECS::Factory*);

		//Get list of scripts names
		std::vector<std::string>& GetScriptsName();
		void CreateScriptList();

		std::map<std::pair<EntityID, std::string>, ScriptType*>& GetScripts();

		//Store all the logic components in _allBehaviour
		void InitScripts();

		// Add new scipt to _allBehaviour
		void AddScript(std::string scriptName,EntityID id);

		//// Delete scipt from _allBehaviour
		void DeleteScript(std::string scriptName, EntityID id);

		//Pointer to the script for a specific entity
		ScriptType* GetScript(std::string, EntityID);
		void ReInit();

		// -1 : lose
		// 0 : in game
		// 1 : win
		void SetEndState(int endstate, std::string text= "");

		void Init();
		void Update();
		void Exit();
	};
};

