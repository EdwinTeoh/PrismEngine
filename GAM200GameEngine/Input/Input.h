/*******************************************************************************
filename    Input.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include <string>
#include <array>

#define p_Input Engine::InputSystem::GetInstance()

namespace Engine
{
    class InputSystem
    {
        static InputSystem* _instance;

    public:
        static InputSystem* GetInstance();

        void Init();
        void Update();
        void Exit();

        static std::array<bool, 512> _keysPressed, _keysHeld, _keysReleased;
        static std::array<bool, 8> _mouseButtonPressed, _mouseButtonHeld, _mouseButtonReleased;
        static int _mouseScrollStatus;

        bool IsKeyValid(const int& key);

        bool KeyIsPressed(const int& key);
        bool KeyIsHeld(const int& key);
        bool KeyIsReleased(const int& key);

        void SetKeyPressed(const int& key);
        void SetKeyHeld(const int& key);
        void SetKeyReleased(const int& key);

        bool MouseButtonIsPressed(const int& button);
        bool MouseButtonIsHeld(const int& button);
        bool MouseButtonIsReleased(const int& button);

        void SetMouseButtonPressed(const int& button);
        void SetMouseButtonHeld(const int& button);
        void SetMouseButtonReleased(const int& button);

        void ResetPressedKeys();

        void PrintKeyAction(const int& key, const std::string& action);
    };
}