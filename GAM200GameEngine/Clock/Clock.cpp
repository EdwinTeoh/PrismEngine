/* Start Header ************************************************************************/
/*!
\file Clock.cpp
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 04, 2020
\brief
    This file contains the definition for Clock class

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Clock.h"

namespace Utility
{
    Clock* Clock::s_clock = 0;

    Clock::Clock()
    {
        G_Start();
        for (int i = 0; i < _startTimes.size(); ++i)
            Start(static_cast<SystemCode>(i));
        for (int i = 0; i < _dts.size(); ++i)
            _dts[i] = 0.016f;
        _gDt = (float)(1.0 / 60.0);
    }

    void Clock::G_RuntimeStart()
    {
        _gRuntimeStart = clock();
    }

    void Clock::G_Start()
    {
        _gStart = clock();
    }

    void Clock::G_Update()
    {
        _totalRuntime = (float)(clock() - _gRuntimeStart) / 1000.0f;
        _gDt = (float)(clock() - _gStart);
        if (_gDt > 0)
            _fps = 1000.0f / _gDt;
        _gDt = 1.0f / _fps;
    }

    float Clock::GetGDT()
    {
        return _gDt;
    }

    Clock::sys_clock::time_point Clock::GetStartTime(SystemCode code)
    {
        return _startTimes[static_cast<int>(code)];
    }

    time_t Clock::GetSysClockNow()
	{
		return sys_clock::to_time_t(sys_clock::now());
	}

    void Clock::Start(SystemCode code)
    {
        if (_startTimes.size())
            _startTimes[static_cast<int>(code)] = sys_clock::now();
    }

    void Clock::SetDT(SystemCode code, float dt)
    {
        if (_dts.size())
            _dts[static_cast <int>(code)] = dt;
    }

    void Clock::Update(SystemCode code)
    {
        _end = sys_clock::now();
        _start = GetStartTime(code);
        _elapsed_seconds = _end - _start;
        SetDT(code, _elapsed_seconds.count() / 1000.0f);
    }

    float Clock::GetDT(SystemCode code)
    {
        return _dts[static_cast<int>(code)];
    }

}