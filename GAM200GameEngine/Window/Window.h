/*******************************************************************************
filename    Window.h
author(s)   Leow Yumi, 440002919, yumi.leow@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Engine
{
    class Window
    {
        static bool _doDrag;
        static int _cursorX, _cursorY;
    public:
        Window() = default;
        ~Window() = default;

        static bool Init();
        static void CleanUp();

        // Standard callbacks
        static void Error_cb(int error, char const* description);
        static void Fbsize_cb(GLFWwindow* pWin, int w, int h);

        // IO callbacks
        static void Key_cb(GLFWwindow* pwin, int key, int scancode, int action, int mod);
        static void Mousebutton_cb(GLFWwindow* pwin, int button, int action, int mod);
        static void Mousescroll_cb(GLFWwindow* pwin, double xoffset, double yoffset);
        static void Mousepos_cb(GLFWwindow* pwin, double xpos, double ypos);

        static void ToggleVSync(bool& _status);

        static GLFWmonitor* _primaryMonitor;
        static GLFWwindow* _pWindow;
        static std::string _title;
        static GLint _initialWidth, _initialHeight;
        static GLint _currentWidth, _currentHeight;
        static bool _vsync;
        static bool _fullscreen;

        // Must change this, it shouldn't be here!
        static GLboolean _showDebugDrawing;
    };
}