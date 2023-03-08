/*******************************************************************************
filename    PE_Editor.cpp
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        October 19, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "PE_Editor.h"

#include "../Animation/Animation.h"
#include "../Clock/Clock.h"
#include "../ECS/Components.h"
#include "../GameLogic/GameLogic.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Picker.h"
#include "../Graphics/GraphicsSys.h"
#include "../Logger/PE_Logger.h"
#include "../Messaging/Messaging.h"
#include "../Window/Window.h"
#include "../UI/PE_UI.h"
#include "../AI/AISystem.h"
#include "../Resource/AssetManager.h"
#include "../Editor/PE_Editor_API.h"
#include <ImGuizmo.h>

#define clocky Utility::Clock::GetInstance()

#if 0
#define DOCKSPACE_TOP       0
#define DOCKSPACE_BOTTOM    0
#define DOCKSPACE_LEFT      0
#define DOCKSPACE_RIGHT     0
#else
#define DOCKSPACE_TOP       38
#define DOCKSPACE_BOTTOM    234
#define DOCKSPACE_LEFT      240
#define DOCKSPACE_RIGHT     240
#endif

namespace Engine
{
    PE_Editor* PE_Editor::_instance;
    ImGuiViewport* PE_Editor::_viewport;

    EntityID PE_Editor::spriteID;

    Graphics::Camera* PE_Editor::scene_camera;
    std::array<std::pair<GLuint, GLuint>, 2> PE_Editor::fbo_array;

    std::vector<PE_Editor::TranslateAction> PE_Editor::_undoTranslateActions, PE_Editor::_redoTranslateActions;

    PE_Editor* PE_Editor::GetInstance()
    {
        if (!_instance)
        {
            _instance = new PE_Editor;
        }

        return _instance;
    }

    bool CreateFrameBufferObject()
    {
        for (auto& [fbuf, tex] : PE_Editor::fbo_array)
        {
            // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
            glGenFramebuffers(1, &fbuf);
            glBindFramebuffer(GL_FRAMEBUFFER, fbuf);

            // The texture we're going to render to
            //GLuint renderedTexture;
            glGenTextures(1, &tex);

            // "Bind" the newly created texture : all future texture functions will modify this texture
            glBindTexture(GL_TEXTURE_2D, tex);

            // Give an empty image to OpenGL ( the last "0" )
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::_initialWidth, Window::_initialHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

            // Poor filtering. Needed !
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Set "renderedTexture" as our colour attachement #0
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);

            // Set the list of draw buffers.
            GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

            // Always check that our framebuffer is ok
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                return false;
            }

            // Render to the screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        return true;
    }

    void PE_Editor::Init()
    {
        scene_camera = new Graphics::Camera();
        scene_camera->Init();
        scene_camera->SetZoom(-32.0f);

        _sceneRuntime = 0.0f;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsClassic();

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato-Regular.ttf", 18.0f);
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOpenGL(Window::_pWindow, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        _viewport = ImGui::GetMainViewport();

        _toggleVSync = true;

        std::string path = "Assets/SceneData";
        int index = 0;
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (!entry.is_regular_file())
                continue;

            _fileList.push_back(entry);
            _filenameList.push_back(entry.path().filename().string());
            _filenameMap.emplace(entry.path().filename().string(), index++);
        }

        if (_editorOn)
        {
            _isPaused = true;
            CreateFrameBufferObject();
        }
        else
        {
            _isPaused = false;

            //const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            //Window::_currentWidth = mode->width;
            //Window::_currentHeight = mode->height;
            //glfwSetWindowMonitor(
            //    Window::_pWindow, Window::_primaryMonitor,
            //    0, 0,
            //    Window::_currentWidth, Window::_currentHeight, 0);
        }
    }

    void PE_Editor::Update()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(Window::_initialWidth), static_cast<float>(Window::_initialHeight));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!_editorOn)
        {
            return;
        }

        // Wrap in functions later
        ShowPlayBar();
        if (_isPaused || (!_isPaused && _showEditor))
        {
            //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
            CreateWindowWithDockspace({ 0, DOCKSPACE_TOP }, { _viewport->GetWorkSize().x, _viewport->GetWorkSize().y - DOCKSPACE_TOP }, "##DockWindow", "##DockSpace");
            //ShowDockspace();
            if (!_isPaused)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
            }
            else
            {
                ShowSceneList();
            }
            //ShowAnimationDebug();
            ShowCameraWaypointsWindow();
            ShowProfiler();
            ShowImagesFolder();
            ShowCameraInfo();
            ShowInspector();
            ShowHierarchy();
            ShowAssetBrowser();
            if (!_isPaused)
            {
                ImGui::PopStyleVar();
            }
            ShowGameWindow();
            ShowSceneWindow();
            //ImGui::PopStyleVar();

            //ShowScriptDebug();

            ShowConsole();
        }

        //static bool show = true;
        //ImGui::ShowDemoWindow(&show);
    }

    void PE_Editor::Draw()
    {
        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void PE_Editor::CleanUp()
    {
        delete _instance;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    float PE_Editor::SceneMinX()
    {
        return static_cast<float>(DOCKSPACE_LEFT);
    }

    float PE_Editor::SceneMaxX()
    {
        return static_cast<float>(Window::_currentWidth - (DOCKSPACE_RIGHT));
    }

    float PE_Editor::SceneMinY()
    {
        return static_cast<float>(DOCKSPACE_TOP);
    }

    float PE_Editor::SceneMaxY()
    {
        return static_cast<float>(Window::_currentHeight - DOCKSPACE_BOTTOM);
    }

    float PE_Editor::GetSceneWidth()
    {
        return SceneMaxX() - SceneMinX();
    }

    float PE_Editor::GetSceneHeight()
    {
        return SceneMaxY() - SceneMinY();
    }

    Vec2D PE_Editor::GetSceneOffset()
    {
        return Vec2D(DOCKSPACE_LEFT - DOCKSPACE_RIGHT, DOCKSPACE_BOTTOM - DOCKSPACE_TOP);
    }

    void PE_Editor::RestartGame()
    {
        ReloadSceneAndPause(Scene::SceneManager::sceneManager->GetCurrentSceneFilename());
        _isPaused = false;
        GameLogic::GetInstance()->_gameIsPaused = false;
    }

/******************************************************************************/

    void PE_Editor::ShowDockspace()
    {
        CreateWindowWithDockspace(
            { 0, DOCKSPACE_TOP },
            { DOCKSPACE_LEFT, _viewport->GetWorkSize().y - DOCKSPACE_TOP },
            "##window_left", "##dockspace_left");
        CreateWindowWithDockspace(
            { _viewport->GetWorkSize().x - DOCKSPACE_RIGHT, DOCKSPACE_TOP },
            { DOCKSPACE_RIGHT, _viewport->GetWorkSize().y - DOCKSPACE_TOP },
            "##window_right", "##dockspace_right");
        CreateWindowWithDockspace(
            { DOCKSPACE_LEFT, _viewport->GetWorkSize().y - DOCKSPACE_BOTTOM },
            { _viewport->GetWorkSize().x - DOCKSPACE_LEFT - DOCKSPACE_RIGHT, DOCKSPACE_BOTTOM },
            "##window_bottom", "##dockspace_bottom");
    }

    /*  Comment here
        bool* p_open | Whether or not the window can be closed
    */
    void PE_Editor::ShowPlayBar(bool* p_open)
    {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(_viewport->GetWorkSize().x, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking;

        ImVec4 window_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        _useFBO = true;
        if (!_isPaused && !_showEditor)
        {
            window_color = ImVec4();
            _useFBO = false;
        }
        ImGui::PushStyleColor(ImGuiCol_WindowBg, window_color);
        ImGui::Begin("##playbar", p_open, window_flags);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        float currentY = ImGui::GetCursorPosY();
        //if (_isPaused && _showEditor)
        //{
            //ImGui::SetWindowFontScale(1.5f);
            //ImGui::Text("THE ENGINE");
            //ImGui::SetWindowFontScale(1.0f);
        //}

        ImVec2 buttonSize(100.0f, 20.0f);
        ImGui::SetCursorPos(ImVec2(_viewport->GetWorkSize().x / 2 - buttonSize.x / 2, currentY));
        if (PlayPauseStopButton(0.0f) && _isPaused) // Play
        {
            GameLogic::GetInstance()->_gameIsPaused = false;
            _isPaused = false;

            Messaging::AudioMessage message(Messaging::UNPAUSE_ALL_AUDIO, _NONE,
                "", false, 0.7f, false, false, false, std::string(""));
            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
        }
        ImGui::SameLine();
        if (PlayPauseStopButton(1.0f) && !_isPaused) // Pause
        {
            _isPaused = true;

            Messaging::AudioMessage message(Messaging::PAUSE_ALL_AUDIO, _NONE,
                "", false, 0.7f, false, false, false, std::string(""));
            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
        }
        ImGui::SameLine();
        if (PlayPauseStopButton(2.0f)) // Stop
        {
            p_Particle->ClearAll();
            Messaging::AudioMessage message2(Messaging::STOP_ALL_AUDIO, _NONE,
                "", false, 0.7f, false, false, false, std::string(""));
            Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message2);
            //ReloadSceneAndPause(_filenameList[_selectedSaveFile]);
            ReloadSceneAndPause(Scene::SceneManager::sceneManager->GetCurrentSceneFilename());
            _selectedSaveFile = _filenameMap.at(Scene::SceneManager::sceneManager->GetCurrentSceneFilename());
        }

        if (_isPaused)
        {
            ImGui::SameLine();
            static bool maxOnPlay = false;
            ImGui::Checkbox(" Maximise On Play", &maxOnPlay);
            _showEditor = !maxOnPlay;

            static size_t selectedCombo = 0;

            ImGui::SameLine(Window::_currentWidth - 220.0f);
            ImGui::SetNextItemWidth(180.0f);
            if (ImGui::BeginCombo("##WindowMode", _windowModes[selectedCombo]))
            {

                // Borderless Windowed
                if (selectedCombo != 0 && ImGui::Selectable(_windowModes[0]))
                {
                    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    Window::_currentWidth = Window::_initialWidth;
                    Window::_currentHeight = Window::_initialHeight;
                    glfwSetWindowMonitor(
                        Window::_pWindow, nullptr,
                        mode->width / 2 - Window::_initialWidth / 2, mode->height / 2 - Window::_initialHeight / 2,
                        Window::_initialWidth, Window::_initialHeight, 0);
                    selectedCombo = 0;
                }

                // Borderless Fullscreen
                if (selectedCombo != 1 && ImGui::Selectable(_windowModes[1]))
                {
                    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    Window::_currentWidth = mode->width;
                    Window::_currentHeight = mode->height;
                    glfwSetWindowMonitor(
                        Window::_pWindow, nullptr,
                        0, 0,
                        Window::_currentWidth, Window::_currentHeight, 0);
                    selectedCombo = 1;
                }

                // Fullscreen
                if (selectedCombo != 2 && ImGui::Selectable(_windowModes[2]))
                {
                    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    Window::_currentWidth = mode->width;
                    Window::_currentHeight = mode->height;
                    glfwSetWindowMonitor(
                        Window::_pWindow, Window::_primaryMonitor,
                        0, 0,
                        Window::_currentWidth, Window::_currentHeight, 0);
                    selectedCombo = 2;
                }

                p_Editor->_windowMode = selectedCombo;

                ImGui::EndCombo();
            }
        }

        if (_isPaused)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
            ImGui::SetCursorPos(ImVec2(_viewport->GetWorkSize().x - 30.0f, currentY));
            if (ImGui::Button("X", ImVec2(ImGui::GetItemRectSize().y, ImGui::GetItemRectSize().y)))
            {
                glfwSetWindowShouldClose(Window::_pWindow, GLFW_TRUE);
            }
            ImGui::PopStyleColor();
        }

        ImGui::End();
    }

    void PE_Editor::ShowProfiler()
    {
        ImGui::Begin("Profiler");

        static bool _showColliders = true;
        ImGui::Checkbox("Show Colliders", &_showColliders);
        Window::_showDebugDrawing = _showColliders;

        // Does the update in intervals
        if (PE_Editor::_updateTimer > 0.0f)
        {
            PE_Editor::_updateTimer -= clocky.GetGDT();
        }
        else
        {
            PE_Editor::_updateTimer = 0.3f;

            PE_Editor::_framerate = clocky._fps;
            _systemsPercentages[0] = _isPaused ? 0.0f : clocky.GetDT(Utility::SystemCode::PHYSICS) / clocky.GetGDT();
            _systemsPercentages[1] = _isPaused ? 0.0f : clocky.GetDT(Utility::SystemCode::COLLISION) / clocky.GetGDT();
            _systemsPercentages[2] = _isPaused ? 0.0f : clocky.GetDT(Utility::SystemCode::GRAPHICS) / clocky.GetGDT();
            _systemsPercentages[3] = _isPaused ? 0.0f : clocky.GetDT(Utility::SystemCode::AI) / clocky.GetGDT();
            _systemsPercentages[4] = _isPaused ? 0.0f : clocky.GetDT(Utility::SystemCode::LOGIC) / clocky.GetGDT();
        }

        if (!_isPaused)
        {
            _sceneRuntime += clocky.GetGDT();
        }

        ImGui::Text("Scene Runtime: %.2f", _sceneRuntime);
        ImGui::Text("Framerate: %.2f", PE_Editor::_framerate);
        ImGui::SameLine(140);
        ImGui::Checkbox(" VSync", &p_Editor->_toggleVSync);
        Window::ToggleVSync(PE_Editor::_toggleVSync);

        ImGui::Text("Physics");
        ImGui::SameLine(80);
        ImGui::ProgressBar(_systemsPercentages[0], ImVec2(-1.0f, 0.0f));
        ImGui::Text("Collision");
        ImGui::SameLine(80);
        ImGui::ProgressBar(_systemsPercentages[1], ImVec2(-1.0f, 0.0f));
        ImGui::Text("Graphics");
        ImGui::SameLine(80);
        ImGui::ProgressBar(_systemsPercentages[2], ImVec2(-1.0f, 0.0f));
        ImGui::Text("AI");
        ImGui::SameLine(80);
        ImGui::ProgressBar(_systemsPercentages[3], ImVec2(-1.0f, 0.0f));
        ImGui::Text("Logic");
        ImGui::SameLine(80);
        ImGui::ProgressBar(_systemsPercentages[4], ImVec2(-1.0f, 0.0f));
        ImGui::End();
    }

    void PE_Editor::ShowSceneList()
    {
#pragma region FILE_ARRAY
        /*
            This region contains the set up for getting the file names into a const char* array
            It's an ImGui limitation that it has to be a const char* array
            But I can't fix the size since the number of save files might differ
            This is my workaround for now
        */
        std::vector<const char*> filenames;
        const size_t arrSize = 100;
        const char* itemsArr[arrSize];

        for (auto& item : _filenameList)
        {
            filenames.push_back(item.c_str());
        }

        std::copy(filenames.begin(), filenames.end(), itemsArr);

        // This is to avoid any errors if you try to read the file name from non-existent slots
        for (size_t i = filenames.size(); i < arrSize; i++)
        {
            itemsArr[i] = "EMPTY";
        }
#pragma endregion FILE_ARRAY

        ImGui::Begin("Save / Load");

        float initialCursorY = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(initialCursorY + 2.0f);
        ImGui::Text("Scene");
        ImGui::SameLine();
        ImGui::SetCursorPosY(initialCursorY);

        int fileToLoad = _selectedSaveFile;
        ImGui::Combo("##filecombo", &fileToLoad, itemsArr, static_cast<int>(filenames.size()), static_cast<int>(filenames.size()));

        if (fileToLoad != _selectedSaveFile)
        {
            _selectedSaveFile = fileToLoad;
            ReloadSceneAndPause(_fileList[_selectedSaveFile].path().filename().string());
        }

        if (_selectedSaveFile < 0)
        {
            ImGui::End();
            return;
        }

        ImGui::Button("Save Changes");
        if (ImGui::IsItemClicked() || (p_Input->KeyIsHeld(GLFW_KEY_LEFT_CONTROL) && p_Input->KeyIsPressed(GLFW_KEY_S)))
        {
            using namespace Messaging;
            SerializerMessage message(FACTORIZE, "Assets/SceneData/" + Scene::SceneManager::sceneManager->GetCurrentSceneFilename());
            msgManager->GetSystem<SerializerMessage>().CreateMessage(message);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Ctrl + S");
            ImGui::EndTooltip();
        }

        //ReloadSceneAndPause(_fileList[_selectedSaveFile].path().filename().string());

        ImGui::End();
    }

    void PE_Editor::ShowHierarchy()
    {
        ImGui::Begin("Hierarchy");

#pragma region SCENE_INFO
        /*
        ImGui::BeginGroup();

        if (_isPaused)
        {
            ImGui::Button("SAVE");
            ImGui::SameLine();
            if (ImGui::IsItemClicked() || (p_Input->KeyIsHeld(GLFW_KEY_LEFT_CONTROL) && p_Input->KeyIsPressed(GLFW_KEY_S)))
            {
                using namespace Messaging;
                SerializerMessage message(FACTORIZE, "Assets/SceneData/" + Scene::SceneManager::sceneManager->GetCurrentSceneFilename());
                msgManager->GetSystem<SerializerMessage>().CreateMessage(message);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Ctrl + S");
                ImGui::EndTooltip();
            }
        }

        ImGui::Text(Scene::SceneManager::sceneManager->GetCurrentSceneName().c_str());

        ImGui::Separator();

        ImGui::EndGroup();
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMin(), NULL);
        */
#pragma endregion SCENE_INFO

#pragma region NEW_ENTITY
        ImGui::BeginGroup();
        
        static bool newEntityCreated = false;
        static int entitySelected = -1;
        entitySelected = _pickedObject < 0 ? -1 : _pickedObject;

        if (ImGui::Button("New Entity"))
        {
            EntityID e_id = ECS::entityManager->CreateNewEntity();
            TransformComponent t = ECS::Factory::make<TransformComponent>();
            ECS::entityManager->FindEntity(e_id).AddComponent(std::move(t));

            entitySelected = e_id;
            newEntityCreated = true;
        }
        static USHORT ui_id = 0;

        if (false)
        {
            if (ImGui::Button("New Platform Corner"))
            {
                EntityID e_id = ECS::entityManager->CreateNewEntity();
                TransformComponent t = ECS::Factory::make<TransformComponent>();
                t._scale = 2.5f;
                t._position = { p_Editor->_gameWindowMousePosition.x, p_Editor->_gameWindowMousePosition.y };
                t._z = 1.0f;
                ECS::entityManager->FindEntity(e_id).AddComponent(std::move(t));

                SpriteComponent s = ECS::Factory::make<SpriteComponent>();
                s._texPath = "Assets/Textures/shock-tiles.png";
                s._texName = "shock-tiles.png";
                ECS::entityManager->FindEntity(e_id).AddComponent(std::move(s));
            }

            if (ImGui::Button("New Platform Middle"))
            {
                EntityID e_id = ECS::entityManager->CreateNewEntity();
                TransformComponent t = ECS::Factory::make<TransformComponent>();
                t._scale = 2.5f;
                t._position = { p_Editor->_gameWindowMousePosition.x, p_Editor->_gameWindowMousePosition.y };
                t._z = 1.0f;
                ECS::entityManager->FindEntity(e_id).AddComponent(std::move(t));

                SpriteComponent s = ECS::Factory::make<SpriteComponent>();
                s._texPath = "Assets/Textures/platform-new-2.png";
                s._texName = "platform-new-2.png";
                s._isRepeating = true;
                ECS::entityManager->FindEntity(e_id).AddComponent(std::move(s));
            }
        }

        ImGui::Separator();

        //ImGui::SameLine();

        //if (ImGui::SmallButton("New UI Element"))
        //{
        //    UISystem::GetInstance()->CreateNewElement<Button>();
        //    _selectedUI = UISystem::GetInstance()->_canvas->GetElementsCountInt() - 1;
        //}
        //static int eCount = UISystem::GetInstance()->_canvas->GetElementsCountInt();
        //if (ImGui::BeginCombo("##uicombo", "New UI Element"))
        //{
        //    if (ImGui::Selectable("Button"))
        //    {
        //        UISystem::GetInstance()->CreateNewElement<Button>();
        //    }
        //    if (ImGui::Selectable("Text"))
        //    {
        //        UISystem::GetInstance()->CreateNewElement<Text>();
        //    }
        //    if (ImGui::Selectable("Image"))
        //    {
        //        UISystem::GetInstance()->CreateNewElement<Image>();
        //    }

        //    if (UISystem::GetInstance()->_canvas->GetElementsCountInt() > eCount)
        //    {
        //        _selectedUI = UISystem::GetInstance()->_canvas->GetElementsCountInt() - 1;
        //    }

        //    ImGui::EndCombo();
        //}
        //ImGui::Separator();

        //ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        //ImGui::TextWrapped("When an object is selected, press 'F' to focus on it.");
        //ImGui::PopStyleColor();
        //ImGui::Separator();

        ImGui::EndGroup();
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMin(), NULL);
#pragma endregion NEW_ENTITY

