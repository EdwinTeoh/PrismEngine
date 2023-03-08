#pragma once
#include "../Animation/Animation.h"

using Animator = ::Engine::Animation::Animator;
using AnimClip = ::Engine::Animation::AnimationClip;

enum class JeffState
{
    j_Idle = 0,
    j_Walk,
    j_Crawl,
    j_Climb,
    j_Float,
    j_Squish,
    j_Unsquish,
    j_Hurt,
    j_Shock,
    j_Fall,
    j_Dead
};
extern JeffState sJeffState;

void ChangeState_Jeff(JeffState newState, bool play = true);
