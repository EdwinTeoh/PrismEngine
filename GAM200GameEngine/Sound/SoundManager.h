/*******************************************************************************
filename    SoundManager.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#pragma warning(disable: 4505)
#include <string>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include "../Messaging/Messaging.h"
#include <filesystem>
#include <map>
#include <ostream>
#include "SoundEnum.h"

#define p_SoundManager Engine::Audio::SoundManager::GetInstance()

namespace Engine
{
    namespace Audio
    {
        class SoundManager
        {
            FMOD::System* _system;
            FMOD::Channel* _channel;
            FMOD::Sound* _sound;
            FMOD::SoundGroup* _GroupBGM = 0;
            FMOD::SoundGroup* _GroupSFX = 0;
            FMOD::SoundGroup* _GroupLoopSFX = 0;
            std::unordered_map<std::string, FMOD::Sound*> _BGMCont;
            std::unordered_map<std::string, FMOD::Sound*> _SFXCont;
            std::unordered_map<std::string, FMOD::Sound*> _LoopSFXCont;
            std::unordered_map < std::string, FMOD::Channel*> _NowPlayingCont;
            static void HandleMessage(Messaging::AudioMessage msg);

        public:
            SoundManager();
            ~SoundManager();

            static SoundManager* GetInstance();

            int initialize();
            int loadSoundFromPath(std::string pathToFile);
            int loadSoundFromMemory(char* memoryPtr, unsigned int memorySize);
            void playSound();
           
            void playSFX(std::string, bool pause, float vol);
            void playLoopSFX(std::string, bool pause, float vol);
            void playBGM(std::string, bool pause, float vol);
            void playTag(SoundAction act, bool pause, float vol);
            void playComponentSound(std::string audio, bool pause, float vol, bool bgm, bool sfx, bool lsfx);

            void AddBGMpath(std::string filepath);
            void AddSFXpath(std::string filepath);
            void AddLSFXpath(std::string filepath);
            
            bool BGMexist(std::string filename);
            bool SFXexist(std::string filename);
            bool LoopSFXexist(std::string filename);
            bool SoundExist(std::string filename);



            void pauseSound();
            void unpauseSound();
            void stopSound();
            void StopSpecificLoop(std::string filename);

            float getBGMVol();
            float getSFXVol();
            float getLoopSFXVol();

            void setBGMVol(float vol);
            void setSFXVol(float vol);
            void setLoopSFXVol(float vol);

            void increaseBGMVol();
            void increaseSFXVol();
            void increaseLoopSFXVol();

            void decreaseBGMVol();
            void decreaseSFXVol();
            void decreaseLoopSFXVol();

            void StopAllSFX();
            void StopAllBGM();
            void StopAllLoopSFX();

            void SoundUpdate();
            void LoadAllSounds(std::string filepath);
            void StopAllSounds();
            void PauseAllSounds();
            void UnpauseAllSounds();
            void clearCont();

            void SetTagPath(SoundAction act, std::string filepath);
            std::string GetTagPath(SoundAction act);
            void UpdateAudioAssets();

            std::string SoundTag[50];
        };

        void Init();
        void Update();
        void Quit();
    }
}