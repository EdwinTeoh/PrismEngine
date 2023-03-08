/*******************************************************************************
filename    FontControl.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, 100% Code Contribution

date        September 23, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once

#include "../Clock/Clock.h"
#include "../Window/Window.h"
#include "Shader.h"
#include <ft2build.h>
#include <map>
#include <string>

#include FT_FREETYPE_H

namespace Engine
{
	namespace Graphics
	{
		class Font
		{	
			struct Character
			{
				unsigned int TextureID;
				glm::ivec2 Size;
				glm::ivec2 Bearing;
				unsigned int Advance;
			};

			std::map<char, Character> Characters;

			FT_Library _library;
			FT_Face _face;			// Font loaded as face
			glm::mat4 _projection;

			//glm::vec2 _renderPos;
			//glm::vec2 _drawingPos{ 20.0f, Window::height - 20.0f };
			//glm::vec2 _drawingPos{ 0.0f, 0.0f };
			float _interval;
			float _scale;
			glm::vec3 _color;


			GLShader& _shdrRef;

			unsigned int _vao, _vbo;
		public:
			// Create font with its shader from the shader container
			Font();
			~Font();

			// Init from a .ttf font file
			void Init(const char* filepath);
			
			// Reference of (0,0) taken from the bottom-left
			// Render dt information on the screen
			void RenderDtInfo(Utility::Clock& clockSys, const glm::vec2 renderPos);

			// Render a text on the screen
			void RenderText(const std::string text, const glm::vec2 renderPos, float scale = 40.0f);

			// Set colour of the text; Default is black
			void SetColor(glm::vec3 color);

			// Free allocated memory for the font face and library
			void Exit();
		};

		class TextController
		{
			static TextController* _instance;
		public:
			static TextController* GetInstance();

			// Map of the font name and the font object to be saved in the system
			std::map<std::string, Font> FontsStorage;

			// Map of all the messages and the position it will be rendered
			std::map<std::string, glm::vec2> TextMessages;

			void Create(const char* fontname, const char* TTFfilepath);

			void DrawAll();

			void Exit();
		};
	}
}