/*******************************************************************************
filename    Shader.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, ---% Code Contribution
            Leow Yumi, 440002919, yumi.leow@digipen.edu, ---% Code Contribution
date        September 16, 2020
brief       

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once


#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Engine
{
    namespace Graphics
    {
        class GLShader
        {
            enum SHADERTYPE
            {
                VERTEX = GL_VERTEX_SHADER,
                FRAGMENT = GL_FRAGMENT_SHADER
            };

            GLuint _pgmHandle;
            GLboolean _linkStatus = GL_FALSE;
            std::string _logString;

            // Helper functions
            GLboolean CheckFileExists(const std::string& file_name);
            GLuint GLCreateProgramEncap();
            void GetGLErrorMessage(GLint handle);
            GLboolean Link();
            GLboolean Validate();

        public:
            // Public functions for shader compilation from an vert or frag file
            GLShader();

            GLboolean CompileLinkValidate(std::vector< std::pair<GLenum, std::string> > shader);
            GLboolean CompileShaderFromFile(GLenum shader_type, const std::string& file_name);
            GLboolean CompileShaderFromString(GLenum shader_type, const std::string& shader_src);

            enum SHADER_PRGM
            {
                SHADER_TEXTURE,
                SHADER_FONT,
                SHADER_DEBUG,
                SHADER_PARTICLE
            };

            void Use();
            void UnUse();

            // Getters
            GLuint GetHandle() const;
            GLboolean GetLinkStatus() const;
            std::string GetLog() const;            

            // OpenGL is C-based API and therefore doesn't understand function overloading or templates
            // Therefore, we need a family of functions to specify values of uniform variables of different types for the current program object
            void SetUniform(GLchar const* name, GLboolean val);
            void SetUniform(GLchar const* name, GLint val);
            void SetUniform(GLchar const* name, GLfloat val);
            void SetUniform(GLchar const* name, GLfloat x, GLfloat y);
            void SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z);
            void SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
            void SetUniform(GLchar const* name, glm::vec2& val);
            void SetUniform(GLchar const* name, glm::vec3& val);
            void SetUniform(GLchar const* name, glm::vec4& val);
            void SetUniform(GLchar const* name, glm::mat3& val);
            //void SetUniform(GLchar const* name, glm::mat3&& val);
            void SetUniform(GLchar const* name, const glm::mat4 val);
            void SetUniform(GLchar const* name, glm::f32* val);

            static void Init();

            // Container for shaders
            //static std::map<std::string, GLShader> shadersCont;
            // Enum will correspond to the type of shader
            static std::array<GLShader, 4> shadersCont;
        };

        // Compile, link and insert a shader program using the vertex shader and the fragment shader
        // void insert_shdrpgm(std::string shdr_pgm_name, std::string vtx_shdr, std::string frg_shdr);
    }
}
