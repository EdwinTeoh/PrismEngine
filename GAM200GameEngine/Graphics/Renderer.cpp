/*******************************************************************************
filename    Renderer.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, 100% Code Contribution
date        November 02, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "Renderer.h"
#include "../Window/Window.h"
#include "Shader.h"

namespace Engine
{
	namespace Graphics
	{
		void Renderer::Init()
		{
			
		}

		void Renderer::Exit()
		{

		}

		void Renderer::Clear()
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void Renderer::Render(GLModel& model, GLuint rasterMode)
		{
			void(model), void(rasterMode);
			/*
			For each shader
			For each texture
			For each vertex buffer
			For each object
			{
				Write in uniform data
				glDrawElements or something
			}
			*/

			//glBindVertexArray(model.GetVAOID());

			//switch (rasterMode)
			//{
			//case Window::BOTH:
			//case Window::SOLID:
			//	glEnable(GL_BLEND);
			//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//	glDrawElements(model.GetPrimitive_type(), model.GetDraw_cnt(), GL_UNSIGNED_SHORT, nullptr);
			//	if (Window::_rasterMode != Window::BOTH) { break; }
			//case Window::LINES:
			//	GLShader::shadersCont[GLShader::SHADER_TEXTURE].SetUniform("useTexture", false);
			//	glLineWidth(4.0f);
			//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//	glDrawElements(model.GetPrimitive_type(), model.GetDraw_cnt(), GL_UNSIGNED_SHORT, nullptr);
			//	break;
			//default:
			//	break;
			//}

			//glBindVertexArray(0);
		}

		void Renderer::RenderDebug(GLModel& model)
		{
			glBindVertexArray(model.GetVAOID());

			glLineWidth(0.1f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(model.GetPrimitive_type(), model.GetDraw_cnt(), GL_UNSIGNED_SHORT, nullptr);

			glBindVertexArray(0);
		}

		void Renderer::StartBatch()
		{
		}

		void Renderer::EndBatch()
		{
		}

		void Renderer::BatchRender()
		{
			// Dont know if should do anything before this like set the uniforms or something
			//glDrawElementsInstanced();
			//glBindVertexArray(0);
		}

		void Renderer::InstancedRender()
		{




			//glDrawElementsInstanced();
		}
	}
}