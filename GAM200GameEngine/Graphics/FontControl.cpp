/*******************************************************************************
filename    FontControl.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, 100% Code Contribution

date        September 23, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "FontControl.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	namespace Graphics
	{
		TextController* TextController::_instance;

		Font::Font() : 
			_library{ 0 }, 
			_face{ 0 }, 
			_projection{ glm::mat4(1.0f) },
			_shdrRef{ GLShader::shadersCont[GLShader::SHADER_FONT] },
			_interval{ 20.0f }, 
			_scale{ 0.5f },
			_color{ 1.0f,1.0f,1.0f },
			_vao{0}, 
			_vbo{0}
		{

		}

		Font::~Font()
		{

		}

		// Change to take in a const char*
		void Font::Init(const char* filepath)
		{
			// Init Freetype lib
			FT_Error error = FT_Init_FreeType(&_library);
			assert(("Error : Initialization of freetype failed", error == 0));

			// Load font into the face
			//"../fonts/07558_CenturyGothic.ttf"
			error = FT_New_Face(_library, filepath, 0, &_face);
			assert(("Error: failed to load font file", error == 0));

			// Define pixel font size to extract from the face; width and height
			// Width to 0 allows for face to use height to calculate width
			FT_Set_Pixel_Sizes(_face, 0, 48);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			for (unsigned char c = 0; c < 128; ++c)
			{
				if (FT_Load_Char(_face, c, FT_LOAD_RENDER))
				{
					std::cout << "Error: Freetype failed to load glyph" << std::endl;
					continue;
				}

				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D,	0, GL_RED, _face->glyph->bitmap.width,
							_face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, 
							_face->glyph->bitmap.buffer);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				Character character = {
					texture,
					glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
					glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
					static_cast<unsigned int>(_face->glyph->advance.x)
				};
				Characters.insert(std::pair<char, Character>(c, character));
			}			

			// (0,0) at the bottom-left corner
			_projection = glm::ortho(0.0f, static_cast<float>(Window::_initialWidth), 0.0f, static_cast<float>(Window::_initialHeight));

			
			// Generate buffers for VAO and VBO
			glGenVertexArrays(1, &_vao);
			glGenBuffers(1, &_vbo);

			// Bind VAO and VBO for the fonts
			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);

			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

		}

		void Font::RenderText(const std::string text, const glm::vec2 renderPos, float scale)
		{
			scale /= 40;
			_scale = scale;

			_shdrRef.Use();

			/*glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

			//glUniform3f(glGetUniformLocation(_shdrRef.GetHandle(), "textColor"), _color.x, _color.y, _color.z);
			//glUniformMatrix4fv(glGetUniformLocation(_shdrRef.GetHandle(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			GLShader::shadersCont[GLShader::SHADER_FONT].SetUniform("textColor", _color);
			GLShader::shadersCont[GLShader::SHADER_FONT].SetUniform("projection", glm::value_ptr(_projection));

			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(_vao);

			// iterate through all characters
			std::string::const_iterator c;
			glm::vec2 _drawingPos{ renderPos };

			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = Characters[*c];

				float xpos = _drawingPos.x + ch.Bearing.x * scale;
				float ypos = _drawingPos.y - (ch.Size.y - ch.Bearing.y) * scale;

				float w = ch.Size.x * scale;
				float h = ch.Size.y * scale;

				// update VBO for each character
				float vertices[6][4] = {
					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos,     ypos,       0.0f, 1.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },

					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },
					{ xpos + w, ypos + h,   1.0f, 0.0f }
				};

				// render glyph texture over quad
				glBindTexture(GL_TEXTURE_2D, ch.TextureID);

				// VBO 
				glBindBuffer(GL_ARRAY_BUFFER, _vbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// render
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// advance cursors for next glyph (note that advance is number of 1/64 pixels)
				_drawingPos.x += (ch.Advance >> 6)* scale; // bitshift by 6 to get value in pixels (2^6 = 64)
			}
			
			// Reset drawing position to align to left side
			//_drawingPos = { 20.0f, Window::height - 20.0f };
			//_drawingPos = renderPos;

			//glDisable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);

			_shdrRef.UnUse();
		}

		void Font::RenderDtInfo(Utility::Clock& clockSys, const glm::vec2 renderPos)
		{
			std::stringstream dtInfo;
			std::vector<std::string> fontMessages;

			// Organization of the information to be rendered
			// Push back into the vector of strings and then empty the streamstring
			dtInfo << "Runtime: " << round(clockSys._totalRuntime);
			fontMessages.push_back(dtInfo.str());
			dtInfo.str("");

			dtInfo << "FPS: " << round(clockSys._fps);
			fontMessages.push_back(dtInfo.str());
			dtInfo.str("");

			dtInfo << "Phyics: " << (100 * clockSys.GetDT(Utility::SystemCode::PHYSICS) / clockSys.GetGDT());
			fontMessages.push_back(dtInfo.str());
			dtInfo.str("");

			dtInfo << "Collsion: " << (100 * clockSys.GetDT(Utility::SystemCode::COLLISION) / clockSys.GetGDT());
			fontMessages.push_back(dtInfo.str());
			dtInfo.str("");

			dtInfo << "Graphics: " << (100 * clockSys.GetDT(Utility::SystemCode::GRAPHICS) / clockSys.GetGDT());
			fontMessages.push_back(dtInfo.str());
			dtInfo.str("");

			glm::vec2 _drawingPos{ renderPos };
			for (std::string str : fontMessages)
			{
				RenderText(str, _drawingPos);
				_drawingPos.y -= _interval;
			}

			//_drawingPos = { 20.0f, Window::height - 20.0f };
			_drawingPos = renderPos;	
		}

		void Font::SetColor(glm::vec3 color)
		{
			_color = color;
		}

		void Font::Exit()
		{
			Characters.clear();
			FT_Done_Face(_face);
			FT_Done_FreeType(_library);
		}

		TextController* TextController::GetInstance()
		{
			if (!_instance)
				_instance = new TextController;
			return _instance;
		}

		void TextController::Create(const char* fontname, const char* TTFfilepath)
		{
			std::string FontName = fontname;
			if (FontsStorage.find(FontName) != FontsStorage.end())
				return;

			Font textObject;
			textObject.Init(TTFfilepath);

			FontsStorage.insert({ FontName, textObject });
		}

		void TextController::DrawAll()
		{
			for (auto& [words, pos] : TextMessages)
			{
				// Problem is what about different Fonts with different messages
				std::string name;
				FontsStorage[name].RenderText(words, pos);
			}
		}

		void TextController::Exit()
		{
			delete _instance;
		}
	}
}
