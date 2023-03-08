/*******************************************************************************
filename    Animation.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        October 23, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once

#include <array>
#include <string>
#include <vector>

namespace Engine
{
    namespace Animation
    {
        void Init();
        void Update();
        void Exit();

        class AnimationClip
        {
            float _clipSpeed;
        public:
            size_t _numOfFrames;
            size_t _numOfSpritesRow;    // HOW MANY SPRITES IN 1 ROW
            float _rowOffset;
            size_t _numOfSpritesCol;    // HOW MANY SPRITES IN 1 COLUMN
            float _columnOffset;
            size_t _currentFrame;
            //float _fCurrentFrame;

            int _rangeStart, _rangeEnd;

            bool _looping;

            AnimationClip() = default;
            // Custom Constructor
            AnimationClip(const size_t& numOfFrames, const size_t& rowCount, const size_t& columnCount, const float& clipSpeed = 1,
                int start = 0, int end = -1, bool looping = true);
            AnimationClip& operator=(const AnimationClip&) = default;
            ~AnimationClip() = default;

            float GetCurrentFrameX();
            float GetCurrentFrameY();
            float GetClipSpeed();
            void ChangeClipSpeed(const float& newSpeed);
            bool Compare(AnimationClip& otherClip);

            static std::vector<AnimationClip> _clipCont;
            static std::array<AnimationClip, 11> _jClips;
            static std::vector<AnimationClip> _pirateClips;
            static std::array<AnimationClip, 3> _mClips;
        };

        class Animator
        {
            AnimationClip* _currentClip;
        public:
            float _currentFrame;
            bool _isPlaying;

            Animator() = default;
            Animator& operator=(const Animator&) = default;
            ~Animator() = default;

            // Every frame update the clip
            void Play();
            void Pause();
            void Restart();
            AnimationClip* GetCurrentClip();
            void SetCurrentClip(AnimationClip& newClip);
            const float GetClipDuration();

            enum AnimatorEnum
            {
                SHOCK_TILES = 6,
                HEALTH_STATION
            };

            static std::vector<Animator> _animatorCont;
        };
    }
}