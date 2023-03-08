/*!
@file	 Camera.h
@author  Edwin Teoh Yan-jie, t.yanjieedwin@digipen.edu, 100% Code Contribution
@date    20/09/2020
@brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.

*//*__________________________________________________________________________*/

#pragma once

#include "../ECS/Transform.h"
#include "../Window//Window.h"

#include <GL/glew.h>
#include <GlFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define p_Camera Engine::Graphics::Camera::GetInstance()
#define TransformComponent Engine::ECS::Components::Transform

namespace Engine
{
	namespace Graphics
	{
		class Camera
		{
			glm::vec3 _position;
			glm::vec3 _prevPos;
			glm::vec3 _front;
			glm::vec3 _up;
			glm::vec3 _right;
			glm::vec3 _worldup;

			GLint _viewportWidth, _viewportHeight;
			float _nearPlane;
			float _farPlane;
			float _aspect;
			float _vertFOV;

			float _movementSpeed = 1.0f;
			float _zoomAmount = 16.0f;
			float _zoomUpperLimit = 20.0f;
			float _zoomLowerLimit = -20.0f;

			glm::mat4 _projection;
			glm::mat4 _view;

			GLboolean _zoomed;

			enum CAMERA_MOVEMENT
			{
				FORWARD,
				BACKWARD,
				LEFT,
				RIGHT
			};
			glm::mat4 CalculateViewMatrix();
			glm::mat4 CalculateOrthMatrix();
			glm::mat4 CalculatePespMatrix();

			static Camera* _instance;
		public:
			//Camera();
			Camera();
			~Camera() = default;

			void InternalUpdate();

			// Getters for view and set projection matrix
			// Projection matrix can either be ortho or perspective
			glm::mat4 GetViewMatrix();
			glm::mat4 GetProjMatrix();

			// Getters for other parameters
			GLint GetViewportWidth() const;
			GLint GetViewportHeight() const;

			float GetNearDist() const;
			float GetFarDist() const;
			float GetAspect() const;
			float GetFOV() const;
			float GetZoom() const;

			Vec2D GetPositionVec2() const;

			glm::vec3 GetUp() const;
			glm::vec3 GetPosition() const;
			glm::vec3 GetTarget() const;

			static Camera* GetInstance();

			// Setters
			void SetPosition(glm::vec2 pos2d);
			void SetPosition(float x, float y);
			bool SetPositionSmooth(const TransformComponent& transform);
			void SetPositionSmooth(const float x, const float y, const float zoom = 16.0f, const float speed = 2.5f);
			void SetViewportSize(const glm::vec2 size);
			void SetZoom(float value);

			void Init();
			void ZoomControls();
			void Follow(const TransformComponent& transform, const float speed = 2.5f);
			void Exit();

			bool _doFollow;
		};

	}
}