#pragma region ENTITY_LIST
        ImGui::BeginChild("##list");

        for (auto& e : ECS::entityManager->GetEntityList())
        {
            ImGui::Selectable(e.GetName().c_str(), e.GetID() == _selectedEntityID);
            if (ImGui::IsItemClicked())
            {
                _selectedEntityID = e.GetID();

                if (_pickedObject != _selectedEntityID)
                {
                    _pickedObject = _selectedEntityID;
                }
            }
        }

        if (_selectedEntityID != _pickedObject)
        {
            //std::cout << _pickedObject << " " << _selectedEntityID << " | ";
            _selectedEntityID = _pickedObject;
        }

        if (newEntityCreated)
        {
            ImGui::SetScrollHereY(1.0f);
            newEntityCreated = false;
        }

        ImGui::EndChild();

        //if (ImGui::IsItemClicked())
        //{
        //    entitySelected = -1;
        //    _pickedObject = -1;
        //}
#pragma endregion ENTITY_LIST

        ImGui::End();
    }

    void PE_Editor::ShowInspector()
    {
        ImGui::Begin("Inspector");

        if (!ECS::entityManager->EntityExist((EntityID)_selectedEntityID))
        {
            ImGui::End();
            return;
        }

        ECS::Entity& e = ECS::entityManager->FindEntity((EntityID)_selectedEntityID);

        std::string e_name = e.GetName();
        char str[128] = "";
        e_name.copy(str, e.GetName().length(), 0);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
        ImGui::Text("Name"); ImGui::SameLine(); ImGui::SetCursorPos(ImVec2(60, ImGui::GetCursorPosY() - 2.0f));
        if (ImGui::InputText("##name", str, IM_ARRAYSIZE(str)))
        {
            e.Rename(str);
        }
        char str2[128] = "";
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
        ImGui::Text("Tag"); ImGui::SameLine();
        e.GetTag().copy(str2, e.GetTag().length(), 0);
        ImGui::SetCursorPosX(60);
        if (ImGui::InputText("##tag", str2, IM_ARRAYSIZE(str2)))
        {
           e.SetTag(str2);
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
        ImGui::Separator();

        //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.4f, 0.6f, 1.0f));
        for (auto& c : e._attachedComponents)
        {
            spriteID = e.GetID();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
            if (c->GetComponentName() == "RectCollider")
            {
                if (ImGui::SmallButton("DELETE"))
                {
                    compManager->RemoveComponentFromEntity<RectColliderComponent>(e.GetID());
                    continue;
                }
            }
            c->ToEditor();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
            ImGui::Separator();
        }
        //ImGui::PopStyleColor();

        ImGui::NewLine();
        float widgetWidth = 130.0f;
        ImGui::SetNextItemWidth(widgetWidth);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - widgetWidth / 2);
        ImGuiComboFlags comboFlags = ImGuiComboFlags_NoArrowButton;
        if (ImGui::BeginCombo("##componentcombo", "Add New Component", comboFlags))
        {
            if (ImGui::Selectable("Transform"))
                e.AddComponent(ECS::Factory::make<TransformComponent>());
            if (ImGui::Selectable("Sprite"))
            {
                SpriteComponent sprite = ECS::Factory::make<SpriteComponent>();
                sprite._texPath = "Assets/Textures/default-sprite.png";
                sprite._texName = "default-sprite.png";
                e.AddComponent(std::move(sprite));
            }
            if (ImGui::Selectable("Rigidbody"))
                e.AddComponent(ECS::Factory::make<RigidBodyComponent>());
            if (ImGui::Selectable("RectCollider"))
                e.AddComponent(ECS::Factory::make<RectColliderComponent>());
            if (ImGui::Selectable("CircleCollider"))
                e.AddComponent(ECS::Factory::make<CircleColliderComponent>());
            if (ImGui::Selectable("LineCollider"))
                e.AddComponent(ECS::Factory::make<LineColliderComponent>());
            if (ImGui::Selectable("Patrol"))
                e.AddComponent(ECS::Factory::make<PatrolComponent>());
            if (ImGui::Selectable("Platform"))
                e.AddComponent(ECS::Factory::make<PlatformComponent>());
            if (ImGui::Selectable("Particle Emitter"))
                e.AddComponent(ECS::Factory::make<ParticleComponent>());
            if (ImGui::Selectable("Attack"))
                e.AddComponent(ECS::Factory::make<AttackComponent>());
            ImGui::EndCombo();
        }

        ImGui::End();
    }

    void PE_Editor::ShowAnimationDebug()
    {
        using anim = Animation::AnimationClip;

        ImGui::Begin("Animation Preview");

        //for (size_t i = 1; i <= 7; i++)
        //{
        //    ImGui::SetNextItemWidth(20.0f);
        //    if (ImGui::SmallButton(toStr(i).c_str()))
        //    {
        //    }

        //    if (i < 7)
        //    {
        //        ImGui::SameLine();
        //    }
        //}
        
        //for (size_t y = 0; y < 7; ++y)
        //{
        //    for (size_t x = 0; x < 8; ++x)
        //    {
        //        ImGui::SmallButton(toStr(x + x * y).c_str());

        //        if (x < 7)
        //        {
        //            ImGui::SameLine();
        //        }
        //    }
        //}

        Graphics::Texture* tex = AssetManager::getTexture("pirate.png");
        static int x = 0, y = 0;
        ImGui::SliderInt("X", &x, 0, 8);
        ImGui::SliderInt("Y", &y, 0, 7);
        ImGui::Image(
            (void*)(intptr_t)tex->_id,
            ImVec2(ImGui::GetContentRegionAvail().y / (float)(tex->_height) * (float)(tex->_width), ImGui::GetContentRegionAvail().y),
            ImVec2(0.0f, 1.0f / 7), ImVec2(1.0f / 8, 0.0f)
        );
        //static int clipToPlay = 0, previous = 0;
        //ImGui::SliderInt("##jollibee", &clipToPlay, 0, 2);
        //if (clipToPlay != previous)
        //{
        //    ChangeState_Mark((MarkState)clipToPlay);
        //}
        //anim clip = anim::_mClips[clipToPlay];
        //previous = clipToPlay;

        //ImGui::Text("Row Count: %d, Column Count: %d", clip._numOfSpritesRow, clip._numOfSpritesCol);
        //ImGui::Text("Current Frame: %d", clip._currentFrame);
        //ImGui::Text("Offset X: %.3f", clip.GetCurrentFrameX());
        //ImGui::Text("Offset Y: %.3f", clip.GetCurrentFrameY());
        ImGui::End();
    }

    void PE_Editor::ShowConsole()
    {
        ImGui::Begin("My Console");
        ImGui::BeginChild("#checkboxes", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetItemRectSize().y));
        //ImGui::Checkbox("Export Log", &PE_Logger::D_Logger::GetInstance()->_doSaveLog);
        //if (ImGui::IsItemHovered())
        //{
        //    ImGui::SetTooltip("When ticked, a text file will be generated when the program is closed.");
        //}
        //ImGui::SameLine();
        ImGui::Checkbox("Auto Scroll", &_autoScroll);
        ImGui::EndChild();
        ImGui::Separator();
        ImGui::BeginChild("#console output");
        ImGui::TextUnformatted(PE_Logger::D_Logger::GetInstance()->GetLog_String().c_str());
        if (_autoScroll && !ImGui::IsWindowHovered())
        {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::End();
    }

    void PE_Editor::ShowCameraInfo()
    {
        ImGui::Begin("Camera");

        ImGui::Text("Position");
        glm::vec3 cameraPos = Graphics::Camera::GetInstance()->GetPosition();
        ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.0f); ImGui::DragFloat("##camerapositionx", &cameraPos.x);
        ImGui::SameLine();
        ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.0f); ImGui::DragFloat("##camerapositiony", &cameraPos.y);
        Graphics::Camera::GetInstance()->SetPosition(cameraPos.x, cameraPos.y);

        ImGui::End();
    }

