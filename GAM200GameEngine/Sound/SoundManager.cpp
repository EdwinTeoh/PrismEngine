/*******************************************************************************
filename    SoundManager.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 10, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "SoundManager.h"
#include <functional>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

namespace Engine
{
    namespace Audio
    {
        SoundManager* SOUNDMANAGER;
        int MAX_CHNL = 64;
        std::string SoundTag[50];

        SoundManager::SoundManager() :_system(NULL), _channel(NULL), _sound(NULL)
        {
        }

        int SoundManager::initialize()
        {

            FMOD_RESULT result = System_Create(&_system);
            if (result != FMOD_OK)
            {
                return result;
            }
            else
            {
                //Added in sound groups to be used in game.
                _system->init(MAX_CHNL, FMOD_INIT_NORMAL, NULL);
                _system->createSoundGroup(NULL, &_GroupBGM);
                _system->createSoundGroup(NULL, &_GroupSFX);
                _system->createSoundGroup(NULL, &_GroupLoopSFX);
                std::cout << "Audio system loaded." << std::endl;
            }

            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().AddReceiver(
                std::function<void(Messaging::AudioMessage)>(SoundManager::HandleMessage));

            LoadAllSounds("Assets/Audio");

            return 0;
        }

        SoundManager::~SoundManager()
        {
            if (_sound)
            {
                _GroupBGM->release();
                _GroupSFX->release();
                _GroupLoopSFX->release();
                _sound->release();
            }
            if (_system)
            {
                _system->close();
                _system->release();
            }
        }

        SoundManager* SoundManager::GetInstance()
        {
            return SOUNDMANAGER;
        }

        int SoundManager::loadSoundFromPath(std::string pathToFile)
        {
            FMOD_RESULT result = _system->createSound(pathToFile.c_str(), FMOD_LOOP_NORMAL, 0, &_sound);
            return result;
        }

        void SoundManager::LoadAllSounds(std::string filepath)
        {
            std::string pathStr = std::filesystem::current_path().string() + "/Assets/Audio/";

            if (std::filesystem::is_directory(pathStr + "BGM"))
            {
                for (auto& B : std::filesystem::directory_iterator(pathStr + "BGM"))
                {
                    const std::string Bdir = B.path().string();
                    _system->createSound((B.path()).string().c_str(), FMOD_LOOP_NORMAL, 0, &_sound);
                    _BGMCont.insert(std::make_pair(B.path().filename().string(), _sound));
                    _sound->setSoundGroup(_GroupBGM);
                    _GroupBGM->setVolume(0.10f);
                    //std::cout << B.path().filename().string() << std::endl;
                }
            }

            if (std::filesystem::is_directory(pathStr + "SFX"))
            {
                for (auto& S : std::filesystem::directory_iterator(pathStr + "SFX"))
                {
                    const std::string Sdir = S.path().string();
                    _system->createSound((S.path()).string().c_str(), FMOD_LOOP_OFF, 0, &_sound);
                    _SFXCont.insert(std::make_pair(S.path().filename().string(), _sound));
                    _sound->setSoundGroup(_GroupSFX);
                    _GroupSFX->setVolume(0.5f);
                    //std::cout << S.path().filename().string() << std::endl;
                }
            }

            if (std::filesystem::is_directory(pathStr + "LOOPSFX"))
            {
                for (auto& SF : std::filesystem::directory_iterator(pathStr + "LOOPSFX"))
                {
                    const std::string SFdir = SF.path().string();
                    _system->createSound((SF.path()).string().c_str(), FMOD_LOOP_NORMAL, 0, &_sound);
                    _LoopSFXCont.insert(std::make_pair(SF.path().filename().string(), _sound));
                    _sound->setSoundGroup(_GroupLoopSFX);
                    _GroupLoopSFX->setVolume(0.5f);
                    //std::cout << SF.path().filename().string() << std::endl;
                }
            }
        }

        int SoundManager::loadSoundFromMemory(char* memoryPtr, unsigned int memorySize)
        {
            FMOD_CREATESOUNDEXINFO sndinfo = { 0 };
            sndinfo.cbsize = sizeof(sndinfo);
            sndinfo.length = memorySize;
            FMOD_RESULT result = _system->createSound(memoryPtr, FMOD_OPENMEMORY | FMOD_LOOP_NORMAL, &sndinfo, &_sound);
            return result;
        }

        void SoundManager::playSound()
        {
            _system->playSound(_sound, 0, false, &_channel);
        }

        void SoundManager::playSFX(std::string filename, bool pause, float vol)
        {
            auto searchSFX = _SFXCont.find(filename);
            float soundVol;
            if (searchSFX != _SFXCont.end())
            {
                //std::cout << "Found " << searchSFX->first << std::endl;
                //std::cout << "Found " << searchSFX->second << std::endl;
                    FMOD_RESULT result = _system->playSound(searchSFX->second, 0, pause, &_channel);

                    _channel->getVolume(&soundVol);
                    if (soundVol != vol)
                    {
                        _channel->setVolume(vol);
                    }

                    if (result != FMOD_OK)
                    {
                        std::cout << "Cannot play" << std::endl;
                    }
            }

            else
            {
                std::cout << filename << std::endl;
                std::cout << "Sound to play not attached or not found.\n";
            }
        }

        void SoundManager::playLoopSFX(std::string filename, bool pause, float vol)
        {
            auto searchLSFX = _LoopSFXCont.find(filename);
            auto searchPlaying = _NowPlayingCont.find(filename);
            float soundVol;

            if (searchPlaying != _NowPlayingCont.end())
            {
                return;
            }

            if (searchLSFX != _LoopSFXCont.end())
            {
                FMOD_RESULT result = _system->playSound(searchLSFX->second, 0, pause, &_channel);
                _channel->getVolume(&soundVol);
                if (soundVol != vol)
                {
                    _channel->setVolume(vol);
                }
                
                if (result == FMOD_OK)
                    _NowPlayingCont.insert(std::make_pair(searchLSFX->first.c_str(), _channel));

                if (result != FMOD_OK)
                {
                    std::cout << "Cannot play" << std::endl;
                }
            }

            else
            {
                std::cout << filename << std::endl;
                std::cout << "Not found1n";
            }
        }

        void SoundManager::playBGM(std::string filename, bool pause, float vol)
        {
            auto searchBGM = _BGMCont.find(filename);
            float soundVol;

            if (searchBGM != _BGMCont.end())
            {
                _system->playSound(searchBGM->second, 0, pause, &_channel);
                _channel->getVolume(&soundVol);
                if (soundVol != vol)
                {
                    _channel->setVolume(vol);
                }
             
            }

            else
            {
                std::cout << "Not found\n";
            }
        }

        void SoundManager::playComponentSound(std::string audio, bool pause, float vol, bool bgm, bool sfx, bool lsfx)
        {
            if (bgm == true)
            {
                playBGM(audio, pause, vol);
            }

            if (sfx == true)
            {
                playSFX(audio, pause, vol);
            }

            if (lsfx == true)
            {
                playLoopSFX(audio, pause, vol);
            }
        }

        void SoundManager::playTag(SoundAction act, bool pause, float vol)
        {
            playSFX(SoundTag[act], pause, vol);
            playLoopSFX(SoundTag[act], pause, vol);
            playBGM(SoundTag[act], pause, vol);
        }

        void SoundManager::pauseSound()
        {
            bool isPaused;
            _channel->getPaused(&isPaused);

            if (!isPaused)
                _channel->setPaused(true);
        }

        void SoundManager::unpauseSound()
        {
            bool isPaused;
            _channel->getPaused(&isPaused);

            if (isPaused)
                _channel->setPaused(false);
        }

        void SoundManager::stopSound()
        {
            _channel->stop();
        }

        void SoundManager::StopSpecificLoop(std::string filename)
        {
            if (!_NowPlayingCont.size())
            {
                return;
            }

            auto search = _NowPlayingCont.find(filename);

            if (search != _NowPlayingCont.end() && search->first == filename)
            {
                if (!_NowPlayingCont.empty())
                {
                    search->second->stop();
                    _NowPlayingCont.erase(filename);
                }
            }
        }

        void SoundManager::setBGMVol(float vol)
        {
            _GroupBGM->setVolume(vol);
        }

        void SoundManager::setSFXVol(float vol)
        {
            _GroupBGM->setVolume(vol);
        }

        void SoundManager::setLoopSFXVol(float vol)
        {
            _GroupLoopSFX->setVolume(vol);
        }

        float SoundManager::getBGMVol()
        {
            float currVol = 0.0f;
            _GroupBGM->getVolume(&currVol);
            return currVol;
        }

        float SoundManager::getSFXVol()
        {
            float currVol = 0.0f;
            _GroupSFX->getVolume(&currVol);
            return currVol;
        }

        float SoundManager::getLoopSFXVol()
        {
            float currVol = 0.0f;
            _GroupLoopSFX->getVolume(&currVol);
            return currVol;
        }

        void SoundManager::increaseBGMVol()
        {
            float _currvolume = 0.0f;
            _GroupBGM->getVolume(&_currvolume);
            _GroupBGM->setVolume(_currvolume + 0.1f);
        }

        void SoundManager::increaseSFXVol()
        {
            float _currvolume = 0.0f;
            _GroupSFX->getVolume(&_currvolume);
            _GroupSFX->setVolume(_currvolume + 0.1f);
        }

        void SoundManager::increaseLoopSFXVol()
        {
            float _currvolume = 0.0f;
            _GroupLoopSFX->getVolume(&_currvolume);
            _GroupLoopSFX->setVolume(_currvolume + 0.1f);
        }

        void SoundManager::decreaseBGMVol()
        {
            float _currvolume = 0.0f;
            _GroupBGM->getVolume(&_currvolume);
            _GroupBGM->setVolume(_currvolume - 0.1f);
        }

        void SoundManager::decreaseSFXVol()
        {
            float _currvolume = 0.0f;
            _GroupSFX->getVolume(&_currvolume);
            _GroupSFX->setVolume(_currvolume - 0.1f);
        }

        void SoundManager::decreaseLoopSFXVol()
        {
            float _currvolume = 0.0f;
            _GroupLoopSFX->getVolume(&_currvolume);
            _GroupLoopSFX->setVolume(_currvolume - 0.1f);
        }

        void SoundManager::StopAllSounds()
        {
            for (int iter = 0; iter < MAX_CHNL; iter++)
            {
                FMOD::Channel* ref = nullptr;
                FMOD_RESULT _result = SOUNDMANAGER->_system->getChannel(iter, &ref);
                if (_result == FMOD_OK && ref)
                {
                    ref->stop();
                }
            }

            _NowPlayingCont.clear();
        }

        void SoundManager::PauseAllSounds()
        {
            for (int iter = 0; iter < MAX_CHNL; iter++)
            {
                FMOD::Channel* ref = nullptr;
                FMOD_RESULT _result = SOUNDMANAGER->_system->getChannel(iter, &ref);
                if (_result == FMOD_OK && ref)
                {
                    ref->setPaused(1);
                }
            }
        }

        void SoundManager::UnpauseAllSounds()
        {
            for (int iter = 0; iter < MAX_CHNL; iter++)
            {
                FMOD::Channel* ref = nullptr;
                FMOD_RESULT _result = SOUNDMANAGER->_system->getChannel(iter, &ref);
                if (_result == FMOD_OK && ref)
                {
                    ref->setPaused(0);
                }
            }
        }

        void SoundManager::AddBGMpath(std::string filepath)
        {
            _system->createSound(filepath.c_str(), FMOD_LOOP_NORMAL, 0, &_sound);
            _BGMCont.insert(std::make_pair(filepath, _sound));
            _sound->setSoundGroup(_GroupBGM);
        }

        void SoundManager::AddSFXpath(std::string filepath)
        {
            _system->createSound(filepath.c_str(), FMOD_LOOP_OFF, 0, &_sound);
            _SFXCont.insert(std::make_pair(filepath, _sound));
            _sound->setSoundGroup(_GroupSFX);
        }

        void SoundManager::AddLSFXpath(std::string filepath)
        {
            _system->createSound(filepath.c_str(), FMOD_LOOP_NORMAL, 0, &_sound);
            _LoopSFXCont.insert(std::make_pair(filepath, _sound));
            _sound->setSoundGroup(_GroupLoopSFX);
        }

        void SoundManager::StopAllSFX()
        {
            SOUNDMANAGER->_GroupSFX->stop();
        }

        void SoundManager::StopAllBGM()
        {
            SOUNDMANAGER->_GroupBGM->stop();
        }

        void SoundManager::StopAllLoopSFX()
        {
            SOUNDMANAGER->_GroupLoopSFX->stop();
        }

        void SoundManager::SoundUpdate()
        {
            //It is a must to update every loop
            if (_system)
                _system->update();
        }

        void SoundManager::clearCont()
        {
            _BGMCont.clear();
            _SFXCont.clear();
            _LoopSFXCont.clear();
        }

        void SoundManager::UpdateAudioAssets()
        {
            if (_sound)
            {
                _GroupBGM->release();
                _GroupSFX->release();
                _GroupLoopSFX->release();
            }

            LoadAllSounds("Assets/Audio");
        }

        void SoundManager::SetTagPath(SoundAction act, std::string filename)
        {
            SoundTag[act] = filename;
        }

        std::string SoundManager::GetTagPath(SoundAction act)
        {
            return SoundTag[act];
        }

        bool SoundManager::BGMexist(std::string filename)
        {
            for (auto itr = _BGMCont.find(filename); itr != _BGMCont.end(); itr++)
            {
                if (filename == itr->first)
                    return true;
            }
            return false;
        }

        bool SoundManager::SFXexist(std::string filename)
        {
            for (auto itr = _SFXCont.find(filename); itr != _SFXCont.end(); itr++)
            {
                if (filename == itr->first)
                    return true;
            }
            return false;
        }

        bool SoundManager::LoopSFXexist(std::string filename)
        {
            for (auto itr = _LoopSFXCont.find(filename); itr != _LoopSFXCont.end(); itr++)
            {
                if (filename == itr->first)
                    return true;
            }
            return false;
        }

        bool SoundManager::SoundExist(std::string filename)
        {
            return (BGMexist(filename) || SFXexist(filename) || LoopSFXexist(filename));
        }

        void SoundManager::HandleMessage(Messaging::AudioMessage msg)
        {
            switch (msg.EventType)
            {
            case Messaging::PLAY_AUDIO:
                SOUNDMANAGER->playSound();
                break;
            case Messaging::STOP_AUDIO:
                SOUNDMANAGER->stopSound();
                break;
            case Messaging::PAUSE_AUDIO:
                SOUNDMANAGER->pauseSound();
                break;
            case Messaging::UNPAUSE_AUDIO:
                SOUNDMANAGER->unpauseSound();
                break;

            case Messaging::INCREASE_BGMVOL:
                SOUNDMANAGER->increaseBGMVol();
                break;
            case Messaging::INCREASE_SFXVOL:
                SOUNDMANAGER->increaseSFXVol();
                break;
            case Messaging::INCREASE_LOOPSFXVOL:
                SOUNDMANAGER->increaseLoopSFXVol();
                break;
            
            case Messaging::DECREASE_BGMVOL:
                SOUNDMANAGER->decreaseBGMVol();
                break;
            case Messaging::DECREASE_SFXVOL:
                SOUNDMANAGER->decreaseSFXVol();
                break;
            case Messaging::DECREASE_LOOPSFXVOL:
                SOUNDMANAGER->decreaseLoopSFXVol();
                break;
         
            case Messaging::GET_BGMVOL:
                SOUNDMANAGER->getBGMVol();
                break;
            case Messaging::GET_SFXVOL:
                SOUNDMANAGER->getSFXVol();
                break;
            case Messaging::GET_LOOPSFXVOL:
                SOUNDMANAGER->getLoopSFXVol();
                break;

            case Messaging::SET_BGMVOL:
                SOUNDMANAGER->setBGMVol(msg._volume);
                break;
            case Messaging::SET_SFXVOL:
                SOUNDMANAGER->setSFXVol(msg._volume);
                break;
            case Messaging::SET_LOOPSFXVOL:
                SOUNDMANAGER->setLoopSFXVol(msg._volume);
                break;

            case Messaging::PLAY_SFX:
                SOUNDMANAGER->playSFX(msg._filename, msg._pause, msg._volume);
                break;
            case Messaging::PLAY_BGM:
                SOUNDMANAGER->playBGM(msg._filename, msg._pause, msg._volume);
                break;
            case Messaging::PLAY_LOOPSFX:
                SOUNDMANAGER->playLoopSFX(msg._filename, msg._pause, msg._volume);
                break;
            case Messaging::PLAY_TAG:
                SOUNDMANAGER->playTag(msg._act, msg._pause, msg._volume);
                break;
            case Messaging::PLAY_COMPONENT:
                SOUNDMANAGER->playComponentSound(msg._filename, msg._pause, msg._volume, msg._bgm, msg._sfx, msg._lsfx);
                break;

            case Messaging::STOP_SFXGROUP:
                SOUNDMANAGER->StopAllSFX();
                break;
            case Messaging::STOP_BGMGROUP:
                SOUNDMANAGER->StopAllBGM();
                break;
            case Messaging::STOP_LOOPSFXGROUP:
                SOUNDMANAGER->StopAllLoopSFX();
                break;

            case Messaging::LOAD_FROM_PATH:
                SOUNDMANAGER->loadSoundFromPath(msg._filename);
                break;
            case Messaging::LOAD_ALL_SOUNDS:
                SOUNDMANAGER->LoadAllSounds(msg._filename);
                break;

            case Messaging::ADD_BGM:
                SOUNDMANAGER->AddBGMpath(msg._filename);
                break;
            case Messaging::ADD_SFX:
                SOUNDMANAGER->AddSFXpath(msg._filename);
                break;
            case Messaging::ADD_LSFX:
                SOUNDMANAGER->AddLSFXpath(msg._filename);
                break;

            case Messaging::BGMEXIST:
                SOUNDMANAGER->BGMexist(msg._filename);
                break;
            case Messaging::SFXEXIST:
                SOUNDMANAGER->SFXexist(msg._filename);
                break;
            case Messaging::SOUNDEXIST:
                SOUNDMANAGER->SoundExist(msg._filename);
                break;
            case Messaging::LOOPSFXEXIST:
                SOUNDMANAGER->LoopSFXexist(msg._filename);
                break;
                
            case Messaging::PAUSE_ALL_AUDIO:
                SOUNDMANAGER->PauseAllSounds();
                break;
            case Messaging::UNPAUSE_ALL_AUDIO:
                SOUNDMANAGER->UnpauseAllSounds();
                break;
            case Messaging::STOP_ALL_AUDIO:
                SOUNDMANAGER->StopAllSounds();
                break;
            case Messaging::STOP_LOOPED_SOUND:
                SOUNDMANAGER->StopSpecificLoop(msg._filename);
                break;
            case Messaging::UPDATE_AUDIO_ASSET:
                SOUNDMANAGER->UpdateAudioAssets();
                break;

            default:
                break;

            }
        }

        void Init()
        {
            SOUNDMANAGER = new SoundManager();
            SOUNDMANAGER->initialize();
        }

        void Update()
        {
            SOUNDMANAGER->SoundUpdate();
        }

        void Quit()
        {
            SOUNDMANAGER->clearCont();
            delete SOUNDMANAGER;
            SOUNDMANAGER = nullptr;
        }
    }
}