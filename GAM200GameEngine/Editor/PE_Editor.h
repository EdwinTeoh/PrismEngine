/*******************************************************************************
filename    PE_Editor.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        October 19, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "../framework.h"
#include "../System/Macro.h"
//#include "../Graphics/Object.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Camera.h"
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#define p_Editor Engine::PE_Editor::GetInstance()

namespace Engine
{
    class PE_Editor
    {
        static PE_Editor* _instance;
        static ImGuiViewport* _viewport;

        std::vector<std::filesystem::directory_entry> _fileList;
        std::vector<std::string> _filenameList;
        std::map<std::string, int> _filenameMap;
        std::array<float, 5> _systemsPercentages{ 0.0f, 0.0f, 0.0f, 0.0f };
        std::array<const char*, 3> _windowModes{"Borderless Windowed", "Borderless Fullscreen", "Fullscreen"};
        std::string _loadedSaveFile =  "NO SAVE LOADED";

        int _selectedSaveFile = -1;
        int _selectedEntityID = -1;
        float _framerate = 0.0f;
        float _updateTimer = 0.0f;
        float _sceneRuntime = 0.0f;
        bool _toggleVSync = true;
        bool _autoScroll = true;

        void ShowDockspace();
        void ShowPlayBar(bool* p_open = NULL);
        void ShowProfiler();
        void ShowSceneList();
        void ShowHierarchy();
        void ShowInspector();
        void ShowAnimationDebug();
        void ShowConsole();
        void ShowCameraInfo();
        void ShowImagesFolder();
        void ShowAssetBrowser();
        void ShowScriptDebug();
        void ShowSceneWindow();
        void ShowGameWindow();
        void ShowCameraWaypointsWindow();
        void ShowImGuizmo(bool canPick);

        // TEMPORARY
        bool PlayPauseStopButton(float offset);
        void CreateWindowWithDockspace(ImVec2 position, ImVec2 size, const char* window_id, const char* dockspace_id, bool* p_open = NULL);
        void ReloadSceneAndPause(std::string filename);
    public:
        ImVec2 _gameWindowMousePosition;
        ImVec4 _gameWindowSpecs;
        size_t _windowMode = 0;
        int _pickedObject = -1;
        bool _isPaused = true;
        bool _showEditor = true;
        bool _editorOn = true;
        bool _useFBO = true;
        bool _gameWindowFocused = false;

        float _deltaX, _deltaY;
        Graphics::Texture* _meter;
        static EntityID spriteID;

        PE_Editor() = default;
        ~PE_Editor() = default;

        static PE_Editor* GetInstance();

        bool EditorWindowsIsOn() { return _instance->_showEditor; }

        void Init();
        void Update();
        void Draw();
        void CleanUp();

        static float SceneMinX();
        static float SceneMaxX();
        static float SceneMinY();
        static float SceneMaxY();
        static float GetSceneWidth();
        static float GetSceneHeight();
        static Vec2D GetSceneOffset();

        void RestartGame();

        struct TranslateAction
        {
            EntityID _id;
            Vec2D _delta;
        };

        static std::vector<TranslateAction> _undoTranslateActions;
        static std::vector<TranslateAction> _redoTranslateActions;

        static Graphics::Camera* scene_camera;
        static std::array<std::pair<GLuint, GLuint>, 2> fbo_array;
    };
}