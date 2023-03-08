/*!
@file	 Camera.cpp
@author  Edwin Teoh Yan-jie, t.yanjieedwin@digipen.edu, 100% Code Contribution
@date    20/09/2020
@brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.

*//*__________________________________________________________________________*/

#include "Camera.h"
#include "../Clock/Clock.h"
#include "../Input/Input.h"
#include "../Editor/PE_Editor.h"
#include "../GameLogic/GameLogic.h"

//#define PERSPECTIVECAM

namespace Engine
{
    namespace Graphics
    {
        Camera* Camera::_instance;

        glm::mat4 Camera::CalculateViewMatrix()
        {
            return glm::lookAt(_position, _position + _front, _up);
        }

        glm::mat4 Camera::CalculateOrthMatrix()
        {
            return glm::ortho(
                -static_cast<float>(_viewportWidth) / (50.0f + _zoomAmount),
                static_cast<float>(_viewportWidth) / (50.0f + _zoomAmount),
                -static_cast<float>(_viewportHeight) / (50.0f + _zoomAmount),
                static_cast<float>(_viewportHeight) / (50.0f + _zoomAmount),
                _nearPlane, _farPlane);
        }

        glm::mat4 Camera::CalculatePespMatrix()
        {
            return glm::perspective(
                glm::radians(_vertFOV),
                static_cast<float>((Window::_initialWidth) / Window::_initialHeight),
                _nearPlane, _farPlane);
        }

        void Camera::InternalUpdate()
        {
            _view = CalculateViewMatrix();
            _projection = CalculateOrthMatrix();

            if (_zoomed)
            {
//#ifdef PERSPECTIVECAM
//                _projection = CalculatePespMatrix();
//#else
//                _projection = CalculateOrthMatrix();
//#endif				
                _zoomed = GL_FALSE;
            }
        }

        // Constructor for the camera
        // Takes the position of the camera and the window and sets its internal position vector
        Camera::Camera() :
            _position{ glm::vec3{0.0f, 0.0f, 3.0f} },
            _worldup{ glm::vec3(0.0f, 1.0f, 0.0f) },
            _front{ glm::vec3(0.0f, 0.0f, -1.0f) },
            _zoomed{ GL_FALSE },
            _viewportWidth{ Window::_initialWidth },
            _viewportHeight{ Window::_initialHeight },
            _nearPlane{ 0.1f },
            _farPlane{ 15.0f },
            _vertFOV{ 885.0f }
        {
            _aspect = static_cast<float>(_viewportWidth) / static_cast<float>(_viewportHeight);
            InternalUpdate();
        }

        glm::mat4 Camera::GetViewMatrix() { return _view; }
        glm::mat4 Camera::GetProjMatrix() { return _projection; }

        GLint Camera::GetViewportWidth() const { return _viewportWidth; }
        GLint Camera::GetViewportHeight() const { return _viewportHeight; }

        float Camera::GetNearDist() const { return _nearPlane; }
        float Camera::GetFarDist() const { return _farPlane; }
        float Camera::GetAspect() const { return _aspect; }
        float Camera::GetZoom() const { return _zoomAmount; }

        Vec2D Camera::GetPositionVec2() const
        {
            return { _position.x,_position.y };
        }

        float Camera::GetFOV() const { return _vertFOV; }

        glm::vec3 Camera::GetUp() const { return _up; }
        glm::vec3 Camera::GetPosition() const { return _position; }

        glm::vec3 Camera::GetTarget() const
        {
            glm::vec3 target{ _position.x, _position.y, 0 };
            return target;
        }

        Camera* Camera::GetInstance()
        {
            if (!_instance)
                _instance = new Camera();
            return _instance;
        }

        void Camera::SetPosition(glm::vec2 pos2d)
        {
            _position = glm::vec3(pos2d.x, pos2d.y, _position.z);
        }

        void Camera::SetPosition(float x, float y)
        {
            _position = glm::vec3(x, y, _position.z);
        }

        void Camera::SetZoom(float value)
        {
            _zoomed = GL_TRUE;
            _zoomAmount = value;
        }

        void Camera::Init()
        {
#ifdef PERSPECTIVECAM
            _projection = CalculatePespMatrix();
#else
            _projection = CalculateOrthMatrix();
#endif

            _right = glm::normalize(glm::cross(_front, _worldup));
            _up = glm::normalize(glm::cross(_right, _front));

            _view = CalculateViewMatrix();
        }

