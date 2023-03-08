/*******************************************************************************
filename    Shader.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, ---% Code Contribution
            Leow Yumi, 440002919, yumi.leow@digipen.edu,   ---% Code Contribution
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "../Graphics/Shader.h"

namespace Engine
{
    namespace Graphics
    {
        //std::map<GLShader::SHADER_PRGM, GLShader> GLShader::shadersCont;
        std::array<GLShader, 4> GLShader::shadersCont;

        // Helper function
        GLboolean GLShader::CheckFileExists(const std::string& filename)
        {
            std::ifstream inputfile{ filename };
            return inputfile.good();
        }

        GLuint GLShader::GLCreateProgramEncap()
        {
            if (_pgmHandle <= 0)
            {
                _pgmHandle = glCreateProgram();
                if (_pgmHandle == 0)
                {
                    _logString = "Cannot create Program handle";
                    return GL_FALSE;
                }
            }
            return GL_TRUE;
        }

        //Constructor
        GLShader::GLShader() : _pgmHandle{ 0 }, _linkStatus{ GL_FALSE }
        {

        }

        GLboolean GLShader::CompileLinkValidate(std::vector< std::pair<GLenum, std::string> > shader_lib)
        {
            for (std::pair<GLenum, std::string> elem : shader_lib)
            {
                // Compilation
                if (!CompileShaderFromFile(elem.first, elem.second.c_str()))
                {
                    return GL_FALSE;
                }
            }

            // Linking
            GLboolean linkingSuccessful = Link();
            assert(("Shader linking was unsuccessful!!!", linkingSuccessful));
            (void)linkingSuccessful;

            // Validate
            GLboolean validateSuccessful = Validate();
            assert(("Shader validation was unsuccessful!!!", validateSuccessful));
            (void)validateSuccessful;

            // Debug portion can be included here
            return GL_TRUE;
        }

        GLboolean GLShader::CompileShaderFromFile(GLenum shader_type, const std::string& filename)
        {
            GLboolean checkFileExistsSuccessful = CheckFileExists(filename);
            assert(("Shader file does not exist!!!", checkFileExistsSuccessful));
            (void)checkFileExistsSuccessful;

            GLboolean createProgramEncapSuccessful = (bool)GLCreateProgramEncap();
            assert(("Shader program encap unsuccessful!!!", createProgramEncapSuccessful));
            (void)createProgramEncapSuccessful;

            std::ifstream shader_file(filename, std::ifstream::in);
            if (!shader_file)
            {
                _logString = "Error opening file: " + filename;
                return GL_FALSE;
            }

            std::stringstream buffer;
            buffer << shader_file.rdbuf();
            shader_file.close();
            return CompileShaderFromString(shader_type, buffer.str());
        }

        GLboolean GLShader::CompileShaderFromString(GLenum shader_type, const std::string& shader_src)
        {
            if (GLCreateProgramEncap() == GL_FALSE)
            {
                return GL_FALSE;
            }

            GLuint shader_handle = 0;
            switch (shader_type)
            {
            case VERTEX:
                shader_handle = glCreateShader(GL_VERTEX_SHADER);
                break;
            case FRAGMENT:
                shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
                break;
            default:
                _logString = "Invalid shader type";
                return GL_FALSE;
            }

            // Creating the shader
            const GLchar* const shader_code[] = { shader_src.c_str() };
            glShaderSource(shader_handle, 1, shader_code, NULL);

            // Compile the shader
            glCompileShader(shader_handle);

            // Check compilation result
            GLint compiled;
            glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compiled);
            if (compiled) // Compilation ok
            {
                glAttachShader(_pgmHandle, shader_handle);
                return GL_TRUE;
            }
            else // Compilation failed
            {
                // Discussed with TA, this is how errors should be logged
                GLchar infoLog[512];
                glGetShaderInfoLog(shader_handle, 512, NULL, infoLog);
                std::cout << (const char*)infoLog << std::endl;

                _logString = "Vertex shader compilation failed";
                //GetGLErrorMessage(shader_handle);

                // Get message of the error type
                /*GLint log_length;
                glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
                if (log_length > 0)
                {
                    GLchar* log = new GLchar[log_length];
                    GLsizei written_log_len;
                    glGetShaderInfoLog(shader_handle, log_length, &written_log_len, log);
                    _logString += log;
                    delete[] log;
                }*/
                return GL_FALSE;
            }
        }

        void GLShader::GetGLErrorMessage(GLint handle)
        {
            // Get message of the error type
            GLint log_len;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_len);
            if (log_len > 0)
            {
                GLchar* log_str = new GLchar[log_len];
                GLsizei written_log_len;
                glGetProgramInfoLog(handle, log_len, &written_log_len, log_str);
                _logString += log_str;
                delete[] log_str;
            }
        }

        GLboolean GLShader::Link()
        {
            if (_linkStatus)
                return GL_TRUE;

            if (_pgmHandle <= 0)
                return GL_FALSE;

            glLinkProgram(_pgmHandle);

            // Check link status
            GLint link_status = 0;
            glGetProgramiv(_pgmHandle, GL_LINK_STATUS, &link_status);
            if (link_status) // Linking successful
            {
                _linkStatus = GL_TRUE;
                return GL_TRUE;
            }
            else
            {
                _logString = "Failed to link shader program";
                GetGLErrorMessage(_pgmHandle);
                return GL_FALSE;
            }
        }

        GLboolean GLShader::Validate()
        {
            if ((_pgmHandle <= 0) || (_linkStatus == GL_FALSE))
                return GL_FALSE;

            glValidateProgram(_pgmHandle);
            GLint status;
            glGetProgramiv(_pgmHandle, GL_VALIDATE_STATUS, &status);
            if (status) // Validation successful
            {
                return GL_TRUE;
            }
            else
            {
                _logString = "Failed to validate shader program";
                GetGLErrorMessage(_pgmHandle);
                return GL_FALSE;
            }
        }

        void GLShader::Use()
        {
            if ((_pgmHandle > 0) && (_linkStatus == GL_TRUE))
                glUseProgram(_pgmHandle);
        }

        void GLShader::UnUse()
        {
            glUseProgram(0);
        }

        GLuint GLShader::GetHandle() const
        {
            return _pgmHandle;
        }

        GLboolean GLShader::GetLinkStatus() const
        {
            return _linkStatus;
        }

        std::string GLShader::GetLog() const
        {
            return _logString;
        }

        void GLShader::SetUniform(GLchar const* name, GLboolean val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform1i(loc, val);
            }
            else {
             //   std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, GLint val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform1i(loc, val);
            }
            else {
             //   std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, GLfloat val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform1f(loc, val);
            }
            else {
              //  std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform2f(loc, x, y);
            }
            else {
              //  std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform3f(loc, x, y, z);
            }
            else {
             //   std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void
            GLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform4f(loc, x, y, z, w);
            }
            else {
              //  std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, glm::vec2& val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform2f(loc, val.x, val.y);
            }
            else {
             //   std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, glm::vec3& val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform3f(loc, val.x, val.y, val.z);
            }
            else {
            //    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, glm::vec4& val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniform4f(loc, val.x, val.y, val.z, val.w);
            }
            else {
              //  std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, glm::mat3& val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
            }
            else {
             //   std::cout << "Uniform glm::mat3 variable " << name << " doesn't exist" << std::endl;
            }
        }

        /*void GLShader::SetUniform(GLchar const* name, glm::mat3&& val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
            }
            else {
                std::cout << "Uniform glm::mat3 variable " << name << " doesn't exist" << std::endl;
            }
        }*/

        void GLShader::SetUniform(GLchar const* name, const glm::mat4 val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]);
            }
            else {
               // std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        void GLShader::SetUniform(GLchar const* name, glm::f32* val)
        {
            GLint loc = glGetUniformLocation(_pgmHandle, name);
            if (loc >= 0) {
                glUniformMatrix4fv(loc, 1, GL_FALSE, val);
            }
            else {
              //  std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
            }
        }

        GLShader insert_shdrpgm(std::string vtx_shdr, std::string frg_shdr)
        {
            std::vector< std::pair<GLenum, std::string> > shdr_files
            {
                std::make_pair(GL_VERTEX_SHADER, vtx_shdr),
                std::make_pair(GL_FRAGMENT_SHADER, frg_shdr)
            };

            GLShader shader_pgm;
            GLboolean compileLinkSuccessful = shader_pgm.CompileLinkValidate(shdr_files);
            assert(("Shader program could not compile/link!!!", compileLinkSuccessful));
            (void)compileLinkSuccessful;

            if (GL_FALSE == shader_pgm.GetLinkStatus())
            {
                std::cout << "Unable to compile/link/validate shader programs\n";
                std::cout << shader_pgm.GetLog() << "\n";
                // Discussed with TA, this is how errors should be logged
                GLchar infoLog[512];
                glGetProgramInfoLog(shader_pgm.GetHandle(), 512, NULL, infoLog);
                std::cout << (const char*)infoLog << std::endl;

                GLboolean isValid = shader_pgm.GetLinkStatus();
                assert(("Shader link status invalid!!!", isValid));
                (void)isValid;
            }

            return shader_pgm;
        }

        void GLShader::Init()
        {
            shadersCont[SHADER_TEXTURE] = insert_shdrpgm("Assets/Shaders/sVertShdr.vert", "Assets/Shaders/sFragShdr.frag");
            shadersCont[SHADER_FONT] = insert_shdrpgm("Assets/Shaders/freetype.vert", "Assets/Shaders/freetype.frag");
            shadersCont[SHADER_DEBUG] = insert_shdrpgm("Assets/Shaders/debug.vert", "Assets/Shaders/debug.frag");
            shadersCont[SHADER_PARTICLE] = insert_shdrpgm("Assets/Shaders/particle.vert", "Assets/Shaders/particle.frag");
        }        
    }
}
