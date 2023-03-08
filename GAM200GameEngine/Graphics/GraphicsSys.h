/*!
@file    GraphicsSys.h
@author  Edwin Teoh Yan-jie, t.yanjieedwin@digipen.edu, 100% Code Contribution
@date    16/09/2020
@brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.

*//*__________________________________________________________________________*/
#pragma once

#define p_Graphics Engine::Graphics::GraphicsSystem::GetInstance()

namespace Engine
{
    namespace Graphics
    {
        class GraphicsSystem
        {
            static GraphicsSystem* _instance;

        public:
            static GraphicsSystem* GetInstance();

            void Init();
            void Update();
            void Exit();
        };
    }
}