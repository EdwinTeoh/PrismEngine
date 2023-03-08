/*******************************************************************************
filename    Texture.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, 100% Code Contribution
date        November 13, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once

#include <GL/glew.h>
#include <string>

namespace Engine
{
	namespace Graphics
	{
		class Texture
		{
		public:
			unsigned int _id;
			int _width, _height;
			unsigned int _internalFormat;
			unsigned int _imageFormat;

			unsigned int _wrapModeS;
			unsigned int _wrapModeT;

			unsigned int _filterModeMin;
			unsigned int _filterModeMax;

			Texture();
			Texture(const std::string filepath);

			// Generate texture from image data
			void Create(/*unsigned int width, unsigned int height,*/ unsigned char* data);

			// Binds the texture as the current active GL_TEXTURE_2D texture object
			void Bind() const;

		};
	}
}