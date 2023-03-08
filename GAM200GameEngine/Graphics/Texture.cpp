/*******************************************************************************
filename    Texture.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, 100% Code Contribution
date        November 13, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine
{
	namespace Graphics
	{
		Texture::Texture()
		{

		}

		Texture::Texture(const std::string filepath) : 
			_width{ 0 },
			_height{ 0 },
			_internalFormat{ GL_RGBA },
			_imageFormat{ GL_RGBA },
			_wrapModeS{ GL_CLAMP_TO_EDGE },
			_wrapModeT{ GL_CLAMP_TO_EDGE },
			_filterModeMin{ GL_LINEAR_MIPMAP_LINEAR },
			_filterModeMax{ GL_LINEAR }
		{
			int channels;

			// Given amount of texture objects, ses their referenced ids in array
			glGenTextures(1, &_id);

			// STBI invert y axis if not images will be inverted
			stbi_set_flip_vertically_on_load(true);
			
			// STBI load data from filepath
			unsigned char* data = stbi_load(filepath.c_str(), &_width, &_height, &channels, 0);

			// Create the texture
			Create(data);

			//free image data
			stbi_image_free(data);
		}

		void Texture::Create(/*unsigned int width, unsigned int height,*/ unsigned char* data)
		{
			//_width = width;
			//_height = height;

			// Create texture
			glBindTexture(GL_TEXTURE_2D, _id);
			glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _imageFormat, GL_UNSIGNED_BYTE, data);

			// Setting texture parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapModeS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapModeT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filterModeMin);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filterModeMax);

			glGenerateMipmap(GL_TEXTURE_2D);
			
			// Unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void Texture::Bind() const
		{
			glBindTexture(GL_TEXTURE_2D, _id);
		}
	}
}