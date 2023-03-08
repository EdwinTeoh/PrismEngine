/*******************************************************************************
filename    Massaging.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "../System/Macro.h"
#include <string.h>
#include <queue>
#include <functional>
#include "../Sound/SoundEnum.h"

namespace Engine
{
	namespace Messaging
	{
		enum MsgType
		{
			ECS,
			PHYSICS,
			OTHERS
		};

		enum PhysicsEvents
		{
			//Types of physics event
		};

		enum ECSEvents
		{
			ADD_COMPONENT,
			REMOVE_COMPONENT,
			CREATE_ENTITY,
			REMOVE_ENTITY
		};

		enum SerializerEvents
		{
			FACTORIZE,
			DEFACTORIZE,
			SERIALIZE_SCENE,
			DESERIALIZE_SCENE
		};

		enum AudioEvents
		{
			PLAY_AUDIO,
			STOP_AUDIO,
			PAUSE_AUDIO,
			UNPAUSE_AUDIO,
			INCREASE_BGMVOL,
			DECREASE_BGMVOL,
			INCREASE_SFXVOL,
			DECREASE_SFXVOL,
			INCREASE_LOOPSFXVOL,
			DECREASE_LOOPSFXVOL,
			GET_BGMVOL,
			GET_SFXVOL,
			GET_LOOPSFXVOL,
			SET_BGMVOL,
			SET_SFXVOL,
			SET_LOOPSFXVOL,
			LOAD_FROM_PATH,
			LOAD_ALL_SOUNDS,
			PAUSE_ALL_AUDIO,
			UNPAUSE_ALL_AUDIO,
			STOP_ALL_AUDIO,
			PLAY_SFX,
			PLAY_BGM,
			PLAY_LOOPSFX,
			PLAY_COMPONENT,
			STOP_BGMGROUP,
			STOP_SFXGROUP,
			STOP_LOOPSFXGROUP,
			UPDATE_AUDIO_ASSET,
			ADD_BGM,
			ADD_SFX,
			ADD_LSFX,
			PLAY_TAG,
			SET_TAG_PATH,
			GET_TAG_PATH,
			BGMEXIST,
			SFXEXIST,
			LOOPSFXEXIST,
			SOUNDEXIST,
			STOP_LOOPED_SOUND
		};

		enum SceneEvents
		{
			ADD_SCENE,
			CHANGE_SCENE, 
			RESET_SCENE,
			SET_CHANGE
		};

		enum ResourceEvents
		{
			LoadResource,
			ReleaseResource
		};

		class Message
		{
			std::string EventMSG;

		public:
			Message(const std::string& msg);
			Message(const Message& msg) = default;
			Message(Message&& msg) = default;
			Message(std::string&& msg);
			Message& operator=(const Message& msg) = default;
			Message& operator=(Message&& msg) = default;
			std::string getEvent();
			virtual ~Message() = default;
		};

		class PhysicsMessage : public Message
		{
		public:
			PhysicsEvents EventType;
			EntityID ID;
			Vec2D vec2D;

			PhysicsMessage(PhysicsEvents type, EntityID id, Vec2D vec, std::string msg = "") :
				EventType{ type }, ID{ id }, vec2D{ vec }, Message(msg){}
		};

		class ECSMessage : public Message
		{
		public:
			ECSEvents EventType;
			EntityID ID;
			Vec2D vec2D;
		};

		class SerializerMessage : public Message
		{
		public:
			SerializerEvents EventType;
			//EntityID ID;
			std::string filepath;

			SerializerMessage(SerializerEvents type, std::string path, std::string msg = "") :
				EventType{ type }, filepath{ path }, Message(msg){}
		};

		class SceneMessage : public Message
		{
		public:
			SceneEvents EventType;
			std::string _SceneName;
			std::string _ScenePath;
			bool _change = false;

			SceneMessage(SceneEvents type, std::string sceneName, std::string scenePath, bool change, std::string msg = "") :
				EventType{ type }, _SceneName{ sceneName }, _ScenePath{ scenePath }, _change{ change }, Message(msg){}
		};

		class AudioMessage : public Message
		{
		public:
			AudioEvents EventType;
			SoundAction _act;
			std::string _filename; //Name of sound
			bool _pause;
			float _volume;
			bool _bgm;
			bool _sfx;
			bool _lsfx;

			AudioMessage(AudioEvents type, SoundAction act, std::string filename = "", bool pause = false, float volume = 1.0f, bool bgm = false, bool sfx = false, bool lsfx = false, std::string msg = "")
				: EventType{ type }, _act{ act }, _filename{ filename }, _pause{ pause }, _volume{ volume }, _bgm{ bgm }, _sfx{ sfx }, _lsfx{ lsfx }, Message(msg)
			{}
		};

		class ResourceMessage : public Message
		{
		public:
			ResourceEvents EventType;

			ResourceMessage(ResourceEvents type, std::string msg = "") :
				EventType{ type }, Message(msg){}
		};


		template<typename MsgType>
		class MessageSystem
		{
			using MSG_FUNC = std::function<void(MsgType)>;
			std::queue<MsgType> _messages;
			std::vector<MSG_FUNC> _receivers;
		
		public:
			MessageSystem() = default;

			void AddReceiver(MSG_FUNC receiver)
			{
				_receivers.push_back(receiver);
			}

			void CreateMessage(MsgType message)
			{
				_messages.push(message);
			}

			void UpdateEvents()
			{
				while (!_messages.empty())
				{
					for (MSG_FUNC receiver : _receivers)
					{
						receiver(_messages.front());
					}
					_messages.pop();
				}
			}

			void ClearAll()
			{
				_receivers.clear();
			}
		};


		class MessageManager
		{
			MessageSystem <PhysicsMessage> _physics;
			MessageSystem <ECSMessage> _ecs;
			MessageSystem <SerializerMessage> _serialize;
			MessageSystem <SceneMessage> _scene;
			MessageSystem <AudioMessage> _audio;
			MessageSystem <ResourceMessage> _resource;

		public:
			
			template <typename MsgType>
			auto& GetSystem()
			{
				
				if constexpr (std::is_same<MsgType, PhysicsMessage>::value)
				{
					return _physics;
				}
				else if constexpr (std::is_same<MsgType, ECSMessage>::value)
				{
					return _ecs;
				}
				else if constexpr (std::is_same<MsgType, SerializerMessage>::value)
				{
					return _serialize;
				}
				else if constexpr (std::is_same<MsgType, SceneMessage>::value)
				{
					return _scene;
				}
				else if constexpr (std::is_same<MsgType, AudioMessage>::value)
				{
					return _audio;
				}
				else if constexpr (std::is_same<MsgType, ResourceMessage>::value)
				{
					return _audio;
				}
				else
				{
					_ASSERT_EXPR(0, "No messaging system for given type found");
				}
			}

			void UpdateAllSystems(); 
			void ClearAllSystems();
		};

		extern MessageManager* msgManager;

		void Init();
		void Update();
		void Quit();
	}
}