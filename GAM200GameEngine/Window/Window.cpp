/*******************************************************************************
filename    Window.cpp
author(s)   Leow Yumi, 440002919, yumi.leow@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "../Editor/PE_Editor.h"
#include "Window.h"
#include "../Input/Input.h"
#include "../Serializer/RapidJsonBase.h"
#include "../Scene/Scene.h"

namespace Engine
{
    int Window::_cursorX;
    int Window::_cursorY;
    bool Window::_doDrag;

    GLFWmonitor* Window::_primaryMonitor;
    GLFWwindow* Window::_pWindow;
    std::string Window::_title;
    GLint Window::_initialWidth, Window::_initialHeight;
    GLint Window::_currentWidth, Window::_currentHeight;

    GLboolean Window::_showDebugDrawing;
    bool Window::_vsync;
    bool Window::_fullscreen;

    bool Window::Init()
    {
        Serializer::ConfigDeserializer("config.json", _title, _initialWidth, _initialHeight, _vsync, p_Editor->_editorOn, Scene::SceneManager::initialScene);

        std::cout << "Initialising window..." << std::endl;
        //std::cout << "W: " << _initialWidth << "H: " << _initialHeight << std::endl;

        if (!glfwInit())
        {
            std::cout << "GLFW init has failed - abort program!!!" << std::endl;
            return false;
        }

        glfwSetErrorCallback(Error_cb);

        // Before asking GLFW to create an OpenGL context, we need to specify the minimum constraints
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        _primaryMonitor = glfwGetPrimaryMonitor();

        if (p_Editor->_editorOn)
        {
            _currentWidth = _initialWidth;
            _currentHeight = _initialHeight;
            _pWindow = glfwCreateWindow(_initialWidth, _initialHeight, _title.c_str(), NULL, NULL);
        }
        else
        {
            const GLFWvidmode* mode = glfwGetVideoMode(_primaryMonitor);
            _currentWidth = mode->width;
            _currentHeight = mode->height;
            _pWindow = glfwCreateWindow(mode->width, mode->height, _title.c_str(), _primaryMonitor, NULL);
        }

        if (_pWindow == nullptr)
        {
            std::cout << "No current context detected!" << std::endl;
            return false;
        }

        glfwSetWindowAttrib(_pWindow, GLFW_DECORATED, GLFW_FALSE);

        glfwMakeContextCurrent(_pWindow);

        glfwSetFramebufferSizeCallback(_pWindow, Fbsize_cb);
        glfwSetKeyCallback(_pWindow, Key_cb);
        glfwSetMouseButtonCallback(_pWindow, Mousebutton_cb);
        glfwSetCursorPosCallback(_pWindow, Mousepos_cb);
        glfwSetScrollCallback(_pWindow, Mousescroll_cb);
        glfwSetInputMode(_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        _showDebugDrawing = false;
        glfwSwapInterval(static_cast<int>(_vsync));

        return true;
    }

    void Window::CleanUp()
    {
        glfwTerminate();
    }

    void Window::ToggleVSync(bool& _status)
    {
        _vsync = _status;
        glfwSwapInterval(static_cast<int>(_vsync));
    }

    void Window::Error_cb(int error, char const* description)
    {
        (void)error;
        std::cerr << "GLFW error: " << description << std::endl;
    }

    void Window::Fbsize_cb(GLFWwindow* pWin, int w, int h)
    {
        (void)pWin, (void)w, (void)h;
    }

    void Window::Key_cb(GLFWwindow* pwin, int key, int scancode, int action, int mod)
    {
        void(pwin), void(scancode), void(mod);

        if (action == GLFW_PRESS)
        {
            InputSystem::GetInstance()->SetKeyPressed(key);
        }

        if (action == GLFW_RELEASE)
        {
            InputSystem::GetInstance()->SetKeyReleased(key);
        }
    }

    void Window::Mousebutton_cb(GLFWwindow* pwin, int button, int action, int mod)
    {
        (void)pwin, (void)mod;

        if (action == GLFW_PRESS)
        {
            InputSystem::GetInstance()->SetMouseButtonPressed(button);

            double x, y;
            glfwGetCursorPos(pwin, &x, &y);
            if (y < PE_Editor::SceneMinY())
            {
                _cursorX = static_cast<int>(floor(x));
                _cursorY = static_cast<int>(floor(y));
                _doDrag = true;
            }
        }
        else if (action != GLFW_RELEASE)
        {
        }

        if (action == GLFW_RELEASE)
        {
            InputSystem::GetInstance()->SetMouseButtonReleased(button);

            _doDrag = false;
        }
        else
        {
        }
    }

    void Window::Mousescroll_cb(GLFWwindow* pwin, double xoffset, double yoffset)
    {
        (void)pwin, (void)xoffset, (void)yoffset;
        InputSystem::GetInstance()->_mouseScrollStatus = static_cast<int>(yoffset);
    }

    void Window::Mousepos_cb(GLFWwindow* pwin, double xpos, double ypos)
    {
        (void)pwin;

        if (!_doDrag || p_Editor->_windowMode != 0)
        {
            return;
        }

        int w_x, w_y;
        glfwGetWindowPos(pwin, &w_x, &w_y);
        w_x -= _cursorX - static_cast<int>(xpos);
        w_y -= _cursorY - static_cast<int>(ypos);
        glfwSetWindowPos(pwin, w_x, w_y);
    }
}