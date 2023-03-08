/*******************************************************************************
filename    Systems.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Systems.h"
#include "../AI/AISystem.h"
#include "../Messaging/Messaging.h"
#include "../Physics/Physics.h"
#include "../Physics/Collision.h"
#include "../Window/Window.h"
#include "../UI/PE_UI.h"

namespace Engine
{
    std::vector<ISystem*> ISystem::_systems;
    
    double accumulatedTime = 0.0; // one time definition
    int currentNumberOfSteps = 0;
    double fixedDT = 1.0 / 60.0;

    void SystemsInit()
    {
        PE_Logger::D_Logger::GetInstance()->Init();
        p_Editor->Init();
        Messaging::Init();
        ECS::Init();
        p_PhysicsSystem->Init();
        p_AISystem->Init();
        Scene::Init("Assets");
        Animation::Init(); 
        Audio::Init();


        p_Graphics->Init();
        GameLogic::GetInstance()->Init();
        p_UISystem->Init();

        Scene::SceneManager::sceneManager->SetSceneStatus(true);
        Scene::SceneManager::sceneManager->ChangeScene(Scene::SceneManager::initialScene);
        Scene::SceneManager::sceneManager->SetSceneStatus(false);

        p_Clock.G_RuntimeStart();
    }
    
    void SystemsUpdate()
    {
        int focused = glfwGetWindowAttrib(Engine::Window::_pWindow, GLFW_FOCUSED);
        int iconified = glfwGetWindowAttrib(Engine::Window::_pWindow, GLFW_ICONIFIED);

        if (!focused && iconified)
        {
            p_SoundManager->PauseAllSounds();
            return;
        }
        p_SoundManager->UnpauseAllSounds();

        p_Clock.G_Start();

        currentNumberOfSteps = 0; // reset
        accumulatedTime += p_Clock.GetGDT();

        while (accumulatedTime >= fixedDT)
        {
            accumulatedTime -= fixedDT; // save the exact accumulated time diff.
            currentNumberOfSteps++;
        }

        if (!Scene::SceneManager::sceneManager->GetSceneStatus())
        {
            ECS::Update();
            //p_Input->Update();
            if (!p_Editor->_isPaused)
            {
                if (!GameLogic::GetInstance()->_gameIsPaused)
                {
                    //InputSystem::GetInstance()->GameKeyPressUpdates();
                    GameLogic::GetInstance()->Update();
                    p_PhysicsSystem->Update(currentNumberOfSteps);
                    Collision::Update(currentNumberOfSteps);
                    p_AISystem->Update(currentNumberOfSteps);
                    Animation::Update();
                }
                p_UISystem->Update();
            }

            PE_Logger::D_Logger::GetInstance()->Update();
            p_Editor->Update();

            p_Graphics->Update(); // MUST be after the editor
        }

        Audio::Update();
        Messaging::Update();
        p_Clock.G_Update();
        Scene::SceneManager::sceneManager->SetSceneStatus(false);

        //if (p_Input->KeyIsPressed(GLFW_KEY_SPACE))
        //{
        //    if (p_Editor->_isPaused == true)
        //    {
        //        if (p_UISystem->_canvas->_GameLevelUI[2]->_enabled == false)
        //        {
        //            Scene::SceneManager::sceneManager->SetSceneStatus(true);
        //            Scene::SceneManager::sceneManager->ChangeScene("Menu.json");
        //            Messaging::AudioMessage message(Messaging::STOP_ALL_AUDIO, _NONE, "", false, 0.7f, false, true, false, std::string(""));
        //            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
        //        }

        //        if (p_UISystem->_canvas->_GameLevelUI[1]->_enabled == true)
        //        {
        //            Scene::SceneManager::sceneManager->SetSceneStatus(true);
        //            Scene::SceneManager::sceneManager->ChangeScene("Menu.json");
        //            Messaging::AudioMessage message(Messaging::STOP_ALL_AUDIO, _NONE, "", false, 0.7f, false, true, false, std::string(""));
        //            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
        //        }

        //        else
        //        p_UISystem->DisableNext();
        //        p_Editor->_isPaused = false;
        //    }
        //}

        p_Input->Update();
    }
    
    void SystemsExit()
    {
        p_Editor->CleanUp();
        p_UISystem->Exit();
        InputSystem::GetInstance()->Exit();
        Animation::Exit();

        p_Graphics->Exit();
        p_AISystem->Exit();
        p_PhysicsSystem->Exit();
        Messaging::Quit();
        ECS::Quit();
        Scene::Quit();
        Audio::Quit();
        GameLogic::GetInstance()->Exit();
        PE_Logger::D_Logger::GetInstance()->Exit();
    }
}