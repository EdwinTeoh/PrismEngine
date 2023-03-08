#include "Player_State.h"
#include "LogicScripts/Player.h"

JeffState sJeffState = JeffState::j_Idle;

void ChangeState_Jeff(JeffState newState, bool play)
{
    Engine::EntityID id = Engine::Player::GetInstance()->GetOwner();

    if (play)
        Animator::_animatorCont[id].Play();
    else
        Animator::_animatorCont[id].Pause();

    if (newState == sJeffState)
        return;

    sJeffState = newState;
    Animator::_animatorCont[id].SetCurrentClip(AnimClip::_jClips[static_cast<int>(newState)]);
}