void PE_Editor::ShowImagesFolder()
    {
        ImGui::Begin("Images");
        for (auto& [name, texObj] : AssetManager::TexturesCont)
        {
            unsigned texID = 0;
            if (texObj)
            {
                texID = texObj->_id;
            }
            glBindTexture(GL_TEXTURE_2D, texID);

            int width, height;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            glBindTexture(GL_TEXTURE_2D, 0);

            ImGui::Image((void*)(intptr_t)texID, ImVec2(64 * static_cast<float>(width) / static_cast<float>(height), 64), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                std::string str = name + " " + "(" + toStr(width) + "x" + toStr(height) + ")";
                ToEditor_Text(str);
                ImGui::EndTooltip();
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                ImGui::SetDragDropPayload("Waazaa", &name, sizeof(name));
                ImGui::Image((void*)(intptr_t)texID, ImVec2(64 * static_cast<float>(width) / static_cast<float>(height), 64), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
                ImGui::EndDragDropSource();
            }
        }
        ImGui::End();
    }

    void DoRecursiveTree(std::filesystem::directory_entry entry)
    {
        for (auto& e : std::filesystem::directory_iterator(entry.path()))
        {
            const std::string itemName = e.path().filename().string();
            if (e.is_regular_file())
            {
                ImGui::Selectable(itemName.c_str());
            }
            else if (ImGui::TreeNodeEx(itemName.c_str(), ImGuiTreeNodeFlags_Framed))
            {
                DoRecursiveTree(e);
                ImGui::TreePop();
            }
        }
    }

    void PE_Editor::ShowAssetBrowser()
    {
        if (!std::filesystem::exists("Assets"))
            return;

        ImGui::Begin("Asset Browser");

        std::filesystem::path assetsFolder("Assets");
        const char* currentlySelected = "Nothing selected.";

        //ImGui::BeginChild("Folders", ImVec2(200, ImGui::GetWindowHeight()));

        if (ImGui::TreeNodeEx("Assets", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
        {
            for (auto& e : std::filesystem::directory_iterator(assetsFolder))
            {
                const std::string itemName = e.path().filename().string();
                if (e.is_regular_file())
                {
                    if (ImGui::Selectable(itemName.c_str()))
                    {
                        currentlySelected = itemName.c_str();
                    }
                }
                else if (ImGui::TreeNodeEx(itemName.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    DoRecursiveTree(e);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
        //ImGui::EndChild();

        //ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
        //ImGui::SetCursorPos(ImVec2(210, 20));
        //ImGui::BeginChild("Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetWindowHeight()));
        //ImGui::Text(currentlySelected);
        //ImGui::PopStyleColor();
        //ImGui::EndChild();

        ImGui::End();
    }

    void PE_Editor::ShowScriptDebug()
    {
        using namespace ImGui;
        Begin("Script Debug Info");
        ToEditor_Text(("Count : " + toStr(p_GameLogic->GetScripts().size())).c_str());
        Separator();
        for (auto& s : p_GameLogic->GetScripts())
        {
            ToEditor_Text(("Script: " + s.first.second).c_str());
            ToEditor_Text(("ID: " + toStr(s.first.first)).c_str());
            bool exists = entityManager->EntityExist(s.first.first);
            std::string existsStr = exists ? "Yes" : "No";
            ToEditor_Text(("Exists: " + existsStr).c_str());
            if (exists)
            {
                ToEditor_Text(("Name: " + entityManager->FindEntity(s.first.first).GetName()).c_str());
            }
            Separator();
        }
        End();
    }

    void PE_Editor::ShowGameWindow()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
        ImGui::Begin("Game");

        _gameWindowFocused = ImGui::IsWindowFocused();

        static float tab_size = (ImGui::GetWindowSize().y - ImGui::GetContentRegionAvail().y) * 2;

        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 units(size.x / 16, size.y / 9);
        if ((size.y / units.x) > 9.0f)
            size.y = units.x * 9;
        else
            size.x = units.y * 16;

        ImGui::SetCursorPos({
            (ImGui::GetContentRegionAvail().x - size.x) / 2,
            (ImGui::GetContentRegionAvail().y - size.y + (tab_size - 2)) / 2
            });
        ImGui::Image(
            (void*)(intptr_t)fbo_array[1].second,
            size,
            ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)
        );

        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    void PE_Editor::ShowCameraWaypointsWindow()
    {
        ImGui::Begin("Camera Waypoints");
        if (Player::GetInstance()->cameraTriggerPoints.size() > 0)
        {
            static int page = 0;
            if (ImGui::Button("Back") && page > 0)
                --page;
            ImGui::SameLine();
            if (ImGui::Button("Next") && page < (Player::GetInstance()->cameraTriggerPoints.size() - 1))
                ++page;
            if (ImGui::Button("Add Point"))
                Player::GetInstance()->cameraTriggerPoints.push_back(Player::GetInstance()->cameraTriggerPoints.back());
            ImGui::Text(("Page" + std::to_string(page + 1) + "/" + std::to_string(Player::GetInstance()->cameraTriggerPoints.size())).c_str());
            auto& point = Player::GetInstance()->cameraTriggerPoints[page];
            ImGui::DragFloat("PX", &point._playerPosition._x);
            ImGui::DragFloat("PY", &point._playerPosition._y);;
            if (ImGui::Button("Update"))
            {
                TransformComponent pt = compManager->GetEntityComponent<TransformComponent>(0);
                point._playerPosition._x = floor(pt._position._x);
                point._playerPosition._y = floor(pt._position._y);
            }
            ImGui::DragFloat("CX", &point._cameraPosition._x);;
            ImGui::DragFloat("CY", &point._cameraPosition._y);
            ImGui::DragFloat("Zoom", &point._cameraZoom);
            if (ImGui::Button("Move"))
            {
                point._nextCameraPosition = point._cameraPosition;
                point._nextZoom = point._cameraZoom;
            }

            //if (p_Input->KeyIsPressed(GLFW_KEY_1))
            //{
            //    std::ofstream file("Camera Points.txt");
            //    for (auto& ctp : Player::GetInstance()->cameraTriggerPoints)
            //    {
            //        file << ctp._playerPosition._x << ", "
            //            << ctp._playerPosition._y << ", "
            //            << ctp._cameraPosition._x << ", "
            //            << ctp._cameraPosition._y << ", "
            //            << ctp._cameraZoom << std::endl;
            //    }
            //    file.close();
            //}
        }
        ImGui::End();
    }

    void PE_Editor::ShowSceneWindow()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
        ImGui::Begin("Scene");

        static float tab_size = (ImGui::GetWindowSize().y - ImGui::GetContentRegionAvail().y) * 2;

        ImVec2 position = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetContentRegionAvail();
        scene_camera->SetViewportSize({ size.x, size.y });
        ImVec2 offset{
            position.x - (Window::_currentWidth - position.x - size.x) + (ImGui::GetContentRegionAvail().x - size.x),
            position.y - (Window::_currentHeight - position.y - size.y) + (ImGui::GetContentRegionAvail().y - size.y) + tab_size
        };
        float ratio_x = Window::_currentWidth / size.x;
        float ratio_y = Window::_currentHeight / size.y;
        _gameWindowMousePosition = {
            (ImGui::GetMousePos().x - (Window::_currentWidth + offset.x) / 2) / Window::_currentWidth * 2 * ratio_x,
            -(ImGui::GetMousePos().y - (Window::_currentHeight + offset.y) / 2) / Window::_currentHeight * 2 * ratio_y
        };
        _gameWindowSpecs = {
            position.x + 0.5f * (ImGui::GetContentRegionAvail().x - size.x),
            position.y + 0.5f * (ImGui::GetContentRegionAvail().y - size.y + tab_size),
            size.x,
            size.y
        };

        ImGui::SetCursorPos({
            (ImGui::GetContentRegionAvail().x - size.x) / 2,
            (ImGui::GetContentRegionAvail().y - size.y + (tab_size - 2)) / 2
            });
        ImGui::Image(
            (void*)(intptr_t)fbo_array[0].second,
            ImGui::GetContentRegionAvail(),
            ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)
        );

        ShowImGuizmo(ImGui::IsWindowFocused());

        static bool pan = false;

        if (GetAsyncKeyState(GLFW_KEY_F) && _editorOn && _selectedEntityID > -1)
        {
            TransformComponent& t = ECS::compManager->GetEntityComponent<TransformComponent>((EntityID)_selectedEntityID);
            if (scene_camera->SetPositionSmooth(t))
                pan = true;
        }

        if (pan)
        {
            TransformComponent& t = ECS::compManager->GetEntityComponent<TransformComponent>((EntityID)_selectedEntityID);
            pan = scene_camera->SetPositionSmooth(t);
        }

        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    void PE_Editor::ShowImGuizmo(bool canPick)
    {
        using namespace Graphics;


        // Gizmo controls
        static unsigned int GizmoType = ImGuizmo::OPERATION::TRANSLATE;
        //if (p_Input->KeyIsPressed(GLFW_KEY_I))
        //    GizmoType = ImGuizmo::OPERATION::TRANSLATE;
        //if (p_Input->KeyIsPressed(GLFW_KEY_O))
        //    GizmoType = ImGuizmo::OPERATION::ROTATE;
        //if (p_Input->KeyIsPressed(GLFW_KEY_P))
        //    GizmoType = ImGuizmo::OPERATION::SCALE;

        static Vec2 startPosition, endPosition, zeroDelta, actualDelta;
        static int previousID = -1;

        ImGuizmo::BeginFrame();

        if (_pickedObject >= 0)
        {
            EntityID id = (EntityID)_pickedObject;

            if (!ECS::entityManager->EntityExist(id))
                return;

            ImGuizmo::SetOrthographic(true);

            ImGuizmo::SetRect(
                _gameWindowSpecs.x,
                _gameWindowSpecs.y,
                _gameWindowSpecs.z,
                _gameWindowSpecs.w
            );

            // Get camera information
            glm::mat4 cameraProj = scene_camera->GetProjMatrix();
            glm::mat4 cameraview = scene_camera->GetViewMatrix();

            // Get entity information to calculate mvp to position gizmo
            glm::mat4 transform{ 1.0f };
            ECS::Components::Transform& Trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);
            transform = glm::translate(transform, glm::vec3(Trans._position._x, Trans._position._y, 0.0f));
            transform = glm::rotate(transform, glm::radians(Trans._rotate), glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::scale(transform, glm::vec3(Trans._scale._x, Trans._scale._y, 0.0f));

            // NEEDED THIS TO DRAW INSIDE THE WINDOW
            ImGuizmo::SetDrawlist();

            // Draw the gizmo
            ImGuizmo::Manipulate(
                glm::value_ptr(cameraview),
                glm::value_ptr(cameraProj),
                (ImGuizmo::OPERATION)GizmoType,
                ImGuizmo::WORLD,
                glm::value_ptr(transform));

            // Calculate changes when the gizmo is activated and used
            if (ImGuizmo::IsUsing())
            {
                // Extract components from transform matrix
                float decomTranslate[3], decomRotate[3], decomScale[3];
                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), decomTranslate, decomRotate, decomScale);

                // Update rec collider and transform information based on gizmo
                auto& objTransform = ECS::compManager->GetEntityComponent<TransformComponent>(id);
                if (p_Input->MouseButtonIsPressed(0))
                {
                    previousID = id;
                    startPosition = objTransform._position;
                }
                //objTransform._scale = Vec2D(decomScale[0], decomScale[1]);
                if (p_Input->KeyIsHeld(GLFW_KEY_LEFT_SHIFT))
                    objTransform._position = Vec2D(floorf(4 * decomTranslate[0]) / 4, floorf(4 * decomTranslate[1]) / 4);
                else
                    objTransform._position = Vec2D(decomTranslate[0], decomTranslate[1]);
                //objTransform._rotate = decomRotate[2];
                endPosition = objTransform._position;
                //std::cout << ImGuizmo::deltaPosition.x << ImGuizmo::deltaPosition.y << std::endl;
            }
            if ((endPosition - startPosition) != zeroDelta && p_Input->MouseButtonIsReleased(0))
            {
                actualDelta = endPosition - startPosition;
                _redoTranslateActions.clear();
                _undoTranslateActions.push_back({ (EntityID)previousID, actualDelta });
                //std::cout << actualDelta << std::endl;
                startPosition = zeroDelta;
                endPosition = zeroDelta;
            }
        }

        if (canPick)
        {
            if (p_Input->KeyIsHeld(GLFW_KEY_LEFT_CONTROL))
            {
                if (p_Input->KeyIsPressed(GLFW_KEY_Z) && _undoTranslateActions.size())
                {
                    std::cout << "Undo" << std::endl;
                    TranslateAction action = _undoTranslateActions.back();
                    _redoTranslateActions.push_back(action);
                    auto& t = ECS::compManager->GetEntityComponent<TransformComponent>(action._id);
                    t._position -= action._delta;
                    _undoTranslateActions.pop_back();
                    previousID = -1;
                }
                else if (p_Input->KeyIsPressed(GLFW_KEY_Y) && _redoTranslateActions.size())
                {
                    std::cout << "Redo" << std::endl;
                    TranslateAction action = _redoTranslateActions.back();
                    _undoTranslateActions.push_back(action);
                    auto& t = ECS::compManager->GetEntityComponent<TransformComponent>(action._id);
                    t._position += action._delta;
                    _redoTranslateActions.pop_back();
                }
            }
        }

        // Picking selection
        // Will run if entity list is not empty AND mouse left button input detected
        if (canPick && !ECS::entityManager->GetEntityList().empty() && p_Input->MouseButtonIsPressed(GLFW_MOUSE_BUTTON_LEFT) && !ImGuizmo::IsOver())
        {
            // Get window coordinates
            double xpos, ypos;
            glfwGetCursorPos(Window::_pWindow, &xpos, &ypos);

            // [START] SORTING TRANSFORMS BY Z
            std::multimap<float, TransformComponent*> sortedMultimap;;
            for (auto& [id, sprite] : ECS::compManager->GetComponentList<TransformComponent>().GetList())
            {
                sortedMultimap.insert({ ECS::compManager->GetEntityComponent<TransformComponent>(id)._z, &sprite });
            }
            // [END] SORTING TRANSFORMS BY Z

            // Returns picked object to the editor's storage of the object's ID
            _pickedObject = Picker::Pick(scene_camera, sortedMultimap);
        }

        if (!canPick)
            return;

        if (_pickedObject > 0 && p_Input->KeyIsPressed(GLFW_KEY_BACKSLASH))
        {
            EntityID toDelete = (EntityID)_pickedObject;
            _pickedObject = -1;
            entityManager->DestroyEntity(toDelete);
            std::cout << "Deleted Entity " << toDelete << "!" << std::endl;
        }

        if (_pickedObject > 0 && p_Input->KeyIsPressed(GLFW_KEY_V))
        {
            EntityID e_id = ECS::entityManager->CreateNewEntity();
            ECS::entityManager->FindEntity(e_id).AddComponent(std::move(compManager->GetEntityComponent<TransformComponent>((EntityID)_pickedObject).clone()));
            ECS::entityManager->FindEntity(e_id).AddComponent(std::move(compManager->GetEntityComponent<SpriteComponent>((EntityID)_pickedObject).clone()));
        }

        if (_pickedObject > 0 && p_Input->KeyIsPressed(GLFW_KEY_B))
        {
            int how_many = (int)(compManager->GetEntityComponent<TransformComponent>((EntityID)_pickedObject)._scale._x / 2.5f);
            compManager->GetEntityComponent<SpriteComponent>((EntityID)_pickedObject)._u = 1.0f / how_many;
        }
    }

/******************************************************************************/

    bool PE_Editor::PlayPauseStopButton(float offset)
    {
        offset = std::clamp(offset, 0.0f, 2.0f);
        std::string newID = "PPSB" + std::to_string(offset);
        ImGui::PushID(newID.c_str());
        bool state = ImGui::ImageButton((void*)(intptr_t)AssetManager::getTexture("icons.png")->_id, ImVec2(18.0f, 18.0f),
            ImVec2(offset / 3.0f, 0.0f), ImVec2((offset + 1.0f) / 3.0f, 1.0f));
        ImGui::PopID();
        return state;
    }

    void PE_Editor::CreateWindowWithDockspace(ImVec2 position, ImVec2 size, const char* window_id, const char* dockspace_id, bool* p_open)
    {
        ImGui::SetNextWindowPos(ImVec2(position));
        ImGui::SetNextWindowSize(ImVec2(size));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        const ImVec4 window_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, window_color);
        ImGui::Begin(window_id, p_open, window_flags);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        const ImVec4 dockspace_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, dockspace_color);
        ImGuiID id = ImGui::GetID(dockspace_id);
        ImGui::DockSpace(id);
        ImGui::PopStyleColor();
        ImGui::End();
    }

    void PE_Editor::ReloadSceneAndPause(std::string filename)
    {
        _isPaused = true;
        _selectedEntityID = -1;

        //std::cout << Scene::SceneManager::sceneManager->GetCurrentSceneFilename() << " -> " << filename << std::endl;

        Scene::SceneManager::sceneManager->SetSceneStatus(true);
        Scene::SceneManager::sceneManager->ChangeScene(filename);

        _sceneRuntime = 0.0f;
    }
}