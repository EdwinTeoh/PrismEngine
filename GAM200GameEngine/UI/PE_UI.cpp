/*******************************************************************************
filename    PE_UI.cpp
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        December 06, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "PE_UI.h"
#include "../Graphics/Camera.h"
#include "../GameLogic/GameLogic.h"
#include "../Resource/AssetManager.h"
#include <random>

#define toInt(x) static_cast<int>(x)

#define DEBUG_MOUSE_POS _DEBUG

namespace Engine
{
    Vec2D pixelFactor;

    UISystem* UISystem::s_instance;

    std::vector<UIElement*> UISystem::Canvas::_MainMenuUI;
    std::vector<UIElement*> UISystem::Canvas::_GameLevelUI;
    std::vector<UIElement*> UISystem::Canvas::_PauseMenuUI;

    bool UISystem::_askQuit;

    Panel* UISystem::blackPanel = nullptr;
    Text* UISystem::winText = nullptr;
    Text* UISystem::loseText = nullptr;
    Text* UISystem::nextText = nullptr;
    Text* UISystem::briefText = nullptr;

    UISystem* UISystem::GetInstance()
    {
        if (!s_instance)
        {
            s_instance = new UISystem;
        }

        return s_instance;
    }

    void UISystem::Init()
    {
        _textRenderer.Init("Assets/Fonts/anime-ace/animeace.ttf");

        pixelFactor = { 2.0f / Window::_initialWidth, 2.0f / Window::_initialHeight };

        _canvas = new Canvas();

        _askQuit = false;
        
        blackPanel = new Panel("Black Panel");
        blackPanel->_color[3] = { 1.0f };
        blackPanel->_enabled = false;

        winText = new Text("Win Text", { -250.0f, 150.0f }, 50.0f);
        winText->_text = "I saved Mark!";
        winText->_enabled = false;

        loseText = new Text("Lose Text", { -250.0f, 150.0f }, 50.0f);
        loseText->_text = "Oh no I died...";
        loseText->_enabled = false;

        briefText = new Text("Briefing", { -575.0f, 100.0f }, 35.0f);
        briefText->_text = "Where is everyone..? I have to find them..";
        briefText->_enabled = false;

        nextText = new Text("Next Level", { 250.0f, -325.0f }, 18.0f);
        nextText->_text = "Press Space To Proceed..";
        nextText->_enabled = false;

        Canvas::_MainMenuUI.push_back(new Image("main-menu-2.png", { 1280, 720 }));
        Canvas::_MainMenuUI.push_back(new Image("select.png", { 354 * 0.7f, 100 * 0.9f }, { 0.0f, -5.0f }));
        Canvas::_MainMenuUI.push_back(new Image("select.png", { 354 * 0.8f, 100 * 0.6f }, { 0.0f, -85.0f }));
        Canvas::_MainMenuUI.push_back(new Image("select.png", { 354 * 0.5f, 100 * 0.5f }, { 0.0f, -140.0f }));
        Canvas::_MainMenuUI.push_back(new Image("quit-button-white.png", { 298 * 0.9f, 122 * 0.9f }, { -310.0f, -280.0f }));
        Canvas::_MainMenuUI.push_back(new Image("quit-button.png", { 298 * 0.9f, 122 * 0.9f }, { -310.0f, -280.0f }));
        Canvas::_MainMenuUI.push_back(new Image("credits.png", { 1280, 720 }));
        Canvas::_MainMenuUI.push_back(new Image("panel.png", { 1280, 720 }));
        Canvas::_MainMenuUI.push_back(new Image("pause3.png", { 559, 645 }, { -100.0f, 0.0f }));
        Canvas::_MainMenuUI.push_back(new Image("pause4.png", { 559, 645 }, { -100.0f, 0.0f }));
        Canvas::_MainMenuUI.push_back(new Image("how-to-play-1.png", { 1280, 720 }));
        Canvas::_MainMenuUI.push_back(new Image("how-to-play-2.png", { 1280, 720 }));

        Canvas::_GameLevelUI.push_back(new Image("end-bad.png", { 1280, 720 }));
        Canvas::_GameLevelUI.push_back(new Image("end-good.png", { 1280, 720 }));
        Canvas::_GameLevelUI.push_back(new Image("hp-bar-back.png", { 744 * 0.4f, 210 * 0.4f }, { -470.0f, 300.0f }));
        for (size_t i = 0; i < 10; i++)
        {
            Canvas::_GameLevelUI.push_back(new Image("hp-bar-1.png", { 744 * 0.4f, 210 * 0.4f }, { -470.0f + i * 27.0f, 300.0f }));
            Canvas::_GameLevelUI.back()->SetRotation(90.0f);
        }

        Canvas::_PauseMenuUI.push_back(new Image("panel.png", { 1280, 720 }));
        Canvas::_PauseMenuUI.push_back(new Image("pause2.png", { 559, 645 }, { -100.0f, 0.0f }));
        Canvas::_PauseMenuUI.push_back(new Image("select2.png", { 354, 100 * 0.7f }, { -80.0f, 50.0f }));
        Canvas::_PauseMenuUI.push_back(new Image("select2.png", { 354 * 0.7f, 100 * 0.4f }, { -60.0f, -140.0f }));
        Canvas::_PauseMenuUI.push_back(new Image("hand.png", { 245, 357 }, { 215.0f, 220.0f }));
        Canvas::_PauseMenuUI.push_back(new Image("pause3.png", { 559, 645 }, { -100.0f, 0.0f }));
        Canvas::_PauseMenuUI.push_back(new Image("pause4.png", { 559, 645 }, { -100.0f, 0.0f }));
        Canvas::_PauseMenuUI.push_back(new Image("how-to-play-1.png", { 1280, 720 }));
        Canvas::_PauseMenuUI.push_back(new Image("how-to-play-2.png", { 1280, 720 }));

        _introImage = dynamic_cast<UIElement*>(new Image("Intro_0.png", { 1290, 730 }));
        _introImage->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
        _outroImage = dynamic_cast<UIElement*>(new Image("Outro_0.png", { 1290, 730 }));
        _outroImage->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
        _promptText = new Text("Press ESC to skip", { 400.0f, -340.0f }, 14.0f);
        dynamic_cast<Text*>(_promptText)->_text = "Press ENTER to skip";
    }

    void UISystem::Draw()
    {
        //if (p_Input->KeyIsPressed(GLFW_KEY_1))
        //    UISystem::SerializeToFile("UI_MainMenu.json");

        //if (p_Input->KeyIsPressed(GLFW_KEY_2))
        //    UISystem::DeserializeFromFile("UI_MainMenu.json");

        for (size_t i = 3; i < 3 + 10; ++i)
        {
            _canvas->_GameLevelUI[i]->_enabled = false;
        }

        if (Player::GetInstance()->_endState == 0)
        {
            for (size_t i = 3; i < 3 + (size_t)(Player::GetInstance()->_currHP / 10); ++i)
            {
                if (i >= _canvas->_GameLevelUI.size())
                    break;

                _canvas->_GameLevelUI[i]->_enabled = true;
            }
        }

        if (blackPanel->_enabled)
        {
            glBegin(GL_QUADS);
            glColor4fv(blackPanel->_color);

            float blackPanel_sizeX = blackPanel->GetRectSizeV()._x / p_Camera->GetViewportWidth();
            float blackPanel_posX = blackPanel->GetRectPosV()._x / p_Camera->GetViewportWidth() * 2.0f;
            float blackPanel_x1 = blackPanel_posX - blackPanel_sizeX;
            float blackPanel_x2 = blackPanel_posX + blackPanel_sizeX;

            float blackPanel_sizeY = blackPanel->GetRectSizeV()._y / p_Camera->GetViewportHeight();
            float blackPanel_posY = blackPanel->GetRectPosV()._y / p_Camera->GetViewportHeight() * 2.0f;
            float blackPanel_y1 = blackPanel_posY - blackPanel_sizeY;
            float blackPanel_y2 = blackPanel_posY + blackPanel_sizeY;

            glVertex2f(blackPanel_x1, blackPanel_y1);
            glVertex2f(blackPanel_x2, blackPanel_y1);
            glVertex2f(blackPanel_x2, blackPanel_y2);
            glVertex2f(blackPanel_x1, blackPanel_y2);

            glEnd();
        }

        if (winText->_enabled)
        {
            glm::vec2 pos{
                (winText->GetRectPosV()._x + 0.5f * p_Camera->GetViewportWidth()),
                (winText->GetRectPosV()._y + 0.5f * p_Camera->GetViewportHeight())
            };
            _textRenderer.RenderText(winText->_text, { pos.x, pos.y }, winText->_fontScale);
        }

        if (loseText->_enabled)
        {
            glm::vec2 pos{
                (loseText->GetRectPosV()._x + 0.5f * p_Camera->GetViewportWidth()),
                (loseText->GetRectPosV()._y + 0.5f * p_Camera->GetViewportHeight())
        };
            _textRenderer.RenderText(loseText->_text, { pos.x, pos.y }, loseText->_fontScale);
        }

        if (nextText->_enabled)
        {
            glm::vec2 pos{
                (nextText->GetRectPosV()._x + 0.5f * p_Camera->GetViewportWidth()),
                (nextText->GetRectPosV()._y + 0.5f * p_Camera->GetViewportHeight())
            };
            _textRenderer.RenderText(nextText->_text, { pos.x, pos.y }, nextText->_fontScale);
        }

        if (briefText->_enabled)
        {
            glm::vec2 pos{
                (briefText->GetRectPosV()._x + 0.5f * p_Camera->GetViewportWidth()),
                (briefText->GetRectPosV()._y + 0.5f * p_Camera->GetViewportHeight())
            };
            _textRenderer.RenderText(briefText->_text, { pos.x, pos.y }, briefText->_fontScale);
        }

#if DEBUG_MOUSE_POS
        if (p_Input->MouseButtonIsHeld(GLFW_MOUSE_BUTTON_1))
        {
            ImVec2 ndc = p_Editor->_gameWindowMousePosition;

            glBegin(GL_QUADS);
            glColor3f(0.0f, 1.0f, 1.0f);
            glVertex2f(ndc.x - 0.01f, ndc.y - 0.01f);
            glVertex2f(ndc.x + 0.01f, ndc.y - 0.01f);
            glVertex2f(ndc.x + 0.01f, ndc.y + 0.01f);
            glVertex2f(ndc.x - 0.01f, ndc.y + 0.01f);
            glEnd();
        }
#endif
    }

    void UISystem::EnableNext()
    {
        if (!(p_UISystem->blackPanel->_enabled && p_UISystem->nextText->_enabled && p_UISystem->briefText->_enabled))
        {
            p_UISystem->blackPanel->_enabled = true;
            p_UISystem->nextText->_enabled = true;
            p_UISystem->briefText->_enabled = true;
        }
        return;
    }

    void UISystem::DisableNext()
    {
        p_UISystem->blackPanel->_enabled = false;
        p_UISystem->nextText->_enabled = false;
        p_UISystem->briefText->_enabled = false;

        return;
        }

    void UISystem::Update()
    {
        if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Intro")
        {
            static float timings[22]{
                1.0f,
                1.0f,
                2.0f,
                2.0f,
                1.0f,
                3.0f,
                1.0f,
                0.5f,
                0.5f,
                0.5f,
                3.0f,
                3.0f,
                2.0f,
                2.0f,
                2.0f,
                2.0f,
                2.0f,
                2.0f,
                2.0f,
                2.0f,
                3.0f,
                1.0f
            };
            static std::array<std::pair<std::string, float>, 22> sounds{
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("Footsteps.ogg", 1.0f),
                std::make_pair<std::string, float>("thud.ogg", 0.25f),
                std::make_pair<std::string, float>("Footsteps.ogg", 1.0f),
                std::make_pair<std::string, float>("thud.ogg", 0.25f),
                std::make_pair<std::string, float>("Pop.ogg", 0.5f),
                std::make_pair<std::string, float>("Pop.ogg", 0.5f),
                std::make_pair<std::string, float>("Pop.ogg", 0.5f),
                std::make_pair<std::string, float>("Pop.ogg", 0.5f),
                std::make_pair<std::string, float>("question.ogg", 0.5f),
                std::make_pair<std::string, float>("alarm.mp3", 0.25f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("worry.ogg", 2.0f),
                std::make_pair<std::string, float>("Spotted.ogg", 0.5f),
                std::make_pair<std::string, float>("surprised.ogg", 0.25f),
                std::make_pair<std::string, float>("Footsteps.ogg", 1.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f)
            };
            // Intro is 0 to 20
            if (_introFrame == 20)
            {
                if (p_Input->KeyIsPressed(GLFW_KEY_ENTER))
                {
                    Scene::SceneManager::sceneManager->SetSceneStatus(true);
                    Scene::SceneManager::sceneManager->ChangeScene("Level-1.1.json");

                    _cutsceneFrameTimer = 0.0f;
                    _introFrame = 0;
                    _introImage->SetName("Intro_0.png");
                    _introImage->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
                    _promptText->SetRectPos({ 400.0f, -340.0f });
                    dynamic_cast<Text*>(_promptText)->_text = "Press ENTER to skip";

                    p_SoundManager->StopAllSFX();
                    Messaging::AudioMessage message4(Messaging::PLAY_LOOPSFX, _LEVEL,
                        "Level1.ogg", false, 1.5f, true, false, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message4);
                }

                _promptText->SetRectPos({ 340.0f, -340.0f });
                dynamic_cast<Text*>(_promptText)->_text = "Press ENTER to continue";

                _introImage->_color[0] = std::clamp(1.0f - _cutsceneFrameTimer, 0.0f, 1.0f);
                _introImage->_color[1] = std::clamp(1.0f - _cutsceneFrameTimer, 0.0f, 1.0f);
                _introImage->_color[2] = std::clamp(1.0f - _cutsceneFrameTimer, 0.0f, 1.0f);
            }
            else
            {
                if (p_Input->KeyIsPressed(GLFW_KEY_ENTER))
                {
                    p_SoundManager->StopAllSFX();
                    Messaging::AudioMessage message4(Messaging::PLAY_LOOPSFX, _LEVEL,
                        "Level1.ogg", false, 1.5f, true, false, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message4);
                    Scene::SceneManager::sceneManager->SetSceneStatus(true);
                    Scene::SceneManager::sceneManager->ChangeScene("Level-1.1.json");

                    _cutsceneFrameTimer = 0.0f;
                    _introFrame = 0;
                    _introImage->SetName("Intro_0.png");
                    _introImage->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
                    _promptText->SetRectPos({ 400.0f, -340.0f });
                    dynamic_cast<Text*>(_promptText)->_text = "Press ENTER to skip";

                    return;
                }
                if (_cutsceneFrameTimer > timings[_introFrame])
                {
                    if (_introFrame == 15)
                    {
                        p_SoundManager->StopAllSFX();
                    }
                    if (sounds[_introFrame].first != "NONE")
                    {
                        p_SoundManager->playSFX(sounds[_introFrame].first, false, sounds[_introFrame].second);
                    }
                    ++_introFrame;
                    if (_introFrame == 2 || _introFrame == 19)
                    {
                        p_SoundManager->playSFX("door.ogg", false, 0.5f);
                    }

                    _cutsceneFrameTimer = 0.0f;

                    std::string imageName = "Intro_" + toStr(_introFrame) + ".png";
                    dynamic_cast<Image*>(_introImage)->SetName(imageName);
                    dynamic_cast<Image*>(_introImage)->SetTextureID();
                }
                if (_introFrame == 0)
                {
                    p_SoundManager->StopAllBGM();
                    _introImage->_color[0] = _cutsceneFrameTimer;
                    _introImage->_color[1] = _cutsceneFrameTimer;
                    _introImage->_color[2] = _cutsceneFrameTimer;
                }
            }

            _cutsceneFrameTimer += p_Clock.GetGDT();

            return;
        }

        if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Outro")
        {
            // Outro is 0 to 25
            static float timings[27]{
                1.0f,
                3.0f,
                4.0f,
                3.0f,
                8.0f,
                2.0f,
                2.0f,
                4.0f,
                4.0f,
                4.0f,
                2.0f,
                2.0f,
                3.0f,
                3.0f,
                0.5f,
                0.5f,
                0.5f,
                0.5f,
                3.0f,
                1.0f,
                1.0f,
                1.0f,
                1.0f,
                2.0f,
                3.0f,
                3.0f,
                1.0f
            };
            static std::array<std::pair<std::string, float>, 27> sounds{
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("rope.ogg", 3.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("sparkly.ogg", 1.0f),
                std::make_pair<std::string, float>("punching.ogg", 1.0f),
                std::make_pair<std::string, float>("UIButton.ogg", 1.0f),
                std::make_pair<std::string, float>("whoosh.ogg", 1.0f),
                std::make_pair<std::string, float>("ding.ogg", 1.0f),
                std::make_pair<std::string, float>("praise.ogg", 1.0f),
                std::make_pair<std::string, float>("Footsteps.ogg", 0.5f),
                std::make_pair<std::string, float>("Footsteps.ogg", 0.5f),
                std::make_pair<std::string, float>("Footsteps.ogg", 0.5f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("question.ogg", 1.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("SClick.ogg", 1.0f),
                std::make_pair<std::string, float>("SClick.ogg", 1.0f),
                std::make_pair<std::string, float>("SClick.ogg", 1.0f),
                std::make_pair<std::string, float>("SClick.ogg", 1.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("Lose.ogg", 1.0f),
                std::make_pair<std::string, float>("NONE", 0.0f),
                std::make_pair<std::string, float>("NONE", 0.0f)
            };

            if (_outroFrame == 0)
            {
                p_SoundManager->StopAllSounds();
                _outroImage->_color[0] = _cutsceneFrameTimer;
                _outroImage->_color[1] = _cutsceneFrameTimer;
                _outroImage->_color[2] = _cutsceneFrameTimer;
            }
            else if (_outroFrame == 26)
            {
                if (p_Input->KeyIsPressed(GLFW_KEY_ENTER))
                {
                    p_SoundManager->StopAllSFX();
                    Scene::SceneManager::sceneManager->SetSceneStatus(true);
                    Scene::SceneManager::sceneManager->ChangeScene("Menu.json");

                    _cutsceneFrameTimer = 0.0f;
                    _outroFrame = 0;
                    _outroImage->SetName("Outro_0.png");
                    _outroImage->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
                    _promptText->SetRectPos({ 400.0f, -340.0f });
                    dynamic_cast<Text*>(_promptText)->_text = "Press ENTER to skip";

                    return;
                }

                _promptText->SetRectPos({ 340.0f, -340.0f });
                dynamic_cast<Text*>(_promptText)->_text = "Press ENTER to continue";

                _outroImage->_color[0] = std::clamp(1.0f - _cutsceneFrameTimer, 0.0f, 1.0f);
                _outroImage->_color[1] = std::clamp(1.0f - _cutsceneFrameTimer, 0.0f, 1.0f);
                _outroImage->_color[2] = std::clamp(1.0f - _cutsceneFrameTimer, 0.0f, 1.0f);
            }

            if (_cutsceneFrameTimer > timings[_outroFrame])
            {
                if (_outroFrame <= 25)
                {
                    std::string imageName = "Outro_" + toStr(_outroFrame) + ".png";
                    dynamic_cast<Image*>(_outroImage)->SetName(imageName);
                    dynamic_cast<Image*>(_outroImage)->SetTextureID();

                    ++_outroFrame;
                    _cutsceneFrameTimer = 0.0f;
                }

                if (sounds[_outroFrame].first != "NONE")
                {
                    p_SoundManager->playSFX(sounds[_outroFrame].first, false, sounds[_outroFrame].second);
                }
            }

            if (p_Input->KeyIsPressed(GLFW_KEY_ENTER))
            {
                p_SoundManager->StopAllSFX();
                Scene::SceneManager::sceneManager->SetSceneStatus(true);
                Scene::SceneManager::sceneManager->ChangeScene("Menu.json");

                _cutsceneFrameTimer = 0.0f;
                _outroFrame = 0;
                _outroImage->SetName("Outro_0.png");
                _outroImage->SetColor(0.0f, 0.0f, 0.0f, 1.0f);

                return;
            }

            _cutsceneFrameTimer += p_Clock.GetGDT();

            return;
        }

        static bool firstLoad = true;

        if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Menu")
        {
            if (firstLoad)
            {
                firstLoad = false;

                p_SoundManager->StopAllSounds();
                Messaging::AudioMessage message(Messaging::PLAY_LOOPSFX, _MAINMENU,
                    "MainMenu.ogg", false, 0.5f, true, false, false, std::string(""));
                Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
            }

            static USHORT creditsIndex = 6, selected = 1, selectedConfirmation = 8;
            static bool showCredits = false, showConfirmation = false, showHelp = false;

            for (size_t i = 1; i <= 4; ++i)
            {
                _canvas->_MainMenuUI[i]->_enabled = false;
            }

            if (showCredits)
            {
                _canvas->_MainMenuUI[creditsIndex]->_enabled = true;

                if (p_Input->KeyIsPressed(GLFW_KEY_ESCAPE))
                {
                    showCredits = false;
                }
            }
            else if (showHelp)
            {
                static float timer = 0.0f;
                static float show = false;

                _canvas->_MainMenuUI[creditsIndex + 4]->_enabled = true;

                if (timer < 3.0f)
                {
                    timer += p_Clock.GetGDT();

                    if (timer >= 3.0f)
                    {
                        timer = 0.0f;
                        show = !show;
                        _canvas->_MainMenuUI[creditsIndex + 5]->_enabled = show;
                    }
                }

                if (p_Input->KeyIsPressed(GLFW_KEY_ESCAPE))
                {
                    showHelp = false;
                }
            }
            else if (showConfirmation)
            {
                _canvas->_MainMenuUI[7]->_enabled = true;
                _canvas->_MainMenuUI[selectedConfirmation]->_enabled = true;

                if (selectedConfirmation == 9)
                {
                    if (p_Input->KeyIsPressed(GLFW_KEY_ENTER) && !p_Editor->_editorOn)
                    {
                        glfwSetWindowShouldClose(Window::_pWindow, GLFW_TRUE);
                    }
                }

                if (p_Input->KeyIsPressed(GLFW_KEY_ESCAPE) || (selectedConfirmation == 8 && p_Input->KeyIsPressed(GLFW_KEY_ENTER)))
                {
                    showConfirmation = false;
                }
                else if (selectedConfirmation != 9 && (p_Input->KeyIsPressed(GLFW_KEY_S) || p_Input->KeyIsPressed(GLFW_KEY_DOWN)))
                {
                    selectedConfirmation = 9;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }
                else if (selectedConfirmation != 8 && (p_Input->KeyIsPressed(GLFW_KEY_W) || p_Input->KeyIsPressed(GLFW_KEY_UP)))
                {
                    selectedConfirmation = 8;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }
            }
            else
            {
                for (size_t i = creditsIndex; i <= creditsIndex + 5; ++i)
                {
                    _canvas->_MainMenuUI[i]->_enabled = false;
                }

                static Vec2D defaultSizes[3]{
                    _canvas->_MainMenuUI[1]->GetRectSizeV(),
                    _canvas->_MainMenuUI[2]->GetRectSizeV(),
                    _canvas->_MainMenuUI[3]->GetRectSizeV()
                };
                static float speed = 0.25f, multiplier = 1.0f, alphaSpeed = 1.0f;

                if (p_Input->KeyIsPressed(GLFW_KEY_S) || p_Input->KeyIsPressed(GLFW_KEY_DOWN))
                {
                    selected = (selected == 4) ? 1 : ++selected;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }
                else if (p_Input->KeyIsPressed(GLFW_KEY_W) || p_Input->KeyIsPressed(GLFW_KEY_UP))
                {
                    selected = (selected == 1) ? 4 : --selected;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }

                multiplier += speed * p_Clock.GetGDT();
                if (multiplier <= 0.95f || multiplier >= 1.05f)
                {
                    speed *= -1;
                }
                if (selected != 4)
                {
                    _canvas->_MainMenuUI[selected]->SetRectSize(defaultSizes[selected - 1] * multiplier);
                }
                else
                {
                    static float alpha = 0.0f;
                    alpha += alphaSpeed * p_Clock.GetGDT();
                    if (alpha > 1.0f)
                    {
                        alphaSpeed *= -1;
                        alpha = 1.0f;
                    }
                    else if (alpha < 0.0f)
                    {
                        alphaSpeed *= -1;
                        alpha = 0.0f;
                    }
                    _canvas->_MainMenuUI[selected]->_color[3] = alpha;
                }
                _canvas->_MainMenuUI[selected]->_enabled = true;

                if (p_Input->KeyIsPressed(GLFW_KEY_ENTER))
                {
                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);

                    switch (selected)
                    {
                    case 1:
                        firstLoad = true;
                        p_SoundManager->StopAllSounds();
                        Scene::SceneManager::sceneManager->SetSceneStatus(true);
                        Scene::SceneManager::sceneManager->ChangeScene("Intro.json");
                        return;
                    case 2:
                        showHelp = true;
                        break;
                    case 3:
                        showCredits = true;
                        break;
                    case 4:
                        showConfirmation = true;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        if (GameLogic::_gameIsPaused)
        {
            static USHORT bigSelect = 2, smallSelect = 3, handIndex = 4, confirmation = 5,
                first = 0, last = 4, selected = 0, selectedConfirmation = 5;

            static float delay = 0.5f;
            delay -= 1.0f / 60.0f;

            static bool showingConfirmation = false, showHelp = false;
            _canvas->_PauseMenuUI[handIndex]->_enabled = true;
            _canvas->_PauseMenuUI[confirmation]->_enabled = false;
            _canvas->_PauseMenuUI[confirmation + 1]->_enabled = false;

            if (showingConfirmation)
            {
                _canvas->_PauseMenuUI[handIndex]->_enabled = false;
                _canvas->_PauseMenuUI[confirmation]->_enabled = true;
                if (selectedConfirmation == 6)
                {
                    if (p_Input->KeyIsPressed(GLFW_KEY_ENTER) && !p_Editor->_editorOn)
                    {
                        glfwSetWindowShouldClose(Window::_pWindow, GLFW_TRUE);
                    }

                    _canvas->_PauseMenuUI[confirmation + 1]->_enabled = true;
                }

                if (p_Input->KeyIsPressed(GLFW_KEY_ESCAPE) || (selectedConfirmation == 5 && p_Input->KeyIsPressed(GLFW_KEY_ENTER)))
                {
                    showingConfirmation = false;
                    delay = 0.5f;
                }
                else if (selectedConfirmation != 6 && (p_Input->KeyIsPressed(GLFW_KEY_S) || p_Input->KeyIsPressed(GLFW_KEY_DOWN)))
                {
                    selectedConfirmation = 6;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }
                else if (selectedConfirmation != 5 && (p_Input->KeyIsPressed(GLFW_KEY_W) || p_Input->KeyIsPressed(GLFW_KEY_UP)))
                {
                    selectedConfirmation = 5;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }
            }
            else if (showHelp)
            {
                static float timer = 0.0f;
                static float show = false;

                _canvas->_PauseMenuUI[7]->_enabled = true;

                if (timer < 3.0f)
                {
                    timer += p_Clock.GetGDT();

                    if (timer >= 3.0f)
                    {
                        timer = 0.0f;
                        show = !show;
                        _canvas->_PauseMenuUI[8]->_enabled = show;
                    }
                }

                if (p_Input->KeyIsPressed(GLFW_KEY_ESCAPE))
                {
                    showHelp = false;
                }
            }
            else
            {
                _canvas->_PauseMenuUI[7]->_enabled = false;
                _canvas->_PauseMenuUI[8]->_enabled = false;

                static Vec2D selectPositions[5]{
                    { -80.0f, 50.0f },
                    { -74.0f, -14.0f },
                    { -62.0f, -90.0f },
                    { -60.0f, -144.0f },
                    { -50.0f, -196.0f }
                };

                static Vec2D handPositions[5]{
                    { 220.0f, 230.0f },
                    { 220.0f, 160.0f },
                    { 160.0f, 80.0f },
                    { 190.0f, 30.0f },
                    { 160.0f, -20.0f }
                };

                if (delay < 0.0f && p_Input->KeyIsPressed(GLFW_KEY_ESCAPE))
                {
                    GameLogic::_gameIsPaused = false;
                    selected = 0;
                    delay = 0.5f;
                    _canvas->_PauseMenuUI[handIndex]->SetRectPos(handPositions[0]);
                }

                if ((p_Input->KeyIsPressed(GLFW_KEY_S) || p_Input->KeyIsPressed(GLFW_KEY_DOWN)))
                {
                    selected = (selected == last) ? first : ++selected;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }
                else if ((p_Input->KeyIsPressed(GLFW_KEY_W) || p_Input->KeyIsPressed(GLFW_KEY_UP)))
                {
                    selected = (selected == first) ? last : --selected;

                    Messaging::AudioMessage message(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message);
                }

                if (selected > 1)
                {
                    _canvas->_PauseMenuUI[smallSelect]->SetRectPos(selectPositions[selected]);
                    _canvas->_PauseMenuUI[bigSelect]->_enabled = false;
                    _canvas->_PauseMenuUI[smallSelect]->_enabled = true;
                }
                else
                {
                    _canvas->_PauseMenuUI[bigSelect]->SetRectPos(selectPositions[selected]);
                    _canvas->_PauseMenuUI[bigSelect]->_enabled = true;
                    _canvas->_PauseMenuUI[smallSelect]->_enabled = false;
                }

                Vec2D currentPosition = _canvas->_PauseMenuUI[handIndex]->GetRectPosV(); // Hand
                currentPosition = currentPosition + (handPositions[selected] - currentPosition) * p_Clock.GetGDT() * 10.0f;
                _canvas->_PauseMenuUI[handIndex]->SetRectPos(currentPosition);

                if (p_Input->KeyIsPressed(GLFW_KEY_ENTER))
                {
                    Messaging::AudioMessage message_b(Messaging::PLAY_SFX, _UIBUTTON,
                        "UIButton.ogg", false, 0.7f, false, true, false, std::string(""));
                    Messaging::msgManager->GetSystem<Messaging::AudioMessage>().CreateMessage(message_b);

                    switch (selected)
                    {
                    case 0: // Resume
                        selected = 0;
                        delay = 0.5f;
                        _canvas->_PauseMenuUI[handIndex]->SetRectPos(handPositions[0]);
                        GameLogic::_gameIsPaused = false;
                        return;
                    case 1: // Restart
                        selected = 0;
                        Scene::SceneManager::sceneManager->SetSceneStatus(true);
                        Scene::SceneManager::sceneManager->ChangeScene(Scene::SceneManager::sceneManager->GetCurrentSceneFilename());
                        GameLogic::_gameIsPaused = false;
                        return;
                    case 2: // How To Play
                        showHelp = true;
                        break;
                    case 3: // Main Menu
                        selected = 0;
                        Scene::SceneManager::sceneManager->SetSceneStatus(true);
                        Scene::SceneManager::sceneManager->ChangeScene("Menu.json");
                        GameLogic::_gameIsPaused = false;
                        return;
                    case 4: // Quit Game
                        showingConfirmation = true;
                        return;
                    default:
                        break;
                    }
                }
            }
        }
    }

    void UISystem::Exit()
    {
        for (auto& e : _canvas->_MainMenuUI)
        {
            delete e;
        }
        std::vector<UIElement*>().swap(_canvas->_MainMenuUI);

        for (auto& e : _canvas->_GameLevelUI)
        {
            delete e;
        }
        std::vector<UIElement*>().swap(_canvas->_GameLevelUI);

        for (auto& e : _canvas->_PauseMenuUI)
        {
            delete e;
        }
        std::vector<UIElement*>().swap(_canvas->_PauseMenuUI);

        delete _canvas;

        delete blackPanel;
        delete winText;
        delete loseText;

        delete s_instance;
    }

    const float UIElement::GetViewportWidth()
    {
        return (const float)p_Camera->GetViewportWidth();
    }

    const float UIElement::GetViewportHeight()
    {
        return (const float)p_Camera->GetViewportHeight();
    }

    const std::string UIElement::GenerateUID()
    {
        static const char n[]{
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
        static const char c[]{
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
            'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
            'u', 'v', 'w', 'x', 'y', 'z' };
        static std::default_random_engine dre(std::random_device{}());
        static std::uniform_int<int> u_n(0, sizeof(n) - 1);
        static std::uniform_int<int> u_c(0, sizeof(c) - 1);

        std::string s;
        s.push_back(c[u_c(dre)]);
        s.push_back(n[u_n(dre)]);
        s.push_back(c[u_c(dre)]);
        s.push_back(n[u_n(dre)]);
        s.push_back(c[u_c(dre)]);
        s.push_back(n[u_n(dre)]);
        return s;
    }

    const std::string UIElement::GenerateType()
    {
        std::string name = typeid(*this).name();
        size_t found = name.find_last_of("::") + 1;
        return name.substr(found);
    }

    void Image::SetTextureID()
    {
        _textureID = AssetManager::getTexture(_textureName)->_id;
    }
}