#include "NPC_States.h"
#include "../Animation/Animation.h"

#include <iostream>

using Animator = ::Engine::Animation::Animator;
using AnimClip = ::Engine::Animation::AnimationClip;

void ChangeState_Pirate(PirateState& currentState, PirateState newState, size_t index, bool play)
{
    if (play)
        Animator::_animatorCont[index].Play();
    else
        Animator::_animatorCont[index].Pause();

    if (newState == currentState)
        return;

    currentState = newState;
    //std::cout << " New state Pirate: " << (int)sPirateState << std::endl;
    Animator::_animatorCont[index].SetCurrentClip(AnimClip::_pirateClips[static_cast<int>(newState)]);
}

MarkState sMarkState = MarkState::m_IDLE;

void ChangeState_Mark(MarkState newState)
{
    if (newState == sMarkState)
    {
        return;
    }

    sMarkState = newState;
    //std::cout << "Mark: " << (int)sMarkState << std::endl;
    Animator::_animatorCont[1].SetCurrentClip(AnimClip::_mClips[static_cast<int>(newState)]);
}