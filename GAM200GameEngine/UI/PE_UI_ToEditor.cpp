#include "PE_UI.h"

namespace Engine
{
    void Text::ToEditor()
    {
        ImGui::Checkbox("##enabled", &_enabled);
        ImGui::SameLine();
        float m_x = ImGui::GetCursorPosX();
        ImGui::Text(_name.c_str());
        ImGui::SetCursorPosX(m_x);
        ImGui::TextDisabled(_type.c_str());
        ImGui::Separator();

        ImGui::Text("Position");
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##posx", &_rectPos._x);
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##posy", &_rectPos._y);
        ImGui::Separator();

        std::string msg = _text;
        char str[128] = "";
        msg.copy(str, msg.length(), 0);
        ImGui::Text("Text");
        if (ImGui::InputText("##name", str, IM_ARRAYSIZE(str)))
        {
            _text = str;
        }

        ImGui::Text("Text Size");
        static int scale = (int)_fontScale;
        ImGui::DragInt("##fontSize", &scale);
        _fontScale = (float)scale;
    }

    void Panel::ToEditor()
    {
        ImGui::Checkbox("##enabled", &_enabled);
        ImGui::SameLine();
        float m_x = ImGui::GetCursorPosX();
        ImGui::Text(_name.c_str());
        ImGui::SetCursorPosX(m_x);
        ImGui::TextDisabled(_type.c_str());
        ImGui::Separator();
    }

    void Button::ToEditor()
    {
        ImGui::Checkbox("##enabled", &_enabled);
        ImGui::SameLine();
        float m_x = ImGui::GetCursorPosX();
        ImGui::Text(_name.c_str());
        ImGui::SetCursorPosX(m_x);
        ImGui::TextDisabled(_type.c_str());
        ImGui::Separator();

        ImGui::Text("Position");
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##btnposx", &_rectPos._x);
        //ImGui::InputFloat("##btnposx", &_rectPos._x, 0.0f, 0.0f, "%.2f");
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##btnposy", &_rectPos._y);
        ImGui::Separator();

        //ImGui::Checkbox("Use Images", &_imageButton);

        if (_imageButton)
        {
            ImGui::Text("Images");
            ImGui::TextDisabled("Click to change images");

            const std::string buttonNames[4]{ "Normal", "Hovered", "Pressed", "Disabled" };

            for (size_t i = 0; i < 4; ++i)
            {
                ImGui::Button(buttonNames[i].c_str());

                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("Waazaa", buttonNames[i].data(), sizeof(buttonNames[i]));
                    ImGui::Button(buttonNames[i].c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Waazaa");
                    if (payload == nullptr)
                    {

                    }
                    ImGui::EndDragDropTarget();
                }
            }
        }
        else
        {
            ImGui::Text("Colors");
            ImGui::TextDisabled("Click color box to change color");
            ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip;
            ImGui::ColorEdit4("Normal", _buttonColors[BTN_NORMAL], flags);
            ImGui::ColorEdit4("Hovered", _buttonColors[BTN_HOVERED], flags);
            ImGui::ColorEdit4("Pressed", _buttonColors[BTN_PRESSED], flags);
            ImGui::ColorEdit4("Disabled", _buttonColors[BTN_DISABLED], flags);
        }
    }

    void Slider::ToEditor()
    {
        ImGui::Checkbox("##enabled", &_enabled);
        ImGui::SameLine();
        float m_x = ImGui::GetCursorPosX();
        ImGui::Text(_name.c_str());
        ImGui::SetCursorPosX(m_x);
        ImGui::TextDisabled(_type.c_str());
        ImGui::Separator();

        ImGui::Text("Position");
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##posx", &_rectPos._x);
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##posy", &_rectPos._y);
        ImGui::Separator();

        ImGui::Text("Colors");
        ImGui::TextDisabled("Click color box to change color");
        ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip;
        ImGui::ColorEdit4("Back", _color, flags);
        ImGui::ColorEdit4("Back", _fillColor, flags);
        ImGui::Separator();

        ImGui::SliderFloat("##value", &_currentValue, _minimumValue, _maximumValue);
    }

    void Image::ToEditor()
    {
        ImGui::Checkbox("##enabled", &_enabled);
        ImGui::SameLine();
        float m_x = ImGui::GetCursorPosX();
        ImGui::Text(_name.c_str());
        ImGui::SetCursorPosX(m_x);
        ImGui::TextDisabled(_type.c_str());
        ImGui::Separator();

        ImGui::Text("Position");
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##posx", &_rectPos._x);
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##posy", &_rectPos._y);
        ImGui::Separator();

        ImGui::Text("Size");
        ImGui::Text("X"); ImGui::SameLine();
        ImGui::DragFloat("##sizex", &_rectSize._x);
        ImGui::Text("Y"); ImGui::SameLine();
        ImGui::DragFloat("##sizey", &_rectSize._y);
        ImGui::Separator();
    }
}