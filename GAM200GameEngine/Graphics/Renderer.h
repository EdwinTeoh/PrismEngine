/*******************************************************************************
filename    Renderer.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, 100% Code Contribution
date        November 02, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include <GL/glew.h>

#include "Models.h"

namespace Engine
{
	namespace Graphics
	{
		class Renderer
		{
		public:
			static void Init();
			static void Exit();

			static void Clear();

			static void Render(GLModel& model, GLuint rasterMode);
			static void RenderDebug(GLModel& model);


			static void StartBatch();
			static void EndBatch();
			static void Flush();

			static void DrawQuad();

			static void BatchRender();

			static void InstancedRender();
		};
	}
}