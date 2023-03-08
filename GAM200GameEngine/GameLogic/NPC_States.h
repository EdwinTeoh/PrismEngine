#pragma once

enum class PirateState
{
    p_IDLE = 0,
    p_WALK,
    p_CHASE,
    p_STRUGGLE,
    p_ALERTED,
    p_CONFUSED,
    p_HURT,
    p_ATTACK,
    p_STUNNED
};

void ChangeState_Pirate(PirateState& currentState, PirateState newState, size_t index, bool play = true);

enum class MarkState
{
    m_IDLE = 0,
    m_EXCITED,
    m_CELEBRATE
};
extern MarkState sMarkState;

void ChangeState_Mark(MarkState newState);