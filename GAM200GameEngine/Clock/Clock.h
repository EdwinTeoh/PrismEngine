/* Start Header ************************************************************************/
/*!
\file Collision.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the definition for the Collision system

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <chrono>
#include <array>

namespace Utility
{
#define totalNumOfSystems 5
    enum class SystemCode
    {
        PHYSICS,
        COLLISION,
        GRAPHICS,
        AI,
        LOGIC
    };

    class Clock
    {
        typedef std::chrono::system_clock sys_clock;
        typedef std::chrono::duration<float, std::milli> durr;

        static Clock* s_clock;

        clock_t _gStart;
        clock_t _gRuntimeStart;
        float _gDt;

        sys_clock::time_point _start;
        sys_clock::time_point _end;
        durr _elapsed_seconds;
    public:
        std::array<sys_clock::time_point, totalNumOfSystems> _startTimes;
        std::array<float, totalNumOfSystems> _dts;

        float _totalRuntime;
        float _fps;
        float _dt;


        Clock();
        ~Clock() = default;
        // Global
        void G_RuntimeStart();
        void G_Start();
        void G_Update();

        // Systems 
        void Init();
        void Start(SystemCode code);
        void Update(SystemCode code);

        void SetDT(SystemCode code, float dt);

        sys_clock::time_point GetStartTime(SystemCode code);
        time_t GetSysClockNow();
        float GetDT(SystemCode code);

        float GetGDT();

        static Clock& GetInstance()
        {
            static Clock instance;
            return instance;
        }
    };
}

#define p_Clock Utility::Clock::GetInstance()

#endif // _CLOCK_H_