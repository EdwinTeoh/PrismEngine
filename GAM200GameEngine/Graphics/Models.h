/*******************************************************************************
filename    Models.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, ---% Code Contribution
            Leow Yumi, 440002919, yumi.leow@digipen.edu, ---% Code Contribution
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include <array>
#include <list>
#include "Shader.h"

namespace Engine
{
    namespace Graphics
    {
        class GLModel
        {
            GLenum _primitive_type = 0;
            GLenum _mode = 0;
            GLuint _vaoID = 0;
            GLuint _draw_cnt = 0;

            // Note: It is not compulsory to use an AOS for it to work, it just makes calculations easier
            struct VerticesTex
            {
                glm::vec3 pos;
                glm::vec3 clr;
                glm::vec2 tex;
            };
            struct Vertices
            {
                glm::vec3 pos;
                glm::vec3 clr;
            };
        public:
            // Default constructor
            GLModel() = default;

            // Constructor with params
            GLModel(GLuint vaoid, GLuint draw_cnt, GLenum primitive, GLenum mode);

            // Getters
            GLenum GetPrimitive_type();
            GLuint GetVAOID();
            GLuint GetDraw_cnt();

            enum ModelType
            {
                QUAD_FILL,
                QUAD_LINE,
                LINE,
                MAX_SIZE
            };

            // Setters
            void SetPrimitive_type(GLenum primitive);
            void SetVAOID(GLuint vaoid);
            void SetDraw_cnt(GLuint draw_cnt);

            void Render();

            // Model creation functions
            static GLModel rect_model();
            static GLModel modelNoTex(float r, float g, float b, bool isLine = false);

            static void Init();

            static std::array<GLModel, MAX_SIZE> modelsCont;
        };
    }
}
