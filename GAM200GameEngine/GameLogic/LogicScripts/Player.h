/*******************************************************************************
filename   GameLogic.cpp
author(s)   Zhang Xinyu,xinyu.z@digipen.edu
date        November 25, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "../../System/Macro.h"
#include "LogicBase.h"
#include "../Player_State.h"
#include "../NPC_States.h"

namespace Engine
{
	class Player: public ScriptType
	{
		static Player* _instance;
		bool _crawl;
		bool _floating;
		bool _crouchState;
		bool _isHurt;
		float _defaultGravity;
		float _timer;

		// Change to container of checkpoints?
		EntityID _checkpoint;

	public:
		struct CameraTriggerPoints
		{
			static Vec2D _nextCameraPosition;
			static float _nextZoom;

			Vec2D _playerPosition;
			Vec2D _cameraPosition;
			float _cameraZoom;
			bool _triggered = false;

			CameraTriggerPoints(const float px, const float py, const float cx, const float cy, const float zoom = 16.0f) :
				_playerPosition{ px, py },
				_cameraPosition{ cx, cy },
				_cameraZoom{ zoom }
			{}
		};
		std::vector<CameraTriggerPoints> cameraTriggerPoints;

		bool _CollideWithWall;
		bool _stillClimbing;
		bool _QPressed;
		int _climb;
		int _endState; // 1 is win ,-1 is lose,0 is still alive
		float _playerPositionY;
		float _movementSpeed;
		float _mass;
		Vec4D _colliderScale[3]; // 0 - Default, 1 - Crawl, 2 - squish
		Vec2D _holePosition;
		bool _isHoldingObj;
		EntityID _holdObjID;
		int _backCollider;
		
		bool _canUncrawl = true;
		bool _SKey = true;
		
		EntityID climbPathID = 0;

		bool _isImmune = false;
		int _maxHP = 100;
		int _currHP = 100;
		float _immuneTimer;
		std::map<EntityID, bool> _MoveWallCheck;
		std::map< std::string, EntityID> _spawnObjs;
		std::map<EntityID, float> _doorCoord;//coordinate of the blue door

		unsigned _uncrawlCollisionBitflag = 0x00;

		EntityID _vacuumEntity;

		Player();
		~Player();

		virtual ScriptType* Clone() const;
		static Player* GetInstance();

		virtual void SetScriptName();
		virtual std::string GetScriptName();

		//void ChangeToCrawlState(const bool b);
		void IsHurt(const bool b);
		void InputUpdate();
		void UpdateWallMap();
		void UpdateDoorCoord();
		void OpenDoor();
		void BackToIdle(float countdownSpeed = 1.0f);
		bool CheckInteractable(float dt);
		void CheckClimbing(float dt,EntityID id);
		void UpdateHp();

		bool CheckIfClimb() const { return _climb; }
		bool IsCrawl() { return _crawl; }
		bool IsFloating() { return _floating; }
		float GetGravity() { return _defaultGravity; }

		//void SetCrawl(const bool b) { _crawl = b; }
		//void SetClimb(const bool b) { _climb = b; }
		//void SetGravity(const float f) { _defaultGravity = f; }
		//void SetFloating(const float b) { _floating = b; }

		void UpdateDirection(const bool isFacingLeft);

		void SetCheckPoint(const EntityID id) { _checkpoint = id; }
		EntityID GetCheckPoint() { return _checkpoint; }

		void Init();
		void Update(float dt);
		void End();
	};
	static std::pair<std::string, int> currPlayerHP = {"Level-1.1",100};
}
