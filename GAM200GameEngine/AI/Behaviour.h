/* Start Header ************************************************************************/
/*!
\file Behaviour.h
\author Nicolette Chan, c.munlengnicolette
\par c.munlengnicolette\@digipen.edu
\date Oct 30, 2020
\brief
    This file contains the declaration for the different types of state
    and enemy types.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
namespace Engine
{
    namespace AI
    {
        enum struct Behaviour
        {
            // Starting state
            STATE_IDLE = 0,
            // Patrol a timed path
            STATE_PATROL,
            // Chase target
            STATE_CHASE,
            // KnockBack
            STATE_HIT,
            // Play death animation
            STATE_DEAD
        };

        enum struct Target
        {
            PLAYER,
            ENEMY,
            VACUUMOBJECT,
            NONE
        };
    }
}
