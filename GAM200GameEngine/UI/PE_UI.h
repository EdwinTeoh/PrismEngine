/*******************************************************************************
filename    PE_UI.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        December 06, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "../UI/UIElement.h"
#include "../System/Systems.h"
#include "../Graphics/FontControl.h"
#include "../Window/Window.h"

#define p_UISystem Engine::UISystem::GetInstance()

namespace Engine
{
    // Position is currently bottom left aligned
    class Text : public UIElement
    {
    public:
        std::string _text;
        float _fontScale;

        Text(const std::string name = "Text",
            const Vec2D pos = { 0.0f, 0.0f },
            const float scale = 40.0f) :
            _text{ "Your text here" }
        {
            _type = GenerateType();
            _name = name;
            _rectSize = { 0.0f, 0.0f };
            _rectPos = pos;

            _interactable = false;

            _fontScale = scale;
        }
        ~Text() = default;

        virtual void ToEditor() override;
        virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
        virtual void Deserialize(const rapidjson::Value& obj) override;
    };

    class Panel : public UIElement
    {
    public:
        Panel(const std::string name = "Panel",
            const Vec2D size = { static_cast<float>(Window::_initialWidth), static_cast<float>(Window::_currentHeight) },
            const Vec2D pos = { 0.0f, 0.0f })
        {
            _type = GenerateType();
            _name = name;
            _rectSize = size;
            _rectPos = pos;

            _color[0] = 0.0f;
            _color[1] = 0.0f;
            _color[2] = 0.0f;
            _color[3] = 0.5f;

            _interactable = false;
        }
        ~Panel() = default;

        virtual void ToEditor() override;
        virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
        virtual void Deserialize(const rapidjson::Value& obj) override;
    };

    class UISystem : ISystem
    {
        static UISystem* s_instance;
    public:
        void Init();
        void Draw();
        void Update();
        void Exit();

        static Panel* blackPanel;
        static Text* winText;
        static Text* loseText;
        static Text* nextText;
        static Text* briefText;

        void EnableNext();
        void DisableNext();

        void SerializeToFile(const char* filename);
        void DeserializeFromFile(const char* filename);

        static UISystem* GetInstance();
        static bool _askQuit;
        size_t _introFrame;
        size_t _outroFrame;
        std::vector<std::unique_ptr<UIElement>> _uielements;
        UIElement* _introImage;
        UIElement* _outroImage;
        UIElement* _promptText;
        float _cutsceneFrameTimer;
        Graphics::Font _textRenderer;

        class Canvas
        {
            //enum RenderMode { OVERLAY, CAMERA, WORLD };
            //RenderMode _currentRenderMode = OVERLAY;

            int _width, _height;
        public:
            static std::vector<UIElement*> _MainMenuUI;
            static std::vector<UIElement*> _GameLevelUI;
            static std::vector<UIElement*> _PauseMenuUI;

            Canvas() :
                _width{ Window::_initialWidth },
                _height{ Window::_initialHeight }
            {
                // Nothing
            }
            ~Canvas() = default;
        };

        Canvas* _canvas = nullptr;
        std::map <std::string, Canvas*> _canvasList;

        template<typename T>
        std::unique_ptr<UIElement>& CreateNewElement()
        {
            T* tt = new T();
            Canvas::_MainMenuUI.push_back(std::move(tt));

            std::unique_ptr<UIElement> t = std::make_unique<T>();
            _uielements.push_back(std::move(t));
            return _uielements[_uielements.size() - 1];
        }
    };

    class Button : public UIElement
    {
        float _buttonColors[4][4]{
            { 82.0f / 255.0f, 147.0f / 255.0f, 156.0f / 255.0f, 1.0f},
            { 132.0f / 255.0f, 189.0f / 255.0f, 196.0f / 255.0f, 1.0f},
            { 53.0f / 255.0f, 114.0f / 255.0f, 122.0f / 255.0f, 1.0f},
            {0.5f, 0.5f, 0.5f, 0.5f}
        };
    public:
        enum Mode { BTN_NORMAL, BTN_HOVERED, BTN_PRESSED, BTN_DISABLED };
        Mode _currentMode;

        bool _pressedThisFrame;
        bool _imageButton;

        Button(const std::string name = "Button",
            const Vec2D size = { 100.0f, 50.0f },
            const Vec2D pos = { 0.0f, 0.0f })
        {
            _type = GenerateType();
            _name = name;
            _rectSize = size;
            _rectPos = pos;
            *_color = *_buttonColors[BTN_NORMAL];

            _currentMode = BTN_NORMAL;
            _pressedThisFrame = false;

            _imageButton = false;
        }
        ~Button() = default;

        virtual void ToEditor() override;

        virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
        virtual void Deserialize(const rapidjson::Value& obj) override;

        virtual const float* GetColor() override { return _buttonColors[BTN_NORMAL]; }
        virtual const glm::vec4 GetColorGLM() override
        {
            return {
                _buttonColors[BTN_NORMAL][0],
                _buttonColors[BTN_NORMAL][1],
                _buttonColors[BTN_NORMAL][2],
                _buttonColors[BTN_NORMAL][3] };
        }
        float* GetColor(Mode mode) { return _buttonColors[mode]; }
        glm::vec4 GetColorGLM(Mode mode)
        {
            return {
                _buttonColors[mode][0],
                _buttonColors[mode][1],
                _buttonColors[mode][2],
                _buttonColors[mode][3] };
        }
        bool IsPressed() const { return _pressedThisFrame; }
    };

    class Slider : public UIElement
    {
    public:
        float _fillColor[4];
        float _minimumValue, _maximumValue;
        float _currentValue;

        Slider(const std::string name = "Slider",
            const Vec2D size = { 200.0f, 40.0f },
            const Vec2D pos = { 0.0f, 0.0f }) :
            _fillColor{ 0.0f, 1.0f, 0.0f, 1.0f },
            _minimumValue{ 0.0f },
            _maximumValue{ 10.0f },
            _currentValue{ _maximumValue }
        {
            _type = GenerateType();
            _name = name;
            _rectSize = size;
            _rectPos = pos;
        }
        ~Slider() = default;

        virtual void ToEditor() override;
    };

    class Image : public UIElement
    {
        std::string _textureName;
        GLuint _textureID;
        float _border;
    public:
        Image(const std::string name = "Image",
            const Vec2D size = { 200.0f, 200.0f },
            const Vec2D pos = { 0.0f, 0.0f },
            const std::string textureName = "default-sprite.png")
        {
            _type = GenerateType();
            _name = name;
            _rectSize = size;
            _rectPos = pos;

            _color[0] = 1.0f;
            _color[1] = 1.0f;
            _color[2] = 1.0f;
            _color[3] = 1.0f;

            _interactable = false;

            _textureName = textureName;
            _border = 0.0f;

            SetTextureID();
        }
        ~Image() = default;

        virtual void ToEditor() override;
        virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
        virtual void Deserialize(const rapidjson::Value& obj) override;

        const GLuint& GetTextureID() { return _textureID; }
        void SetTextureID();
    };
}