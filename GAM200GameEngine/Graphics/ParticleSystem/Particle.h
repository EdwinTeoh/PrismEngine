/*******************************************************************************
filename    Basic_Particle.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu
date        February 20, 2021
brief

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/


#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <array>

#include "../Camera.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../../ECS/Components.h"

#define p_Particle Engine::Graphics::ParticleSystem::GetInstance()

namespace Engine
{
	namespace Graphics
	{
		// Things that you can change
		struct ParticleProps
		{
			// For line
			struct
			{
				glm::vec2 Point1;
				glm::vec2 Point2;
			} line;
			

			glm::vec2 Position;
			glm::vec2 Velocity, VelocityVariation;
			glm::vec4 ColorBegin, ColorEnd;
			float SizeBegin, SizeEnd, SizeVariation;
			float LifeTime;

			// texture string
			std::string TexturePath;



			// Default constructor for props
			ParticleProps() :
				Position{ 0.0f, 0.0f }, Velocity{ 0.0f, 0.0f },
				VelocityVariation{ 3.0f, 1.0f },
				ColorBegin{ 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f },
				ColorEnd{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f },
				SizeBegin{ 0.7f }, SizeEnd{ 0.0f },
				SizeVariation{ 0.1f },
				LifeTime{ 0.7f }
			{

			}

			void SetLine(Vec2D point1, Vec2D point2);

			void SetPosition(Vec2D pos);
			void SetVelocity(Vec2D vel);
			void SetVelocityVariation(Vec2D velvar);
			void SetClrBegin(Vec4D clrBegin);
			void SetClrEnd(Vec4D clrEnd);
			void SetSizeBegin(float start);
			void SetSizeEnd(float end);
			void SetSizeVariation(float sizevar);
			void SetLifeTime(float life);
			void SetTexturePath(std::string path);
		};

		class ParticleSystem
		{
			struct Vert
			{
				glm::vec3 pos;
				//glm::vec4 clr;
				glm::vec2 tex;
			};

			struct Particle
			{
				glm::vec2 Position;
				glm::vec2 Velocity;
				glm::vec4 ColorBegin;
				glm::vec4 ColorEnd;
				float Rotation;
				float SizeBegin, SizeEnd;

				float LifeTime;
				float LifeRemaining;

				std::string texture;

				bool Active;

				Particle():
					Position{0.0f, 0.0f}, Velocity{0.0f, 0.0f},
					ColorBegin{1.0f, 1.0f, 1.0f, 1.0f}, ColorEnd{0.0f, 0.0f, 0.0f, 1.0f}, 
					Rotation{0.0f}, SizeBegin{1.0f}, SizeEnd{1.0f},
					LifeTime{ 1.0f }, LifeRemaining{ 0.0f }, Active{ false }
				{
				}
			};



			std::vector<Particle> _BlowPool;
			std::vector<Particle> _SuckPool;

			unsigned _Index = 99;

			// For ECS
			std::vector<Particle> _ECSPool;
			unsigned _ECSIndex = 999;
			unsigned _ECSMaxIndex = 999;

			GLuint _QuadVAO;
			GLShader& _ShrRef;
			GLint _ParticleShaderViewProj, _ParticleShaderTransform, _ParticleShaderColor;

			static ParticleSystem* _instance;

			void Emit(const ParticleProps& particleProps);

			void ECSEmit(const ParticleComponent& component, const TransformComponent& trans);
		public:
			static ParticleSystem* GetInstance();

			ParticleSystem();

			void Update(float dt);
			void UpdateECS(float dt);

			void Render(Camera* camera);
			void RenderECS(Camera* camera);

			void EmitSpawnRate(const ParticleProps& particleProps, const double spawnRate, const unsigned spawnAmt);

			void ECSSpawn(const ParticleComponent& part, const TransformComponent& trans);

			void ClearAll();

			void Exit();
		};
	}
}
