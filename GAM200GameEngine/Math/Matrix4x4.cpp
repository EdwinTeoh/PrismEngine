/* Start Header ************************************************************************/
/*!
\file Matrix4X4.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Sep 23, 2020
\brief
    This file contains the implementation for Matrix4X4

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Matrix4x4.h"
#include <cmath>

#define EPSILON		0.0001f
#define PI			3.14159265358f

namespace Engine
{
    namespace Math
    {
        Matrix4x4::Matrix4x4()
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    _m[i][j] = 0.0f;
        }

        // ctor (const float* ptr)
        Matrix4x4::Matrix4x4(const float* pArr)
        {
            //Initiliaze the m[16] array
            for (int i = 0; i < 4; ++i)
                for (int k = 0; k < 4; ++k)
                    _m[i][k] = *(pArr + i + k);
        }

        // ctor matrix4x4
        Matrix4x4::Matrix4x4(float _00, float _01, float _02, float _03,
            float _10, float _11, float _12, float _13,
            float _20, float _21, float _22, float _23,
            float _30, float _31, float _32, float _33)
        {
            _m[0][0] = _00;
            _m[0][1] = _01;
            _m[0][2] = _02;
            _m[0][3] = _03;
            _m[1][0] = _10;
            _m[1][1] = _11;
            _m[1][2] = _12;
            _m[1][3] = _13;
            _m[2][0] = _20;
            _m[2][1] = _21;
            _m[2][2] = _22;
            _m[2][3] = _23;
            _m[3][0] = _30;
            _m[3][1] = _31;
            _m[3][2] = _32;
            _m[3][3] = _33;
        }

        // ctor matrix(glm)
        Matrix4x4::Matrix4x4(const glm::mat4& rhs)
        {
            _m[0][0] = rhs[0][0];
            _m[0][1] = rhs[0][1];
            _m[0][2] = rhs[0][2];
            _m[0][3] = rhs[0][3];
            _m[1][0] = rhs[1][0];
            _m[1][1] = rhs[1][1];
            _m[1][2] = rhs[1][2];
            _m[1][3] = rhs[1][3];
            _m[2][0] = rhs[2][0];
            _m[2][1] = rhs[2][1];
            _m[2][2] = rhs[2][2];
            _m[2][3] = rhs[2][3];
            _m[3][0] = rhs[3][0];
            _m[3][1] = rhs[3][1];
            _m[3][2] = rhs[3][2];
            _m[3][3] = rhs[3][3];
        }

        // copy ctor
        Matrix4x4::Matrix4x4(const Matrix4x4& rhs)
        {
            for (int i = 0; i < 4; ++i)
                for (int k = 0; k < 4; ++k)
                    _m[i][k] = rhs._m[k][i];
        }

        // matrix (float)
        Matrix4x4::Matrix4x4(float x)
        {
            _m[0][0] = x;
            _m[0][1] = 0.0f;
            _m[0][2] = 0.0f;
            _m[0][3] = 0.0f;
            _m[1][0] = 0.0f;
            _m[1][1] = x;
            _m[1][2] = 0.0f;
            _m[1][3] = 0.0f;
            _m[2][0] = 0.0f;
            _m[2][1] = 0.0f;
            _m[2][2] = x;
            _m[2][3] = 0.0f;
            _m[3][0] = 0.0f;
            _m[3][1] = 0.0f;
            _m[3][2] = 0.0f;
            _m[3][3] = x;
        }

        // operator=
        Matrix4x4& Matrix4x4::operator=(const Matrix4x4& rhs)
        {
            for (int i = 0; i < 4; ++i)
                for (int k = 0; k < 4; ++k)
                    _m[i][k] = rhs._m[i][k];

            return *this;
        }

        // operator*=
        Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs)
        {
            // Multiply the first row
            _m[0][0] = _m[0][0] * rhs._m[0][0] + _m[0][1] * rhs._m[1][0] + _m[0][2] * rhs._m[2][0] + _m[0][3] * rhs._m[3][0];
            _m[0][1] = _m[0][0] * rhs._m[0][1] + _m[0][1] * rhs._m[1][1] + _m[0][2] * rhs._m[2][1] + _m[0][3] * rhs._m[3][1];
            _m[0][2] = _m[0][0] * rhs._m[0][2] + _m[0][1] * rhs._m[1][2] + _m[0][2] * rhs._m[2][2] + _m[0][3] * rhs._m[3][2];
            _m[0][3] = _m[0][0] * rhs._m[0][3] + _m[0][1] * rhs._m[1][3] + _m[0][2] * rhs._m[2][3] + _m[0][3] * rhs._m[3][3];
            // Multiply the second row
            _m[1][0] = _m[1][0] * rhs._m[0][0] + _m[1][1] * rhs._m[1][0] + _m[1][2] * rhs._m[2][0] + _m[1][3] * rhs._m[3][0];
            _m[1][1] = _m[1][0] * rhs._m[0][1] + _m[1][1] * rhs._m[1][1] + _m[1][2] * rhs._m[2][1] + _m[1][3] * rhs._m[3][1];
            _m[1][2] = _m[1][0] * rhs._m[0][2] + _m[1][1] * rhs._m[1][2] + _m[1][2] * rhs._m[2][2] + _m[1][3] * rhs._m[3][2];
            _m[1][3] = _m[1][0] * rhs._m[0][3] + _m[1][1] * rhs._m[1][3] + _m[1][2] * rhs._m[2][3] + _m[1][3] * rhs._m[3][3];
            // Multiply the third row 
            _m[2][0] = _m[2][0] * rhs._m[0][0] + _m[2][1] * rhs._m[1][0] + _m[2][2] * rhs._m[2][0] + _m[2][3] * rhs._m[3][0];
            _m[2][1] = _m[2][0] * rhs._m[0][1] + _m[2][1] * rhs._m[1][1] + _m[2][2] * rhs._m[2][1] + _m[2][3] * rhs._m[3][1];
            _m[2][2] = _m[2][0] * rhs._m[0][2] + _m[2][1] * rhs._m[1][2] + _m[2][2] * rhs._m[2][2] + _m[2][3] * rhs._m[3][2];
            _m[2][3] = _m[2][0] * rhs._m[0][3] + _m[2][1] * rhs._m[1][3] + _m[2][2] * rhs._m[2][3] + _m[2][3] * rhs._m[3][3];
            // Multiply the fourth row
            _m[3][0] = _m[3][0] * rhs._m[0][0] + _m[3][1] * rhs._m[1][0] + _m[3][2] * rhs._m[2][0] + _m[3][3] * rhs._m[3][0];
            _m[3][1] = _m[3][0] * rhs._m[0][1] + _m[3][1] * rhs._m[1][1] + _m[3][2] * rhs._m[2][1] + _m[3][3] * rhs._m[3][1];
            _m[3][2] = _m[3][0] * rhs._m[0][2] + _m[3][1] * rhs._m[1][2] + _m[3][2] * rhs._m[2][2] + _m[3][3] * rhs._m[3][2];
            _m[3][3] = _m[3][0] * rhs._m[0][3] + _m[3][1] * rhs._m[1][3] + _m[3][2] * rhs._m[2][3] + _m[3][3] * rhs._m[3][3];

            return *this;
        }

        glm::mat4 Matrix4x4::ConvertMtx44toGlmMat4()
        {
            glm::mat4 tmp;

            tmp[0][0] = _m[0][0];
            tmp[0][1] = _m[0][1];
            tmp[0][2] = _m[0][2];
            tmp[0][3] = _m[0][3];
            tmp[1][0] = _m[1][0];
            tmp[1][1] = _m[1][1];
            tmp[1][2] = _m[1][2];
            tmp[1][3] = _m[1][3];
            tmp[2][0] = _m[2][0];
            tmp[2][1] = _m[2][1];
            tmp[2][2] = _m[2][2];
            tmp[2][3] = _m[2][3];
            tmp[3][0] = _m[3][0];
            tmp[3][1] = _m[3][1];
            tmp[3][2] = _m[3][2];
            tmp[3][3] = _m[3][3];

            glm::mat4 tmp2 = glm::transpose(tmp);

            return tmp2;
        }

        // operator*
        Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs)
        {
            Matrix4x4 temp; //Create a temp Matrix4x4 to return
                            // Multiply the first row
            temp._m[0][0] = lhs._m[0][0] * rhs._m[0][0] + lhs._m[0][1] * rhs._m[1][0] + lhs._m[0][2] * rhs._m[2][0] + lhs._m[0][3] * rhs._m[3][0];
            temp._m[0][1] = lhs._m[0][0] * rhs._m[0][1] + lhs._m[0][1] * rhs._m[1][1] + lhs._m[0][2] * rhs._m[2][1] + lhs._m[0][3] * rhs._m[3][1];
            temp._m[0][2] = lhs._m[0][0] * rhs._m[0][2] + lhs._m[0][1] * rhs._m[1][2] + lhs._m[0][2] * rhs._m[2][2] + lhs._m[0][3] * rhs._m[3][2];
            temp._m[0][3] = lhs._m[0][0] * rhs._m[0][3] + lhs._m[0][1] * rhs._m[1][3] + lhs._m[0][2] * rhs._m[2][3] + lhs._m[0][3] * rhs._m[3][3];
            // Mu_ltiply the second row
            temp._m[1][0] = lhs._m[1][0] * rhs._m[0][0] + lhs._m[1][1] * rhs._m[1][0] + lhs._m[1][2] * rhs._m[2][0] + lhs._m[1][3] * rhs._m[3][0];
            temp._m[1][1] = lhs._m[1][0] * rhs._m[0][1] + lhs._m[1][1] * rhs._m[1][1] + lhs._m[1][2] * rhs._m[2][1] + lhs._m[1][3] * rhs._m[3][1];
            temp._m[1][2] = lhs._m[1][0] * rhs._m[0][2] + lhs._m[1][1] * rhs._m[1][2] + lhs._m[1][2] * rhs._m[2][2] + lhs._m[1][3] * rhs._m[3][2];
            temp._m[1][3] = lhs._m[1][0] * rhs._m[0][3] + lhs._m[1][1] * rhs._m[1][3] + lhs._m[1][2] * rhs._m[2][3] + lhs._m[1][3] * rhs._m[3][3];
            // Mu_ltiply the third row
            temp._m[2][0] = lhs._m[2][0] * rhs._m[0][0] + lhs._m[2][1] * rhs._m[1][0] + lhs._m[2][2] * rhs._m[2][0] + lhs._m[2][3] * rhs._m[3][0];
            temp._m[2][1] = lhs._m[2][0] * rhs._m[0][1] + lhs._m[2][1] * rhs._m[1][1] + lhs._m[2][2] * rhs._m[2][1] + lhs._m[2][3] * rhs._m[3][1];
            temp._m[2][2] = lhs._m[2][0] * rhs._m[0][2] + lhs._m[2][1] * rhs._m[1][2] + lhs._m[2][2] * rhs._m[2][2] + lhs._m[2][3] * rhs._m[3][2];
            temp._m[2][3] = lhs._m[2][0] * rhs._m[0][3] + lhs._m[2][1] * rhs._m[1][3] + lhs._m[2][2] * rhs._m[2][3] + lhs._m[2][3] * rhs._m[3][3];
            // Mu_ltiply the fourth row
            temp._m[3][0] = lhs._m[3][0] * rhs._m[0][0] + lhs._m[3][1] * rhs._m[1][0] + lhs._m[3][2] * rhs._m[2][0] + lhs._m[3][3] * rhs._m[3][0];
            temp._m[3][1] = lhs._m[3][0] * rhs._m[0][1] + lhs._m[3][1] * rhs._m[1][1] + lhs._m[3][2] * rhs._m[2][1] + lhs._m[3][3] * rhs._m[3][1];
            temp._m[3][2] = lhs._m[3][0] * rhs._m[0][2] + lhs._m[3][1] * rhs._m[1][2] + lhs._m[3][2] * rhs._m[2][2] + lhs._m[3][3] * rhs._m[3][2];
            temp._m[3][3] = lhs._m[3][0] * rhs._m[0][3] + lhs._m[3][1] * rhs._m[1][3] + lhs._m[3][2] * rhs._m[2][3] + lhs._m[3][3] * rhs._m[3][3];

            return temp;
        }

        // operator==
        bool operator==(const Matrix4x4& lhs, const glm::mat4& rhs)
        {
            if (rhs[0][0] != lhs._m[0][0]) return false;
            if (rhs[0][1] != lhs._m[0][1]) return false;
            if (rhs[0][2] != lhs._m[0][2]) return false;
            if (rhs[0][3] != lhs._m[0][3]) return false;
            if (rhs[1][0] != lhs._m[1][0]) return false;
            if (rhs[1][1] != lhs._m[1][1]) return false;
            if (rhs[1][2] != lhs._m[1][2]) return false;
            if (rhs[1][3] != lhs._m[1][3]) return false;
            if (rhs[2][0] != lhs._m[2][0]) return false;
            if (rhs[2][1] != lhs._m[2][1]) return false;
            if (rhs[2][2] != lhs._m[2][2]) return false;
            if (rhs[2][3] != lhs._m[2][3]) return false;
            if (rhs[3][0] != lhs._m[3][0]) return false;
            if (rhs[3][1] != lhs._m[3][1]) return false;
            if (rhs[3][2] != lhs._m[3][2]) return false;
            if (rhs[3][3] != lhs._m[3][3]) return false;

            return true;
        }

        // operator*
        Vector2D operator*(const Matrix4x4& pMtx, const Vector2D& rhs)
        {
            Vector2D temp; //Create a temp Vector2D to return
                           //Homogenous 2D
            temp._x = rhs._x * pMtx._m[0][0] + rhs._y * pMtx._m[0][1] + pMtx._m[0][2];
            temp._y = rhs._x * pMtx._m[1][0] + rhs._y * pMtx._m[1][1] + pMtx._m[1][2];

            return temp;
        }

        // identity matrix
        void Mtx44Identity(Matrix4x4& pResult)
        {
            //Creating Identity matrix
            /*
            1 0 0 0
            0 1 0 0
            0 0 1 0
            0 0 0 1
            */
            pResult._m[0][0] = 1.f;
            pResult._m[0][1] = 0.f;
            pResult._m[0][2] = 0.f;
            pResult._m[0][3] = 0.f;

            pResult._m[1][0] = 0.f;
            pResult._m[1][1] = 1.f;
            pResult._m[1][2] = 0.f;
            pResult._m[1][3] = 0.f;

            pResult._m[2][0] = 0.f;
            pResult._m[2][1] = 0.f;
            pResult._m[2][2] = 1.f;
            pResult._m[2][3] = 0.f;

            pResult._m[3][0] = 0.f;
            pResult._m[3][1] = 0.f;
            pResult._m[3][2] = 0.f;
            pResult._m[3][3] = 1.f;
        }

        // 
        void Mtx44Translate(Matrix4x4& pResult, float x, float y)
        {
            pResult._m[0][0] = 1.0f;
            pResult._m[0][1] = 0.0f;
            pResult._m[0][2] = 0.0f; //Translating by x amount
            pResult._m[0][3] = x;

            pResult._m[1][0] = 0.0f;
            pResult._m[1][1] = 1.0f;
            pResult._m[1][2] = 0.0f; //Translating by y amount
            pResult._m[1][3] = y;

            pResult._m[2][0] = 0.0f;
            pResult._m[2][1] = 0.0f;
            pResult._m[2][2] = 1.0f;
            pResult._m[2][3] = 0.0f;

            pResult._m[3][0] = 0.0f;
            pResult._m[3][1] = 0.0f;
            pResult._m[3][2] = 0.0f;
            pResult._m[3][3] = 1.0f;
        }

        void Mtx44Scale(Matrix4x4& pResult, float x, float y)
        {
            pResult._m[0][0] = x; //Scaling by x amount
            pResult._m[0][1] = 0.0f;
            pResult._m[0][2] = 0.0f;
            pResult._m[0][3] = 0.0f;

            pResult._m[1][0] = 0.0f;
            pResult._m[1][1] = y; //Scaling by y amount
            pResult._m[1][2] = 0.0f;
            pResult._m[1][3] = 0.0f;

            pResult._m[2][0] = 0.0f;
            pResult._m[2][1] = 0.0f;
            pResult._m[2][2] = 1.0f; //Scaling by 1
            pResult._m[2][3] = 0.0f;

            pResult._m[3][0] = 0.0f;
            pResult._m[3][1] = 0.0f;
            pResult._m[3][2] = 0.0f;
            pResult._m[3][3] = 1.0f; //Scaling by 1
        }

        void Mtx44RotRad(Matrix4x4& pResult, float angle)
        {
            // Creating the Rotation matrix
            /*
            cos -sin 0 0
            sin  cos 0 0
            0    0   1 0
            0    0   0 1* transform->m_Rotate
            */
            pResult._m[0][0] = cos(angle);
            pResult._m[0][1] = -sin(angle);
            pResult._m[0][2] = 0.0f;
            pResult._m[0][3] = 0.0f;

            pResult._m[1][0] = sin(angle);
            pResult._m[1][1] = cos(angle);
            pResult._m[1][2] = 0.0f;
            pResult._m[1][3] = 0.0f;

            pResult._m[2][0] = 0.0f;
            pResult._m[2][1] = 0.0f;
            pResult._m[2][2] = 1.0f;
            pResult._m[2][3] = 0.0f;

            pResult._m[3][0] = 0.0f;
            pResult._m[3][1] = 0.0f;
            pResult._m[3][2] = 0.0f;
            pResult._m[3][3] = 1.0f;
        }

        void Mtx44RotDeg(Matrix4x4& pResult, float angle)
        {
            // Creating the Rotation matrix
            /*
            cos -sin 0 0
            sin  cos 0 0
            0    0   1 0
            0    0   0 1
            */
            float angleRad = angle * PI / 180.f; // Converting it to Radians
            pResult._m[0][0] = cos(angleRad);
            pResult._m[0][1] = -sin(angleRad);
            pResult._m[0][2] = 0.0f;
            pResult._m[0][3] = 0.0f;

            pResult._m[1][0] = sin(angleRad);
            pResult._m[1][1] = cos(angleRad);
            pResult._m[1][2] = 0.0f;
            pResult._m[1][3] = 0.0f;

            pResult._m[2][0] = 0.0f;
            pResult._m[2][1] = 0.0f;
            pResult._m[2][2] = 1.0f;
            pResult._m[2][3] = 0.0f;

            pResult._m[3][0] = 0.0f;
            pResult._m[3][1] = 0.0f;
            pResult._m[3][2] = 0.0f;
            pResult._m[3][3] = 1.0f;
        }

        void Mtx44Transpose(Matrix4x4& pResult, const Matrix4x4& pMtx)
        {
            //Swapping the rows and columns
            /*
            a b c t      a d g x
            d e f j -->  b e h y
            g h i k 	   c f i z
            x y z q      t j k q
            */
            pResult._m[0][1] = pMtx._m[1][0];
            pResult._m[1][0] = pMtx._m[0][1];
            pResult._m[0][3] = pMtx._m[3][0];
            pResult._m[3][0] = pMtx._m[0][3];
            pResult._m[0][0] = pMtx._m[0][0];

            pResult._m[2][0] = pMtx._m[0][2];
            pResult._m[0][2] = pMtx._m[2][0];
            pResult._m[2][3] = pMtx._m[3][2];
            pResult._m[3][2] = pMtx._m[2][3];
            pResult._m[1][1] = pMtx._m[1][1];

            pResult._m[2][1] = pMtx._m[1][2];
            pResult._m[1][2] = pMtx._m[2][1];
            pResult._m[2][2] = pMtx._m[2][2];

            pResult._m[3][1] = pMtx._m[1][3];
            pResult._m[1][3] = pMtx._m[3][1];
            pResult._m[3][3] = pMtx._m[3][3];
        }

        void Mtx44Inverse(Matrix4x4* pResult,
            float* determinant,
            const Matrix4x4& pMtx)
        {
            // Finding the determinant
            float FirstMinor = pMtx._m[0][0] * (pMtx._m[1][1] * pMtx._m[2][2] - pMtx._m[2][1] * pMtx._m[1][2]);
            float SecondMinor = -pMtx._m[0][1] * (pMtx._m[1][0] * pMtx._m[2][2] - pMtx._m[2][0] * pMtx._m[1][2]);
            float ThirdMinor = pMtx._m[0][2] * (pMtx._m[1][0] * pMtx._m[2][1] - pMtx._m[2][0] * pMtx._m[1][1]);
            *determinant = FirstMinor + SecondMinor + ThirdMinor;

            if (*determinant == 0) //if determinant is 0, Mtx is not invertible
                pResult = nullptr;
            else
            {
                Matrix4x4 temp;
                temp = pMtx; //To prevent overlapping/using of the same matrix
                             //Transpose the original matrix
                Mtx44Transpose(*pResult, temp);
                temp = *pResult;
                //Find the determinant of each of the 2x2 minor matrices
                pResult->_m[0][0] = temp._m[1][1] * temp._m[2][2] - temp._m[2][1] * temp._m[1][2];
                pResult->_m[0][1] = temp._m[1][0] * temp._m[2][2] - temp._m[2][0] * temp._m[1][2];
                pResult->_m[0][2] = temp._m[1][0] * temp._m[2][1] - temp._m[2][0] * temp._m[1][1];
                pResult->_m[1][0] = temp._m[0][1] * temp._m[2][2] - temp._m[2][1] * temp._m[0][2];
                pResult->_m[1][1] = temp._m[0][0] * temp._m[2][2] - temp._m[2][0] * temp._m[0][2];

                pResult->_m[1][2] = temp._m[0][0] * temp._m[2][1] - temp._m[2][0] * temp._m[0][1];

                pResult->_m[2][0] = temp._m[0][1] * temp._m[1][2] - temp._m[1][1] * temp._m[0][2];
                pResult->_m[2][1] = temp._m[0][0] * temp._m[1][2] - temp._m[1][0] * temp._m[0][2];
                pResult->_m[2][2] = temp._m[0][0] * temp._m[1][1] - temp._m[1][0] * temp._m[0][1];

                //Create the matrix of cofactors. Hence the inverse
                pResult->_m[0][0] /= *determinant;
                pResult->_m[0][1] /= -*determinant;
                pResult->_m[0][2] /= *determinant;

                pResult->_m[1][0] /= -*determinant;
                pResult->_m[1][1] /= *determinant;
                pResult->_m[1][2] /= -*determinant;

                pResult->_m[2][0] /= *determinant;
                pResult->_m[2][1] /= -*determinant;
                pResult->_m[2][2] /= *determinant;
            }
        }

        std::ostream& operator<<(std::ostream& os, const Matrix4x4& rhs)
        {
            return os << "[" << rhs._m[0][0] << " " << rhs._m[0][1] << " " << rhs._m[0][2] << " " << rhs._m[0][3] << "]\n"
                << "[" << rhs._m[1][0] << " " << rhs._m[1][1] << " " << rhs._m[1][2] << " " << rhs._m[1][3] << "]\n"
                << "[" << rhs._m[2][0] << " " << rhs._m[2][1] << " " << rhs._m[2][2] << " " << rhs._m[2][3] << "]\n"
                << "[" << rhs._m[3][0] << " " << rhs._m[3][1] << " " << rhs._m[3][2] << " " << rhs._m[3][3] << "]\n";
        }
    }
}