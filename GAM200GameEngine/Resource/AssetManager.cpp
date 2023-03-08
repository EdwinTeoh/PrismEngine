/*******************************************************************************
filename    AssetManager.cpp
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        November 27, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "../ECS/ECS.h"
#include "AssetManager.h"

namespace Engine
{
	std::unordered_map<std::string, Graphics::GLShader*> AssetManager::ShadersCont;
	std::unordered_map<std::string, Graphics::Texture*> AssetManager::TexturesCont;
	std::unordered_map<std::string, Graphics::Font*> AssetManager::FontsCont;

	std::unordered_map <std::string, std::string> AssetManager::ShadersPath;
	std::unordered_map <std::string, std::string> AssetManager::TexturesPath;
	std::unordered_map <std::string, std::string> AssetManager::FontsPath;

	Graphics::GLShader* AssetManager::loadShader(const std::string& name)
	{
		ShadersCont.emplace(std::make_pair(name, new Graphics::GLShader()));
		return ShadersCont[name];
	}

	Graphics::GLShader* AssetManager::getShader(const std::string& name)
	{
		return ShadersCont[name];
	}

	Graphics::Texture* AssetManager::loadTexture(const std::string& name)
	{
		TexturesCont.emplace(std::make_pair(name, new Graphics::Texture()));
		return TexturesCont[name];
	}

	Graphics::Texture* AssetManager::getTexture(const std::string& name)
	{
		return TexturesCont[name];
	}

	Graphics::Font* AssetManager::loadFont(const std::string& name)
	{
		FontsCont.emplace(std::make_pair(name, new Graphics::Font()));
		return FontsCont[name];
	
	}

	Graphics::Font* AssetManager::getFont(const std::string& name)
	{
		return FontsCont[name];
	}

	void AssetManager::loadShaderFromFilePath(std::string filepath)
	{
		for (auto& B : std::filesystem::directory_iterator(filepath + "/Shaders"))
		{
			const std::string Bdir = B.path().string();
			AssetManager::ShadersPath.emplace(std::make_pair(B.path().filename().string(), Bdir));
		}
	}

	std::string AssetManager::getShaderFilePath(std::string name)
	{
		auto searchShader = AssetManager::ShadersPath.find(name);
		if (searchShader != AssetManager::ShadersPath.end())
		{
			return searchShader->second;
		}
		else
		{
			return "Not Found.";
		}
	}

	void AssetManager::loadTextureFromFilePath(std::string filepath)
	{
		(void)filepath;
		//std::string pathStr = std::filesystem::current_path().string() + "/Assets/Textures";
		//std::string pathStr = ASSET_FOLDER_NAME + "/Textures";
		//std::filesystem::directory_iterator dItr(pathStr);
		std::filesystem::directory_iterator dItr(ASSET_FOLDER_NAME + "/Textures");

		for (auto& B : dItr)
		{
			const std::string Bdir = B.path().string();
			TexturesPath.emplace(std::make_pair(B.path().filename().string(), Bdir));

			
			TexturesCont.emplace(std::make_pair(B.path().filename().string(), new Graphics::Texture(Bdir)));
		}
	}

	std::string AssetManager::getTextureFilePath(std::string name)
	{
		auto searchTexture = AssetManager::TexturesPath.find(name);
		if (searchTexture != AssetManager::TexturesPath.end())
		{
			return searchTexture->second;
		}
		else
		{
			return "Not Found.";
		}
	}

	void AssetManager::loadFontFromFilePath(std::string filepath)
	{
		for (auto& B : std::filesystem::directory_iterator(filepath + "/Fonts"))
		{
			const std::string Bdir = B.path().string();
			AssetManager::FontsPath.emplace(std::make_pair(B.path().filename().string(), Bdir));
		}
	}

	std::string AssetManager::getFontFilePath(std::string name)
	{
		auto searchFont = AssetManager::FontsPath.find(name);
		if (searchFont != AssetManager::FontsPath.end())
		{
			return searchFont->second;
		}
		else
		{
			return "Not Found.";
		}
	}

	void AssetManager::clearPath()
	{
		AssetManager::ShadersPath.clear();
		AssetManager::TexturesPath.clear();
		AssetManager::FontsPath.clear();
	}

	/*
	* Not in use yet, but I think it'll be good to load everything in one function in the initialisation
	* - Yumi
	*/
	bool AssetManager::LoadAssets()
	{
		// Load Shaders
		// Load Textures
		// Load Fonts

		return false;
	}

	void AssetManager::erase(std::string filename)
	{
		if (ShadersCont.find(filename) != ShadersCont.end())
		{
			std::pair<const std::string, Graphics::GLShader*>& shaders = *(ShadersCont.find(filename));
			delete shaders.second;
		}
	
		if (TexturesCont.find(filename) != TexturesCont.end())
		{
			std::pair<const std::string, Graphics::Texture*>& textures = *(TexturesCont.find(filename));
			
			if (textures.first != "")
			{
			
				delete textures.second;
			}
		}
	
		if (FontsCont.find(filename) != FontsCont.end())
		{
			std::pair<const std::string, Graphics::Font*>& fonts = *(FontsCont.find(filename));
			fonts.second->Exit();
			delete fonts.second;
		}
	}

	void AssetManager::clear()
	{
		for (std::pair<const std::string, Graphics::GLShader*>& shaders : ShadersCont)
		{
			//glDeleteProgram(shaders.second); <- should be calling this but ????????????????
			delete shaders.second;
		}
		
		for (std::pair<const std::string, Graphics::Texture*>& textures : TexturesCont)
		{
			if (textures.first == "")
				continue;
			if (textures.second)
			{
				glDeleteTextures(1, &textures.second->_id);
				delete textures.second;
			}
		}
	
		for (std::pair<const std::string, Graphics::Font*>& fonts : FontsCont)
		{
			fonts.second->Exit();
			delete fonts.second;
		}
	}
}