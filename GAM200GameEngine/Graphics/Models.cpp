/*******************************************************************************
filename    Models.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, ---% Code Contribution
            Leow Yumi, 440002919, yumi.leow@digipen.edu, ---% Code Contribution
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Models.h"

namespace Engine
{
    namespace Graphics
    {
        std::array<GLModel, GLModel::MAX_SIZE> GLModel::modelsCont;

        GLModel::GLModel(GLuint vaoid, GLuint draw_cnt, GLenum primitive, GLenum mode) :
            _vaoID{ vaoid },
            _draw_cnt{ draw_cnt },
            _primitive_type{ primitive },
            _mode{ mode }
        {

        }

        GLenum GLModel::GetPrimitive_type() { return _primitive_type; }
        GLuint GLModel::GetVAOID() { return _vaoID; }
        GLuint GLModel::GetDraw_cnt() { return _draw_cnt; }

        void GLModel::SetPrimitive_type(GLenum primitive) { _primitive_type = primitive; }
        void GLModel::SetVAOID(GLuint vaoid) { _vaoID = vaoid; }
        void GLModel::SetDraw_cnt(GLuint draw_cnt) { _draw_cnt = draw_cnt; }

        void GLModel::Render()
        {
            glBindVertexArray(_vaoID);
            glPolygonMode(GL_FRONT_AND_BACK, _mode);
            glDrawElements(_primitive_type, _draw_cnt, GL_UNSIGNED_SHORT, nullptr);
            glBindVertexArray(0);
        }

        GLModel GLModel::rect_model()
        {
            std::array<VerticesTex, 4> vtx{};

            // Setting up positions
            // POSITIONS SHOULD BE NORMALIZED
            // (-0.5f to 0.5f), magnitude of 1
            vtx[0].pos = glm::vec3(-0.5f, -0.5f, 0.0f);         // Bottom left
            vtx[1].pos = glm::vec3( 0.5f, -0.5f, 0.0f);         // Bottom right
            vtx[2].pos = glm::vec3(-0.5f,  0.5f, 0.0f);         // Top left
            vtx[3].pos = glm::vec3( 0.5f,  0.5f, 0.0f);         // Top right

            // Setting up colours at each of the positions
            vtx[0].clr = glm::vec3(1.0f, 1.0f, 1.0f);
            vtx[2].clr = glm::vec3(1.0f, 1.0f, 1.0f);
            vtx[1].clr = glm::vec3(1.0f, 1.0f, 1.0f);
            vtx[3].clr = glm::vec3(1.0f, 1.0f, 1.0f);

            GLfloat cornerValue = 1.0f;
            // POSITIONS SHOULD BE RANGED FROM (0.0f to 1.0f)
            vtx[0].tex = glm::vec2(0.0f, 0.0f);                 // Bottom left
            vtx[1].tex = glm::vec2(cornerValue, 0.0f);          // Bottom right
            vtx[2].tex = glm::vec2(0.0f, cornerValue);          // Top left
            vtx[3].tex = glm::vec2(cornerValue, cornerValue);   // Top right

            // Setting idx_vtx values
            std::array<GLushort, 6> idx_vtx{ 0, 1, 2, 2, 3, 1 };

            // Setting up VBO for position, colour and texture
            GLuint vbo_hdl;
            glCreateBuffers(1, &vbo_hdl);
            glNamedBufferStorage(vbo_hdl, sizeof(VerticesTex) * vtx.size(), nullptr, GL_DYNAMIC_STORAGE_BIT);
            glNamedBufferSubData(vbo_hdl, 0, sizeof(VerticesTex) * vtx.size(), vtx.data());

            // Setting up VAO using information from VBO
            GLuint vao_hdl;
            glCreateVertexArrays(1, &vao_hdl);

            glEnableVertexArrayAttrib(vao_hdl, 0);
            glVertexArrayVertexBuffer(vao_hdl, 0, vbo_hdl, offsetof(VerticesTex, VerticesTex::pos), sizeof(VerticesTex));
            glVertexArrayAttribFormat(vao_hdl, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vao_hdl, 0, 0);

            glEnableVertexArrayAttrib(vao_hdl, 1);
            glVertexArrayVertexBuffer(vao_hdl, 1, vbo_hdl, offsetof(VerticesTex, VerticesTex::clr), sizeof(VerticesTex));
            glVertexArrayAttribFormat(vao_hdl, 1, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vao_hdl, 1, 1);

            glEnableVertexArrayAttrib(vao_hdl, 2);
            glVertexArrayVertexBuffer(vao_hdl, 2, vbo_hdl, offsetof(VerticesTex, VerticesTex::tex), sizeof(VerticesTex));
            glVertexArrayAttribFormat(vao_hdl, 2, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vao_hdl, 2, 2);

            //Set up EBO using information from idx_vtx and VAO
            GLuint ebo_hdl;
            glCreateBuffers(1, &ebo_hdl);
            glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(), reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
            glVertexArrayElementBuffer(vao_hdl, ebo_hdl);

            // Break binding
            glBindVertexArray(0);

            return GLModel(vao_hdl, static_cast<GLuint>(idx_vtx.size()), GL_TRIANGLE_STRIP, GL_FILL);
        }

        GLModel GLModel::modelNoTex(float r, float g, float b, bool isLine)
        {
            std::array<Vertices, 4> vtx{};
            std::array<GLushort, 4> idx_vtx{};

            if (isLine)
            {
                // Setting up positions
                // POSITIONS SHOULD BE NORMALIZED
                // (-0.5f to 0.5f), magnitude of 1
                vtx[0].pos = glm::vec3(0.0f, 0.0f, 0.0f); // Middle
                vtx[1].pos = glm::vec3(0.0f, 0.5f, 0.0f); // Top

                // Setting up colours at each of the positions
                vtx[0].clr = glm::vec3(r, g, b);
                vtx[1].clr = glm::vec3(r, g, b);

                // Setting idx_vtx values
                idx_vtx = { 0, 1, 0 };
            }
            else
            {
                // Setting up positions
                // POSITIONS SHOULD BE NORMALIZED
                // (-0.5f to 0.5f), magnitude of 1
                vtx[0].pos = glm::vec3(-0.5f, -0.5f, 0.0f); // Bottom left
                vtx[1].pos = glm::vec3(0.5f, -0.5f, 0.0f);  // Bottom right
                vtx[2].pos = glm::vec3(-0.5f, 0.5f, 0.0f);  // Top left
                vtx[3].pos = glm::vec3(0.5f, 0.5f, 0.0f);   // Top right

                // Setting up colours at each of the positions
                vtx[0].clr = glm::vec3(r, g, b);
                vtx[1].clr = glm::vec3(r, g, b);
                vtx[2].clr = glm::vec3(r, g, b);
                vtx[3].clr = glm::vec3(r, g, b);

                // Setting idx_vtx values
                idx_vtx = { 0, 1, 3, 2 };
            }

            // Setting up VBO for position, colour and texture
            GLuint vbo_hdl;
            glCreateBuffers(1, &vbo_hdl);
            glNamedBufferStorage(vbo_hdl, sizeof(Vertices) * vtx.size(), nullptr, GL_DYNAMIC_STORAGE_BIT);
            glNamedBufferSubData(vbo_hdl, 0, sizeof(Vertices) * vtx.size(), vtx.data());

            // Setting up VAO using information from VBO
            GLuint vao_hdl;
            glCreateVertexArrays(1, &vao_hdl);

            glEnableVertexArrayAttrib(vao_hdl, 0);
            glVertexArrayVertexBuffer(vao_hdl, 0, vbo_hdl, offsetof(Vertices, Vertices::pos), sizeof(Vertices));
            glVertexArrayAttribFormat(vao_hdl, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vao_hdl, 0, 0);

            glEnableVertexArrayAttrib(vao_hdl, 1);
            glVertexArrayVertexBuffer(vao_hdl, 1, vbo_hdl, offsetof(Vertices, Vertices::clr), sizeof(Vertices));
            glVertexArrayAttribFormat(vao_hdl, 1, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vao_hdl, 1, 1);

            // Set up EBO using information from idx_vtx and VAO
            GLuint ebo_hdl;
            glCreateBuffers(1, &ebo_hdl);
            glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(), reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
            glVertexArrayElementBuffer(vao_hdl, ebo_hdl);

            // Break binding
            glBindVertexArray(0);

            return GLModel(vao_hdl, static_cast<GLuint>(idx_vtx.size()), GL_POLYGON, GL_LINE);
        }

        void GLModel::Init()
        {
            modelsCont[QUAD_FILL] = rect_model();
            modelsCont[QUAD_LINE] = modelNoTex(0.0f, 1.0f, 0.0f);
            modelsCont[LINE] = modelNoTex(0.0f, 1.0f, 0.0f, true);
        }
    }
}
