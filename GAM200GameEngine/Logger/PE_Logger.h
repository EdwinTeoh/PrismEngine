/*******************************************************************************
filename    PE_Logger.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        November 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include "../framework.h"
#include <filesystem>
#include <sstream>

namespace Engine
{
    extern std::stringstream* _externBuffer;

    namespace PE_Logger
    {
        class D_Logger
        {
            static D_Logger* _instance;
            std::streambuf* _originalBuffer;
            std::stringstream _customBuffer;
            std::filesystem::path _outputPath;
            std::string _outputFileName;
        public:
            bool _doSaveLog;

            D_Logger();
            ~D_Logger() = default;

            static D_Logger* GetInstance();
            std::string GetLog_String();
            std::filesystem::path GetOutputFilePath();
            std::string GetOutputFileName();
            void SetOutputFileName(std::string newName);
            
            void Init();
            void Update();
            void Exit();
        };
    }
}