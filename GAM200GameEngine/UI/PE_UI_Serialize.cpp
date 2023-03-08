/*******************************************************************************
filename    PE_UI_Serialize.cpp
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        January, 2021
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "PE_UI.h"

namespace Engine
{
    using namespace rapidjson;

    void UISystem::SerializeToFile(const char* filename)
    {
        auto begin_time = p_Clock.GetSysClockNow();

        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        writer.SetMaxDecimalPlaces(3);

        writer.StartObject();
        for (auto& e : s_instance->_uielements)
        {
            e->Serialize(writer);
            writer.EndObject();
        }
        writer.EndObject();

        std::string str = "Assets/SceneData/UI/";
        str.append(filename);
        std::ofstream ofs(str);
        ofs << buffer.GetString();
        ofs.close();

        std::cout << "Serialized [" << filename << "] in approximately " << (p_Clock.GetSysClockNow() - begin_time) << " seconds." << std::endl;
    }

    void UISystem::DeserializeFromFile(const char* filename)
    {
        auto begin_time = p_Clock.GetSysClockNow();

        std::string str = "Assets/SceneData/UI/";
        str.append(filename);
        std::ifstream ifs(str);
        IStreamWrapper isw(ifs);
        Document document;
        document.ParseStream(isw);

        if (document.MemberCount() <= 0)
        {
            std::cout << "[" << filename << "] has no members to deserialize." << std::endl;
            return;
        }

        std::vector<std::unique_ptr<UIElement>> elements;
        for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
        {
            Value& obj = document[itr->name];
            std::string type(obj.FindMember("Type")->value.GetString());

            std::unique_ptr<UIElement> u;
            
            if (type == "Button")
                u = std::make_unique<Button>();
            else if (type == "Text")
                u = std::make_unique<Text>();
            else if (type == "Image")
                u = std::make_unique<Image>();
            else
            {
                std::cout << "No deserialization option for this ["<< type << "]!" << std::endl;
                continue;
            }

            u->SetUID(itr->name.GetString());
            u->Deserialize(obj);

            elements.push_back(std::move(u));
        }

        _uielements.swap(elements);

        std::cout << "Deserialized [" << filename << "] in approximately " << (p_Clock.GetSysClockNow() - begin_time) << " seconds." << std::endl;
    }

    /// <summary>Writes Vec2 as an array</summary>
    void WriteVec2(PrettyWriter<StringBuffer>& writer, const char* key, Vec2& v)
    {
        writer.Key(key);

        writer.SetFormatOptions(PrettyFormatOptions::kFormatSingleLineArray);
        writer.StartArray();

        writer.Double(v._x);
        writer.Double(v._y);

        writer.EndArray();
        writer.SetFormatOptions(PrettyFormatOptions::kFormatDefault);
    }

    Vec2 ReadVec2(const rapidjson::Value& obj)
    {
        Value::ConstValueIterator itr = obj.GetArray().Begin();
        return { itr->GetFloat(), (++itr)->GetFloat() };
    }

    /// <summary>Writes float[4] as an array</summary>
    void WriteFloat4(PrettyWriter<StringBuffer>& writer, const char* key, float* f)
    {
        writer.Key(key);

        writer.SetFormatOptions(PrettyFormatOptions::kFormatSingleLineArray);
        writer.StartArray();

        writer.Double(f[0]);
        writer.Double(f[1]);
        writer.Double(f[2]);
        writer.Double(f[3]);

        writer.EndArray();
        writer.SetFormatOptions(PrettyFormatOptions::kFormatDefault);
    }

    void UIElement::Serialize(PrettyWriter<StringBuffer>& writer)
    {
        writer.String(_uid.c_str());

        writer.StartObject();
        writer.KeyAndValue("Type", _type.c_str());
        writer.KeyAndValue("Name", _name.c_str());
        WriteVec2(writer, "Position", _rectPos);
        WriteVec2(writer, "Size", _rectSize);
        //WriteFloat4(writer, "Color", _color);
        writer.KeyAndValue("Interactable", _interactable);
        writer.KeyAndValue("Enabled", _enabled);
        //writer.EndObject();
    }

    void UIElement::Deserialize(const rapidjson::Value& obj)
    {
        SetName(obj["Name"].GetString());
        SetRectPos(ReadVec2(obj["Position"]));
    }

    void Text::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        // This is for the base class information
        UIElement::Serialize(writer);
    }

    void Text::Deserialize(const rapidjson::Value& obj)
    {
        UIElement::Deserialize(obj);
    }

    void Panel::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        // This is for the base class information
        UIElement::Serialize(writer);
    }

    void Panel::Deserialize(const rapidjson::Value& obj)
    {
        UIElement::Deserialize(obj);
    }

    void Button::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        // This is for the base class information
        UIElement::Serialize(writer);

        writer.String("Type Specific", 13);
        writer.StartObject();
        writer.String("Colors", 6);
        writer.StartArray();
        WriteFloat4(writer, "Normal", _buttonColors[Mode::BTN_NORMAL]);
        WriteFloat4(writer, "Hovered", _buttonColors[Mode::BTN_HOVERED]);
        WriteFloat4(writer, "Pressed", _buttonColors[Mode::BTN_PRESSED]);
        WriteFloat4(writer, "Disabled", _buttonColors[Mode::BTN_DISABLED]);
        writer.EndArray();
        writer.EndObject();
    }

    void Button::Deserialize(const rapidjson::Value& obj)
    {
        UIElement::Deserialize(obj);
    }

    void Image::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        // This is for the base class information
        UIElement::Serialize(writer);
    }

    void Image::Deserialize(const rapidjson::Value& obj)
    {
        UIElement::Deserialize(obj);
    }
}