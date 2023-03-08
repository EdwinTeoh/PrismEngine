/*******************************************************************************
filename    Basic_Particle.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu
date        February 20, 2021
brief

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#include "Particle.h"

#include "Random.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include "../Texture.h"					// For textures
#include "../../Resource/AssetManager.h"
#include "../../Input/Input.h"			// For p_input
#include "../../ECS/Components.h"		// For component names
#include "../../Clock/Clock.h"			// Control spawn rate
#include  "../../GameLogic/Player_State.h"	// For disabling depending on player states
#include "../../Editor/PE_Editor.h"

#define SUCKINSPED 7.0f
#define BLOWSPEED 3.0f

// Add comments

namespace Engine
{
	namespace Graphics
	{
		ParticleSystem* ParticleSystem::_instance;
		float Ctime = 0.0f;
		float Dtime = 0.0f;


		void ParticleProps::SetLine(Vec2D point1, Vec2D point2)
		{
			line.Point1 = glm::vec2(point1._x, point1._y);
			line.Point2 = glm::vec2(point2._x, point2._y);
		}

		void ParticleProps::SetPosition(Vec2D pos)
		{
			Position.x = pos._x;
			Position.y = pos._y;
		}

		void ParticleProps::SetVelocity(Vec2D vel)
		{
			Velocity.x = vel._x;
			Velocity.y = vel._y;
		}

		void ParticleProps::SetVelocityVariation(Vec2D velvar)
		{
			VelocityVariation.x = velvar._x;
			VelocityVariation.y = velvar._y;
		}

		void ParticleProps::SetClrBegin(Vec4D clrbegin)
		{
			ColorBegin = glm::vec4(clrbegin._x, clrbegin._y, clrbegin._z, clrbegin._w);
		}

		void ParticleProps::SetClrEnd(Vec4D clrEnd)
		{
			ColorEnd = glm::vec4(clrEnd._x, clrEnd._y, clrEnd._z, clrEnd._w);
		}

		void ParticleProps::SetSizeBegin(float start)
		{
			SizeBegin = start;
		}

		void ParticleProps::SetSizeEnd(float end)
		{
			SizeEnd = end;
		}

		void ParticleProps::SetLifeTime(float life)
		{
			LifeTime = life;
		}

		void ParticleProps::SetTexturePath(std::string path)
		{
			TexturePath = path;
		}

		void ParticleProps::SetSizeVariation(float sizevar)
		{
			SizeVariation = sizevar;
		}



		ParticleSystem* ParticleSystem::GetInstance()
		{
			if (!_instance)
			{
				_instance = new ParticleSystem;
			}

			return _instance;
		}


		ParticleSystem::ParticleSystem() :
			_ShrRef{GLShader::shadersCont[GLShader::SHADER_PARTICLE]}
		{
			//_ParticlePool.resize(1000);

			_BlowPool.resize(100);
			_SuckPool.resize(100);
			_ECSPool.resize(1000);
		}

		void ParticleSystem::Update(float dt)
		{
			if (!_QuadVAO)
			{
				std::array<Vert, 4> vtx{};

				vtx[0].pos = glm::vec3(-0.5f, -0.5f, 0.0f);		// Bottom left
				vtx[1].pos = glm::vec3(0.5f, -0.5f, 0.0f);		// Bottom right
				vtx[2].pos = glm::vec3(-0.5f, 0.5f, 0.0f);		// Top left
				vtx[3].pos = glm::vec3(0.5f, 0.5f, 0.0f);		// Top right

				vtx[0].tex = glm::vec2(0.0f, 0.0f);				// Bottom left
				vtx[1].tex = glm::vec2(1.0f, 0.0f);				// Bottom right
				vtx[2].tex = glm::vec2(0.0f, 1.0f);				// Top left
				vtx[3].tex = glm::vec2(1.0f, 1.0f);				// Top right


				std::array<GLushort, 6> idx{ 0, 1, 2, 2, 3, 1 };

				// VBO handling
				GLuint vbo;
				glCreateBuffers(1, &vbo);
				glNamedBufferStorage(vbo, sizeof(Vert) * vtx.size(), nullptr, GL_DYNAMIC_STORAGE_BIT);
				glNamedBufferSubData(vbo, 0, sizeof(Vert) * vtx.size(), vtx.data());

				// VAO handling
				glCreateVertexArrays(1, &_QuadVAO);

				glEnableVertexArrayAttrib(_QuadVAO, 0);
				glVertexArrayVertexBuffer(_QuadVAO, 0, vbo, offsetof(Vert, Vert::pos), sizeof(Vert));
				glVertexArrayAttribFormat(_QuadVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_QuadVAO, 0, 0);

				glEnableVertexArrayAttrib(_QuadVAO, 1);
				glVertexArrayVertexBuffer(_QuadVAO, 1, vbo, offsetof(Vert, Vert::tex), sizeof(Vert));
				glVertexArrayAttribFormat(_QuadVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_QuadVAO, 1, 1);

				// EBO handling
				GLuint ebo;
				glCreateBuffers(1, &ebo);
				glNamedBufferStorage(ebo, sizeof(GLushort) * idx.size(), reinterpret_cast<GLvoid*>(idx.data()), GL_DYNAMIC_STORAGE_BIT);
				glVertexArrayElementBuffer(_QuadVAO, ebo);

				glBindVertexArray(0);


				_ParticleShaderViewProj = glGetUniformLocation(_ShrRef.GetHandle(), "u_ViewProj");
				if (_ParticleShaderViewProj < 0)
					std::cout << "UV not found" << std::endl;

				_ParticleShaderTransform = glGetUniformLocation(_ShrRef.GetHandle(), "u_Transform");
				if (_ParticleShaderTransform < 0)
					std::cout << "UV not found" << std::endl;

				_ParticleShaderColor = glGetUniformLocation(_ShrRef.GetHandle(), "u_Color");
				if (_ParticleShaderColor < 0)
					std::cout << "UV not found" << std::endl;
			}


			for (auto& blowParticles : _BlowPool)
			{
				if (!blowParticles.Active)
					continue;

				// Check if the particle still has life remaining
				if (blowParticles.LifeRemaining <= 0.0f)
				{
					blowParticles.Active = false;
					continue;
				}

				// Remaining particles are still active
				if (ECS::compManager->CheckForList<VacuumComponent>())
				{
					EntityID id = ECS::compManager->GetComponentList<VacuumComponent>().GetList().begin()->first;
					// Access the transform component of the vacuum
					if (ECS::compManager->CheckForEntity<TransformComponent>(id))
					{
						TransformComponent trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);

						// Facing right
						if (ECS::compManager->GetEntityComponent<RectColliderComponent>(id)._direction
							== RectColliderComponent::Direction::RIGHT)
						{
							if (blowParticles.Position.x < trans._position._x)
							{
								blowParticles.Active = false;
								break;
							}
						}
						// Facing left
						else if (ECS::compManager->GetEntityComponent<RectColliderComponent>(id)._direction
							== RectColliderComponent::Direction::LEFT)
						{
							if (blowParticles.Position.x > trans._position._x)
							{
								blowParticles.Active = false;
								break;
							}
						}
					}			
				}

				blowParticles.Position += blowParticles.Velocity * dt;
				blowParticles.LifeRemaining -= dt;
				blowParticles.Rotation += 0.01f * dt;
			}

			for (auto& pushParticles : _SuckPool)
			{
				if (!pushParticles.Active)
					continue;

				// Check if the particle still has life remaining
				if (pushParticles.LifeRemaining <= 0.0f)
				{
					pushParticles.Active = false;
					continue;
				}


				// Remaining particles are still active
				if (ECS::compManager->CheckForList<VacuumComponent>())
				{
					EntityID id = ECS::compManager->GetComponentList<VacuumComponent>().GetList().begin()->first;
					// Access the transform component of the vacuum
					if (ECS::compManager->CheckForEntity<TransformComponent>(id))
					{
						TransformComponent trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);

						// Facing right
						if (ECS::compManager->GetEntityComponent<RectColliderComponent>(id)._direction
							== RectColliderComponent::Direction::RIGHT)
						{
							if (pushParticles.Position.x < trans._position._x)
							{
								pushParticles.Active = false;
								break;
							}
						}
						// Facing left
						else if (ECS::compManager->GetEntityComponent<RectColliderComponent>(id)._direction
							== RectColliderComponent::Direction::LEFT)
						{
							if (pushParticles.Position.x > trans._position._x)
							{
								pushParticles.Active = false;
								break;
							}
						}
					}					
				}

				pushParticles.Position += pushParticles.Velocity * dt;
				pushParticles.LifeRemaining -= dt;
				pushParticles.Rotation += 0.01f * dt;
			}

			/* todo
			for each emitter
				for each particle pool inside each emitter
					update position and / or rotation
			*/
		}

		void ParticleSystem::UpdateECS(float dt)
		{
			for (auto& particle : _ECSPool)
			{
				if (!particle.Active)
					continue;

				// Check if the particle still has life remaining
				if (particle.LifeRemaining <= 0.0f)
				{
					particle.Active = false;
					continue;
				}

				particle.Position += particle.Velocity * dt;
				particle.LifeRemaining -= dt;
				particle.Rotation += 0.01f * dt;
			}
		}

		void ParticleSystem::Render(Camera* camera)
		{
			_ShrRef.Use();

			glm::mat4 ViewProj = camera->GetProjMatrix() * camera->GetViewMatrix();
			glUniformMatrix4fv(_ParticleShaderViewProj, 1, GL_FALSE, glm::value_ptr(ViewProj));

			for (auto& blowParticle : _BlowPool)
			{
				if (!blowParticle.Active)
					continue;

				// For fade away effect
				float life = blowParticle.LifeRemaining / blowParticle.LifeTime;

				float size = glm::lerp(blowParticle.SizeEnd, blowParticle.SizeBegin, life);

				glm::vec4 clr = glm::lerp(blowParticle.ColorEnd, blowParticle.ColorBegin, life);

				glm::mat4 TransMat{ 1.0f };
				TransMat = glm::translate(TransMat, glm::vec3(blowParticle.Position.x, blowParticle.Position.y, 0.0f));
				TransMat = glm::rotate(TransMat, glm::radians(blowParticle.Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
				TransMat = glm::scale(TransMat, glm::vec3(size, size, 0.0f));

				glUniformMatrix4fv(_ParticleShaderTransform, 1, GL_FALSE, glm::value_ptr(TransMat));
				glUniform4fv(_ParticleShaderColor, 1, glm::value_ptr(clr));

				// Texture settings, using random texture
				_ShrRef.SetUniform("useTexture", true);
				Texture* tex = AssetManager::getTexture("DustCloud.png");
				glBindTextureUnit(1, tex->_id);
				_ShrRef.SetUniform("tex2d", 1);

				// Alpha
				_ShrRef.SetUniform("alpha", 0.6f);

				// Drawing begins
				glBindVertexArray(_QuadVAO);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, nullptr);

				glBindVertexArray(0);
			}

			for (auto& pushParticle : _SuckPool)
			{
				if (!pushParticle.Active)
					continue;

				// For fade away effect
				float life = pushParticle.LifeRemaining / pushParticle.LifeTime;

				float size = glm::lerp(pushParticle.SizeEnd, pushParticle.SizeBegin, life);

				glm::vec4 clr = glm::lerp(pushParticle.ColorEnd, pushParticle.ColorBegin, life);

				glm::mat4 TransMat{ 1.0f };
				TransMat = glm::translate(TransMat, glm::vec3(pushParticle.Position.x, pushParticle.Position.y, 0.0f));
				TransMat = glm::rotate(TransMat, glm::radians(pushParticle.Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
				TransMat = glm::scale(TransMat, glm::vec3(size, size, 0.0f));

				glUniformMatrix4fv(_ParticleShaderTransform, 1, GL_FALSE, glm::value_ptr(TransMat));
				glUniform4fv(_ParticleShaderColor, 1, glm::value_ptr(clr));

				// Texture settings
				_ShrRef.SetUniform("useTexture", true);
				Texture* tex = AssetManager::getTexture("DustCloud.png");
				glBindTextureUnit(1, tex->_id);
				_ShrRef.SetUniform("tex2d", 1);

				// Alpha
				_ShrRef.SetUniform("alpha", 0.6f);

				// Drawing begins
				glBindVertexArray(_QuadVAO);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, nullptr);

				glBindVertexArray(0);
			}

			_ShrRef.UnUse();
		}

		void ParticleSystem::RenderECS(Camera* camera)
		{
			_ShrRef.Use();

			glm::mat4 ViewProj = camera->GetProjMatrix() * camera->GetViewMatrix();
			glUniformMatrix4fv(_ParticleShaderViewProj, 1, GL_FALSE, glm::value_ptr(ViewProj));

			for (auto& particle : _ECSPool)
			{
				if (!particle.Active)
					continue;

				// Calculations with life of the particle remaining
				float life = particle.LifeRemaining / particle.LifeTime;
				float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);
				glm::vec4 clr = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);

				glm::mat4 TransMat{ 1.0f };
				TransMat = glm::translate(TransMat, glm::vec3(particle.Position.x, particle.Position.y, 0.0f));
				TransMat = glm::rotate(TransMat, glm::radians(particle.Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
				TransMat = glm::scale(TransMat, glm::vec3(size, size, 0.0f));

				glUniformMatrix4fv(_ParticleShaderTransform, 1, GL_FALSE, glm::value_ptr(TransMat));
				glUniform4fv(_ParticleShaderColor, 1, glm::value_ptr(clr));

				// Texture settings
				// If there is texture use texture, if use color
				if (particle.texture != "No Texture")
				{
					_ShrRef.SetUniform("useTexture", true);
					
					Texture* tex = AssetManager::getTexture(particle.texture);
					if (tex)
						glBindTextureUnit(1, tex->_id);

					//glBindTextureUnit(1, tex->_id ? tex->_id : 0);
					_ShrRef.SetUniform("tex2d", 1);

					_ShrRef.SetUniform("alpha", clr.w);
				}
				else
				{
					//std::cout << "not using texture" << std::endl;
					_ShrRef.SetUniform("useTexture", false);
				}

				// Drawing begins
				glBindVertexArray(_QuadVAO);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, nullptr);

				glBindVertexArray(0);
			}

			_ShrRef.UnUse();
		}

		void ParticleSystem::Emit(const ParticleProps& particleProps)
		{
			// Check if there is a vacuum component in the scene
			if (ECS::compManager->CheckForList<VacuumComponent>())
			{
				EntityID id = ECS::compManager->GetComponentList<VacuumComponent>().GetList().begin()->first;
				
				TransformComponent& trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);
				RectColliderComponent& rect = ECS::compManager->GetEntityComponent<RectColliderComponent>(id);

				
				// Suck in
				if (p_Input->MouseButtonIsHeld(GLFW_MOUSE_BUTTON_LEFT))
				{
					// Clear blow particles
					for (auto& blowparticles : _BlowPool)
						blowparticles.Active = false;

					Particle& particle = _SuckPool[_Index];
					particle.Active = true;

					// Set yellow
					particle.ColorBegin = glm::vec4{ 254 / 255.0f, 191 / 255.0f, 1 / 255.0f, 0.4f };

					// Chceck facing direction
					if (rect._direction == RectColliderComponent::Direction::RIGHT)
					{
						float u = Random::Float();
						particle.Position = (1.0f - u) * glm::vec2(trans._position._x + rect._width, trans._position._y + rect._height) +
							u * glm::vec2(trans._position._x + rect._width, trans._position._y - rect._height);

						// Set direction 
						particle.Velocity = glm::vec2(trans._position._x, trans._position._y) - particle.Position;
						particle.Velocity = glm::normalize(particle.Velocity);

						// Speed modifier
						particle.Velocity = glm::vec2(particle.Velocity.x * SUCKINSPED, particle.Velocity.y * SUCKINSPED);
					}
					else if (rect._direction == RectColliderComponent::Direction::LEFT)
					{
						float u = Random::Float();
						particle.Position = (1.0f - u) * glm::vec2(trans._position._x - rect._width, trans._position._y + rect._height) +
							u * glm::vec2(trans._position._x - rect._width, trans._position._y - rect._height);

						// Set direction 
						particle.Velocity = glm::vec2(trans._position._x, trans._position._y) - particle.Position;
						particle.Velocity = glm::normalize(particle.Velocity);

						// Speed modifier
						particle.Velocity = glm::vec2(particle.Velocity.x * SUCKINSPED, particle.Velocity.y * SUCKINSPED);
					}

					// Set remaining parameters using particle props
					particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

					// Color
					particle.ColorEnd = glm::vec4{ particle.ColorBegin.x, particle.ColorBegin.y, particle.ColorBegin.z, 0.0f };

					particle.LifeTime = particleProps.LifeTime;
					particle.LifeRemaining = particleProps.LifeTime;
					particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
					particle.SizeEnd = particleProps.SizeEnd;

					_Index = (_Index - 1) % _SuckPool.size();
				}
				// Blow out
				else if (p_Input->MouseButtonIsHeld(GLFW_MOUSE_BUTTON_RIGHT))
				{
					// Clear suck particle pool
					for (auto& suckparticle : _SuckPool)
						suckparticle.Active = false;

					Particle& particle = _BlowPool[_Index];
					particle.Active = true;

					// Set green
					particle.ColorBegin = glm::vec4{ 138 / 255.0f, 222 / 255.0f, 92 / 255.0f, 0.4f };

					// Chceck facing direction
					if (rect._direction == RectColliderComponent::Direction::RIGHT)
					{
						// Starting position
						particle.Position.x = trans._position._x + 1.5f;	// Offset included
						particle.Position.y = trans._position._y;

						// Set direction
						float u = Random::Float();
						particle.Velocity = ((1.0f - u) * glm::vec2(trans._position._x + rect._width, trans._position._y + rect._height) +
							u * glm::vec2(trans._position._x + rect._width, trans._position._y - rect._height)) -
							particle.Position;

						//particle.Velocity = glm::normalize(particle.Velocity);

						// Speed modifier
						particle.Velocity = glm::vec2(particle.Velocity.x * BLOWSPEED, particle.Velocity.y * BLOWSPEED);
					}
					else if (rect._direction == RectColliderComponent::Direction::LEFT)
					{
						// Starting position
						particle.Position.x = trans._position._x - 1.5f;	// Offset included
						particle.Position.y = trans._position._y;

						// Set direction
						float u = Random::Float();
						particle.Velocity = ((1.0f - u) * glm::vec2(trans._position._x - rect._width, trans._position._y + rect._height) +
							u * glm::vec2(trans._position._x - rect._width, trans._position._y - rect._height)) -
							particle.Position;

						//particle.Velocity = glm::normalize(particle.Velocity);

						// Speed modifier
						particle.Velocity = glm::vec2(particle.Velocity.x * BLOWSPEED, particle.Velocity.y * BLOWSPEED);
					}

					// Set remaining parameters using particle props
					particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

					// Color
					particle.ColorEnd = glm::vec4{ particle.ColorBegin.x, particle.ColorBegin.y, particle.ColorBegin.z, 0.0f };

					particle.LifeTime = particleProps.LifeTime;
					particle.LifeRemaining = particleProps.LifeTime;

					particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
					particle.SizeEnd = particleProps.SizeEnd;

					_Index = (_Index - 1) % _BlowPool.size();
				}


			}
		}		

		void ParticleSystem::EmitSpawnRate(const ParticleProps& particleProps, const double spawnRate, const unsigned spawnAmt)
		{
			// If player state is not idle or walking, disable generation
			if ((sJeffState != JeffState::j_Idle) && (sJeffState != JeffState::j_Walk))
				return;

			// If vacuum is holding something, disable generation
			if (auto& map = ECS::compManager->GetComponentList<VacuumComponent>().GetList(); !map.empty())
			{
				VacuumComponent vac = map.begin()->second;
				if (vac._isHolding)
					return;

				if (Ctime > spawnRate)
				{
					for (unsigned i = 0; i < spawnAmt; ++i)
						Emit(particleProps);
					Ctime = 0.0f;
				}
				else
				{
					Ctime += p_Clock.GetDT(Utility::SystemCode::GRAPHICS);
				}
			}

			
		}

		void ParticleSystem::ECSEmit(const ParticleComponent& part, const TransformComponent& trans)
		{
			// Access a certain particle from the particle pool
			// Set its properties from the particleProps
			// advance the index counter

			Particle& particle = _ECSPool[_ECSIndex];
			particle.Active = true;


			// Position take the transform position and offset
			if (part.GetSourcemode() == Engine::ECS::Components::ParticleSystem::SourceType::Line)
			{
				// Set position for line
				float u = Random::Float();
				particle.Position = (1.0f - u) * glm::vec2(trans._position._x + part.Point1OffSet()._x, trans._position._y + part.Point1OffSet()._y)
					+ u * glm::vec2(trans._position._x + part.Point2OffSet()._x, trans._position._y + part.Point2OffSet()._y);

				// Set Velocity for line
				particle.Velocity = glm::vec2(part.GetVelocity()._x, part.GetVelocity()._y) * Random::Float();
			}
			else if (part.GetSourcemode() == Engine::ECS::Components::ParticleSystem::SourceType::Circle)
			{
				// Set position for circle
				while (true)
				{
					// Probe random points
					float u_x = Random::FloatRanged();
					float u_y = Random::FloatRanged();

					// Check if probed spot is within a circle
					if (sqrt(u_x * u_x + u_y * u_y) < 1)
					{
						// Passes; multiply with radius of the circle and add it to the transform position
						particle.Position = glm::vec2(trans._position._x + part.GetSphereOffset()._x, trans._position._y + part.GetSphereOffset()._y) 
							+ static_cast<float>(part.GetSphereRadius()) * glm::vec2(u_x, u_y);
						break;
					}
				}

				// Set velocity
				if (part.GetEmittermode() == Engine::ECS::Components::ParticleSystem::EmitterType::Normal)
				{
					particle.Velocity = glm::vec2(part.GetVelocity()._x, part.GetVelocity()._y) * Random::Float();
				}
				else if (part.GetEmittermode() == Engine::ECS::Components::ParticleSystem::EmitterType::Burst)
				{
					// Calculate direction
					particle.Velocity = particle.Position - 
						glm::vec2(trans._position._x + part.GetSphereOffset()._x, trans._position._y + part.GetSphereOffset()._y);
					
					// Normalize
					particle.Velocity = glm::normalize(particle.Velocity);

					// Scale with size
					particle.Velocity.x *= part.GetVelocity()._x;
					particle.Velocity.y *= part.GetVelocity()._y;
				}
				else if (part.GetEmittermode() == Engine::ECS::Components::ParticleSystem::EmitterType::Suction)
				{
					// Calculate direction
					particle.Velocity = glm::vec2(trans._position._x + part.GetSphereOffset()._x, trans._position._y + part.GetSphereOffset()._y) -
						particle.Position;

					// Normalize
					particle.Velocity = glm::normalize(particle.Velocity);

					// Scale with size
					particle.Velocity.x *= part.GetVelocity()._x;
					particle.Velocity.y *= part.GetVelocity()._y;
				}
			}
			

			// Rotation
			particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

			// Colors
			particle.ColorBegin = glm::vec4(part.GetColorBegin()._x, part.GetColorBegin()._y, 
				part.GetColorBegin()._z, part.GetColorBegin()._w);
			particle.ColorEnd = glm::vec4(part.GetColorEnd()._x, part.GetColorEnd()._y, 
				part.GetColorEnd()._z, part.GetColorEnd()._w);

			// Life
			particle.LifeTime = static_cast<float>(part.GetLifetime());
			particle.LifeRemaining = static_cast<float>(part.GetLifetime());

			// Size
			particle.SizeBegin = static_cast<float>(part.GetSizeBegin()) + 
									static_cast<float>(part.GetSizeVariation()) * (Random::Float() - 0.5f);
			particle.SizeEnd = static_cast<float>(part.GetSizeEnd());

			// Texture
			particle.texture = part.GetTexName();

			// Move index
			/*if (_ECSIndex == 0)
				_ECSIndex = _ECSMaxIndex;
			else
				--_ECSIndex;*/

			_ECSIndex = (_ECSIndex == 0) ? _ECSMaxIndex : --_ECSIndex;
		}

		void ParticleSystem::ECSSpawn(const ParticleComponent& part, const TransformComponent& trans)
		{
			if (Dtime > part.GetSpawnRate())
			{
				for (unsigned i = 0; i < part.GetSpawnAmt(); ++i)
					ECSEmit(part, trans);
				Dtime = 0.0f;
			}
			else
			{
				Dtime += p_Clock.GetDT(Utility::SystemCode::GRAPHICS);
			}
		}

		void ParticleSystem::ClearAll()
		{
			for (auto& p : _BlowPool)
				p.Active = false;

			for (auto& p : _SuckPool)
				p.Active = false;

			for (auto& p : _ECSPool)
				p.Active = false;
		}

		void ParticleSystem::Exit()
		{
		}
}
}