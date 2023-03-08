/*******************************************************************************
filename    Input.cpp
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Input.h"

#include "../ECS/Components.h"
#include "../Window/Window.h"
#include "../Editor/PE_Editor.h"
#include "../GameLogic/LogicScripts/Player.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace Engine
{
    InputSystem* InputSystem::_instance;
    std::array<bool, 512> InputSystem::_keysPressed, InputSystem::_keysHeld, InputSystem::_keysReleased;
    std::array<bool, 8> InputSystem::_mouseButtonPressed, InputSystem::_mouseButtonHeld, InputSystem::_mouseButtonReleased;
    int InputSystem::_mouseScrollStatus;

    InputSystem* InputSystem::GetInstance()
    {
        if (!_instance)
        {
            _instance = new InputSystem;
        }

        return _instance;
    }

    void InputSystem::Init()
    {
    }

    void InputSystem::Update()
    {
        for (int i = 0; i < _keysPressed.size(); i++)
        {
            if (_keysPressed[i])
            {
                SetKeyHeld(i);
            }

            _keysPressed[i] = false;
        }

        for (int i = 0; i < _keysPressed.size(); i++)
        {
            _keysReleased[i] = false;
        }

        for (int i = 0; i < _mouseButtonPressed.size(); i++)
        {
            if (_mouseButtonPressed[i])
            {
                SetMouseButtonHeld(i);
            }
        }

        for (int i = 0; i < _mouseButtonPressed.size(); i++)
        {
            _mouseButtonReleased[i] = false;
        }

        _mouseScrollStatus = 0;

        //ResetPressedKeys();
    }

    void InputSystem::Exit()
    {
        delete _instance;
    }

    bool InputSystem::IsKeyValid(const int& key)
    {
        bool isKeyValid = (key < _keysPressed.size());
        assert(("The key is out of range!", isKeyValid));

        return isKeyValid;
    }

    bool InputSystem::KeyIsPressed(const int& key)
    {
        return _keysPressed[key];
    }

    bool InputSystem::KeyIsHeld(const int& key)
    {
        return _keysHeld[key];
    }

    bool InputSystem::KeyIsReleased(const int& key)
    {
        return _keysReleased[key];
    }

    void InputSystem::SetKeyPressed(const int& key)
    {
        if (!IsKeyValid(key)) { return; }

        _keysPressed[key] = true;
#ifndef NDEBUG
        //PrintKeyAction(key, "Pressed ");
#endif
    }

    void InputSystem::SetKeyHeld(const int& key)
    {
        if (!IsKeyValid(key)) { return; }

        //_keysPressed[key] = false;
        _keysHeld[key] = true;

#ifndef NDEBUG
        //PrintKeyAction(key, "Holding ");
#endif
    }

    void InputSystem::SetKeyReleased(const int& key)
    {
        if (!IsKeyValid(key)) { return; }

        _keysPressed[key] = false;
        _keysHeld[key] = false;
        _keysReleased[key] = true;
#ifndef NDEBUG
        //PrintKeyAction(key, "Released ");
#endif
    }

    bool InputSystem::MouseButtonIsPressed(const int& button)
    {
        return _mouseButtonPressed[button];
    }

    bool InputSystem::MouseButtonIsHeld(const int& button)
    {
        return _mouseButtonHeld[button];
    }

    bool InputSystem::MouseButtonIsReleased(const int& button)
    {
        return _mouseButtonReleased[button];
    }

    void InputSystem::SetMouseButtonPressed(const int& button)
    {
        _mouseButtonPressed[button] = true;
    }

    void InputSystem::SetMouseButtonHeld(const int& button)
    {
        _mouseButtonPressed[button] = false;
        _mouseButtonHeld[button] = true;
    }

    void InputSystem::SetMouseButtonReleased(const int& button)
    {
        _mouseButtonPressed[button] = false;
        _mouseButtonHeld[button] = false;
        _mouseButtonReleased[button] = true;
    }

    void InputSystem::ResetPressedKeys()
    {
        for (size_t i = 0; i < _keysPressed.size(); i++)
        {
            _keysPressed[i] = false;
        }
    }

    void InputSystem::PrintKeyAction(const int& key, const std::string& action)
    {
        std::stringstream strstr;
        strstr << action;

        switch (key)
        {
        case GLFW_KEY_SPACE:
            strstr << "SPACE";
            break;
        case GLFW_KEY_ESCAPE:
            strstr << "ESC";
            break;
        case GLFW_KEY_ENTER:
            strstr << "ENTER";
            break;
        case GLFW_KEY_LEFT_SHIFT:
            strstr << "LEFT SHIFT";
            break;
        case GLFW_KEY_LEFT_CONTROL:
            strstr << "LEFT CTRL";
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            strstr << "RIGHT SHIFT";
            break;
        case GLFW_KEY_RIGHT_CONTROL:
            strstr << "RIGHT CTRL";
            break;
        default:
            strstr << static_cast<char>(key);
            break;
        }

        std::cout << strstr.str() << std::endl;
    }

}