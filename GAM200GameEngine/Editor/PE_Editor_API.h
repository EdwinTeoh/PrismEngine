/*******************************************************************************
filename    PE_Editor_API.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        January, 2021
brief       ImGui can be slightly tedious and complicated, I've set up some APIs
            that should make it easier to implement.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include <string>

void ToEditor_Text(std::string content, bool sameLine = false, float offset = 0.0f);