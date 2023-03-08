/*******************************************************************************
filename    Components.h
author(s)   Neo Jian Sheng, Kevin, neo.j@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "../framework.h"
#include "../System/Macro.h"
#include "../Serializer/RapidJsonBase.h"

namespace Engine
{
    namespace ECS
    {
        void Update();

        namespace Components
        {
            class IComponent
            {
            public:
                virtual ~IComponent() = default;
                virtual std::string GetComponentName() = 0;
                // ToEditor() does nothing if no override function is written
                virtual void ToEditor() {}
                void PrintDeserializeSuccessMessage()
                {
                    std::cout
                        << "    '" << GetComponentName() << "' successfully deserialized."
                        << std::endl;
                }
            };

            enum class EditOperation
            {
                Nil = 0,
                Translate,
                Rotate,
                Scale
            };
  
        }
    }
}

