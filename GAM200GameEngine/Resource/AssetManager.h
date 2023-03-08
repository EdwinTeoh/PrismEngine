/*******************************************************************************
filename    AssetManager.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 27, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "../Graphics/FontControl.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"
#include <map>
#include <vector>
#include <filesystem>

#define ASSET_FOLDER_NAME std::string("Assets")

namespace Engine
{
    class AssetManager
    {
        AssetManager() {}
    public:
        static std::unordered_map<std::string, std::string> ShadersPath;
        static std::unordered_map<std::string, std::string> TexturesPath;
        static std::unordered_map<std::string, std::string> FontsPath;

        static void clearPath();

        static bool LoadAssets();

        static void loadShaderFromFilePath(std::string filepath);
        static std::string getShaderFilePath(std::string name);

        static void loadTextureFromFilePath(std::string filepath);
        static std::string getTextureFilePath(std::string name);

        static void loadFontFromFilePath(std::string filepath);
        static std::string getFontFilePath(std::string name);

        static std::unordered_map<std::string, Graphics::GLShader*> ShadersCont;
        static std::unordered_map<std::string, Graphics::Texture*> TexturesCont;
        static std::unordered_map<std::string, Graphics::Font*> FontsCont;
        
        static Graphics::GLShader* loadShader(const std::string& name);
        static Graphics::GLShader* getShader(const std::string& name);
        
        static Graphics::Texture* loadTexture(const std::string& name);
        static Graphics::Texture* getTexture(const std::string& name);
        
        static Graphics::Font* loadFont(const std::string& filePath);
        static Graphics::Font* getFont(const std::string& name);

        static void erase(std::string filename);
        static void clear();
    };
}