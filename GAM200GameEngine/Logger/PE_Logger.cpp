/*******************************************************************************
filename    PE_Logger.cpp
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        November 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "PE_Logger.h"
#include "../Clock/Clock.h"
#include <cassert>
#include <fstream>

#define COUT_STRING(str) \
            std::cout << str << std::endl;

namespace fs = std::filesystem;

namespace Engine
{
    namespace PE_Logger
    {
        D_Logger* D_Logger::_instance;

        D_Logger::D_Logger() :
            _originalBuffer{ std::cout.rdbuf(_customBuffer.rdbuf()) },
            _outputPath{ "../dlog" },
            _outputFileName{ "dlog-output.txt" },
            _doSaveLog{ false }
        {
            // std::put_time() causing memory leak! Not using this for now.
#if 0
            // Refer to https://en.cppreference.com/w/cpp/io/manip/put_time for format codes
            // Current format is YYYYMMDD-hhmmss
            std::time_t sys_now = std::time(nullptr);
            std::tm sys_now_tm = *std::localtime(&sys_now);
            // The filename includes the current day and time
            std::stringstream newFilename;
            newFilename << "dlog-output-" << std::put_time(&sys_now_tm, "%Y%m%d-%H%M%S") << ".txt";
            _outputFileName = newFilename.str();
#endif
            struct tm buf;
            time_t t = time(nullptr);
            localtime_s(&buf, &t);

            std::string t_year, t_month, t_day, t_hour, t_minute, t_second;
            t_year      = std::to_string(buf.tm_year + 1900);
            t_month     = buf.tm_mon < 10 ? "0" + std::to_string(buf.tm_mon + 1) : std::to_string(buf.tm_mon + 1);
            t_day       = buf.tm_mday < 10 ? "0" + std::to_string(buf.tm_mday) : std::to_string(buf.tm_mday);
            t_hour      = buf.tm_hour < 10 ? "0" + std::to_string(buf.tm_hour) : std::to_string(buf.tm_hour);
            t_minute    = buf.tm_min < 10 ? "0" + std::to_string(buf.tm_min) : std::to_string(buf.tm_min);
            t_second    = buf.tm_sec < 10 ? "0" + std::to_string(buf.tm_sec % 60) : std::to_string(buf.tm_sec % 60);

            _outputFileName = "dlog-output-" + t_year + t_month + t_day + "-" + t_hour + t_minute + t_second + ".txt";
        }

        void D_Logger::Init()
        {
            //bool isDirectoryValid = fs::is_directory(_outputPath);
            //if (!isDirectoryValid)
            //{
            //    COUT_STRING("Directory not found! Creating folder...");
            //    fs::create_directory(_outputPath);
            //}
        }

        void D_Logger::Update()
        {

        }

        void D_Logger::Exit()
        {
            if (_doSaveLog)
            {
                std::ofstream temp(GetOutputFilePath());
                temp << _customBuffer.rdbuf();
                temp.close();
            }

            std::cout.rdbuf(_originalBuffer);

            delete _instance;
        }

        D_Logger* D_Logger::GetInstance()
        {
            if (!_instance)
            {
                _instance = new D_Logger;
            }

            return _instance;
        }

        std::string D_Logger::GetLog_String()
        {
            return _customBuffer.str();
        }

        fs::path D_Logger::GetOutputFilePath()
        {
            fs::path tempPath{ _outputPath };
            tempPath /= _outputFileName;
            return tempPath;
        }

        std::string D_Logger::GetOutputFileName()
        {
            return _outputFileName;
        }

        void D_Logger::SetOutputFileName(std::string newName)
        {
            _outputFileName = newName;
        }
    }
}