        // Call this function to enable zoom on the camera
        void Camera::ZoomControls()
        {
            if (GameLogic::_gameIsPaused)
            {
                return;
            }

            static bool dragging = false;
            static double c_x = 0.0f, c_y = 0.0f;

            if (InputSystem::GetInstance()->MouseButtonIsPressed(GLFW_MOUSE_BUTTON_RIGHT))
            {
                if (c_x > p_Editor->SceneMinX() && c_x < p_Editor->SceneMaxX() &&
                    c_y > p_Editor->SceneMinY() && c_y < p_Editor->SceneMaxY())
                {
                    dragging = true;
                }
            }
            else if (dragging && InputSystem::GetInstance()->MouseButtonIsHeld(GLFW_MOUSE_BUTTON_RIGHT))
            {
                double new_x, new_y;
                glfwGetCursorPos(Window::_pWindow, &new_x, &new_y);
                _position.x -= static_cast<float>(new_x - c_x) * 0.08f;
                _position.y += static_cast<float>(new_y - c_y) * 0.08f;
            }
            else if (dragging && InputSystem::GetInstance()->MouseButtonIsReleased(GLFW_MOUSE_BUTTON_RIGHT))
            {
                dragging = false;
            }

            glfwGetCursorPos(Window::_pWindow, &c_x, &c_y);

            if (c_x > p_Editor->SceneMinX() && c_x < p_Editor->SceneMaxX() &&
                c_y > p_Editor->SceneMinY() && c_y < p_Editor->SceneMaxY())
            {
                if (InputSystem::GetInstance()->_mouseScrollStatus > 0)
                {
                    _zoomAmount += 4.0f;
                    _zoomed = GL_TRUE;
                    //std::cout << _zoomAmount << "Add" << std::endl;
                }
                else if (InputSystem::GetInstance()->_mouseScrollStatus < 0)
                {
                    _zoomAmount = (_zoomAmount - 4.0f) > -45.0f ? _zoomAmount - 4.0f : -45.0f;
                    _zoomed = GL_TRUE;
                    //std::cout << _zoomAmount << "Min" << std::endl;
                }
            }

            _view = CalculateViewMatrix();

            InternalUpdate();
        }

        // Insert a transform component to lock the camera to the specific transform
        void Camera::Follow(const TransformComponent& transform, const float speed)
        {
            Vec2D offset{ 4.0f, 5.0f };

            //_position = glm::vec3(transform._position._x, transform._position._y + 5.0f, _position.z);
            glm::vec3 track;
            if (transform._scale._x > 0)
            {
                track = glm::vec3(transform._position._x + offset._x, transform._position._y + offset._y, _position.z);
            }
            else
            {
                track = glm::vec3(transform._position._x - offset._x, transform._position._y + offset._y, _position.z);
            }

            _position.x += (track.x - _position.x) * p_Clock.GetGDT() * speed;
            _position.y += (track.y - _position.y) * p_Clock.GetGDT() * speed * 2;

            //if ((transform._position._y < (_prevPos.y + 0.1)) /*|| (_position.y > (_prevPos.y - 1))*/)
            //{
            //	// Shift
            //	//std::cout << "Shift" << std::endl;
            //	_position = glm::vec3(transform._position._x, transform._position._y + 0.3, _position.z);
            //	_prevPos = _position;
            //}


            //_position = glm::vec3(transform._position._x, transform._position._y + 0.3, _position.z);

            _prevPos = _position;

            //_prevPos += (_position - _prevPos) * p_Clock.GetGDT() * 0.1f;

            _view = CalculateViewMatrix();
        }

        bool Camera::SetPositionSmooth(const TransformComponent& transform)
        {
            float speed = 10.0f;

            glm::vec3 track = glm::vec3(transform._position._x, transform._position._y, _position.z);
            glm::vec2 diff(track.x - _position.x, track.y - _position.y);

            const float margin = 0.1f;
            if ((abs(diff.x) > margin) || (abs(diff.y) > margin))
            {
                _position.x += diff.x * p_Clock.GetGDT() * speed;
                _position.y += diff.y * p_Clock.GetGDT() * speed;
                _prevPos = _position;
                _view = CalculateViewMatrix();

                return true;
            }

            return false;
        }

        void Camera::SetPositionSmooth(const float x, const float y, const float zoom, const float speed)
        {
            glm::vec3 track = glm::vec3(x, y, _position.z);
            glm::vec2 diff(track.x - _position.x, track.y - _position.y);

            const float margin = 0.1f;
            if ((abs(diff.x) > margin) || (abs(diff.y) > margin))
            {
                _position.x += diff.x * p_Clock.GetGDT() * speed;
                _position.y += diff.y * p_Clock.GetGDT() * speed;
                _prevPos = _position;
                _view = CalculateViewMatrix();
            }

            float zoom_diff(zoom - _zoomAmount);
            if (_zoomAmount != zoom)
            {
                _zoomAmount += zoom_diff * p_Clock.GetGDT() * speed;
            }
        }

        void Camera::SetViewportSize(const glm::vec2 size)
        {
            _viewportWidth = (GLint)size.x;
            _viewportHeight = (GLint)size.y;
        }

        void Camera::Exit()
        {
            delete _instance;
        }
    }
}