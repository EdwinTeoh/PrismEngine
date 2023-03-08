/*******************************************************************************
filename    Animation.cpp
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        October 23, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "Animation.h"
#include "../Clock/Clock.h"
#include "../GameLogic/LogicScripts/Player.h"
#include "../GameLogic/Player_State.h"
#include "../GameLogic/NPC_States.h"

#include <GLFW/glfw3.h>

namespace Engine
{
    namespace Animation
    {
        std::vector<Animator> Animator::_animatorCont;
        std::vector<AnimationClip> AnimationClip::_clipCont;
        std::array<AnimationClip, 11> AnimationClip::_jClips;
        std::vector<AnimationClip> AnimationClip::_pirateClips;
        std::array<AnimationClip, 3> AnimationClip::_mClips;

        void Init()
        {
            using antor = Animator;
            using aclip = AnimationClip;

            // Player
            size_t totalFrames = 48, row = 8, col = 6, i = 0;
            antor::_animatorCont.push_back(antor()); // 0
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 0, 3 };

            //aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 4, 7 };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 10.0f, 40, 47 };

            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 8, 9 };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 10, 11 };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 12, 13 };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 14, 15 };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 16, 18, false };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 19, 20 };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 21, 22 };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 24, 28, false };
            aclip::_jClips[i++] = aclip{ totalFrames, row, col, 6.0f, 32, 39, false };
            antor::_animatorCont.back().SetCurrentClip(aclip::_jClips[0]);
            antor::_animatorCont.back().Play();

            // Mark
            totalFrames = 24, row = 8, col = 3, i = 0;
            antor::_animatorCont.push_back(antor()); // 1
            aclip::_mClips[i++] = aclip{ totalFrames, row, col, 6.0f, 0, 5 };
            aclip::_mClips[i++] = aclip{ totalFrames, row, col, 6.0f, 8, 15 };
            aclip::_mClips[i++] = aclip{ totalFrames, row, col, 6.0f, 16, 19 };

            antor::_animatorCont.back().SetCurrentClip(aclip::_mClips[0]);
            antor::_animatorCont.back().Play();

            // Pirate
            totalFrames = 56, row = 8, col = 7, i = 0;
            int frames[]{ 4, 8, 8, 8, 5, 5, 2, 7, 8 };
            int sum = 0;
            std::vector<int> range;
            for (size_t f = 0; f < sizeof(frames) / sizeof(frames[0]); ++f)
            {
                //std::cout << sum << "/";
                range.push_back(sum);
                sum += frames[f];
                range.push_back(sum - 1);
                //std::cout << sum - 1 << std::endl;
            }
            antor::_animatorCont.push_back(antor()); // 2
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.back()._looping = false;
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.back()._looping = false;
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];
            aclip::_pirateClips.back()._looping = false;
            aclip::_pirateClips.push_back(aclip{ totalFrames, row, col, 6.0f });
            aclip::_pirateClips.back()._rangeStart = range[i++];
            aclip::_pirateClips.back()._rangeEnd = range[i++];

            antor::_animatorCont.back().SetCurrentClip(aclip::_pirateClips[1]);
            antor::_animatorCont.back().Play();

            antor::_animatorCont.push_back(antor()); // 3
            antor::_animatorCont.back().SetCurrentClip(aclip::_pirateClips[1]);
            antor::_animatorCont.back().Play();

            antor::_animatorCont.push_back(antor()); // 4
            antor::_animatorCont.back().SetCurrentClip(aclip::_pirateClips[1]);
            antor::_animatorCont.back().Play();

            antor::_animatorCont.push_back(antor()); // 5
            antor::_animatorCont.back().SetCurrentClip(aclip::_pirateClips[1]);
            antor::_animatorCont.back().Play();

            totalFrames = 3, row = 3, col = 1;
            aclip::_clipCont.push_back(aclip(totalFrames, row, col, 6.0f)); // 0

            totalFrames = 4, row = 4, col = 1;
            aclip::_clipCont.push_back(aclip(1, row, col, 6.0f, 0, 0)); // 1 - Shock Tiles
            aclip::_clipCont.push_back(aclip(4, row, col, 6.0f, 1, 3)); // 2

            aclip::_clipCont.push_back(aclip(8, 8, 1, 10.0f)); // 3

            aclip::_clipCont.push_back(aclip(10, 3, 4, 10.0f, 0, 9)); // 4
            aclip::_clipCont[4]._looping = false;

            antor::_animatorCont.push_back(antor()); // 6 - Shock Tiles
            antor::_animatorCont[6].SetCurrentClip(aclip::_clipCont[0]);
            antor::_animatorCont[6].Play();

            antor::_animatorCont.push_back(antor()); // 7 - Healing Station (Off)
            antor::_animatorCont[7].SetCurrentClip(aclip::_clipCont[1]);
            antor::_animatorCont[7].Play();

            antor::_animatorCont.push_back(antor()); // 8 - Healing Station (On)
            antor::_animatorCont[8].SetCurrentClip(aclip::_clipCont[3]);
            antor::_animatorCont[8].Play();

            antor::_animatorCont.push_back(antor()); // 9 - Explosion
            antor::_animatorCont[9].SetCurrentClip(aclip::_clipCont[4]);

            antor::_animatorCont.push_back(antor()); // 10 - Healing Station (Off)
            antor::_animatorCont[10].SetCurrentClip(aclip::_clipCont[2]);
            antor::_animatorCont[10].Play();

            antor::_animatorCont.push_back(antor()); // 11 - Healing Station (On)
            antor::_animatorCont[11].SetCurrentClip(aclip::_clipCont[3]);
            antor::_animatorCont[11].Play();
        }

        void Update()
        {
            for (Animator& animator : Animator::_animatorCont)
            {
                AnimationClip* clip = animator.GetCurrentClip();

                if (animator._isPlaying)
                {
                    animator._currentFrame += clip->GetClipSpeed() * p_Clock.GetGDT();
                    clip->_currentFrame = (int)(floorf(animator._currentFrame));
                    //clip->_fCurrentFrame += clip->GetClipSpeed()* p_Clock.GetGDT();
                    //clip->_currentFrame = static_cast<int>(floorf(clip->_fCurrentFrame));

                    if (clip->_currentFrame >= clip->_numOfFrames ||
                        clip->_currentFrame > clip->_rangeEnd)
                    {
                        if (!clip->_looping)
                        {
                            clip->_currentFrame = clip->_rangeEnd;
                            animator.Pause();
                            continue;
                        }
                        else
                        {
                            animator.Play();
                        }
                        clip->_currentFrame = clip->_rangeStart;
                        //clip->_fCurrentFrame = (float)clip->_rangeStart;
                        animator._currentFrame = (float)clip->_rangeStart;;
                    }
                }
            }

            //if (p_Input->KeyIsHeld(GLFW_KEY_LEFT_CONTROL) && p_Input->KeyIsPressed(GLFW_KEY_A))
            //{
                //static size_t idx = 0;
                //ChangeState_Jeff((JeffState)(++idx > 10 ? (idx = 0) : idx));
                //ChangeState_Pirate((PirateState)(++idx > 6 ? (idx = 0) : idx));
                //ChangeState_Mark((MarkState)(++idx > 2 ? (idx = 0) : idx));
            //}
        }

        void Exit()
        {
        }

        AnimationClip* Animator::GetCurrentClip()
        {
            return _currentClip;
        }

        void Animator::SetCurrentClip(AnimationClip& newClip)
        {
            newClip._currentFrame = newClip._rangeStart;
            _currentFrame = (float)newClip._currentFrame;
            //newClip._fCurrentFrame = (float)newClip._currentFrame;
            _currentClip = &newClip;
        }

        const float Animator::GetClipDuration()
        {
            return _currentClip->GetClipSpeed() * (_currentClip->_rangeEnd - _currentClip->_rangeStart) * p_Clock.GetGDT();
        }

        AnimationClip::AnimationClip(const size_t& numOfFrames, const size_t& rowCount, const size_t& columnCount, const float& clipSpeed,
            int start, int end, bool looping) :
            _numOfFrames{ numOfFrames },
            _numOfSpritesRow{ rowCount },
            _rowOffset{ 1.0f / _numOfSpritesRow },
            _numOfSpritesCol{ columnCount },
            _columnOffset{ 1.0f / _numOfSpritesCol },
            _clipSpeed{ clipSpeed },
            _currentFrame{ 0 },
            //_fCurrentFrame{ 0.0f },
            _rangeStart{ start },
            _rangeEnd{ end < 0 ? (int)(_numOfFrames - 1) : end },
            _looping{ looping }
        {
            // Nothing
        }

        float AnimationClip::GetCurrentFrameX()
        {
            return (_numOfSpritesRow == 1 ? 1 : _rowOffset * (_currentFrame % _numOfSpritesRow));
        }

        float AnimationClip::GetCurrentFrameY()
        {
            return (1 - _columnOffset * (floorf((float)_currentFrame / _numOfSpritesRow) + 1));
        }

        float AnimationClip::GetClipSpeed()
        {
            return _clipSpeed;
        }

        void AnimationClip::ChangeClipSpeed(const float& newSpeed)
        {
            _clipSpeed = newSpeed;
        }

        bool AnimationClip::Compare(AnimationClip& otherClip)
        {
            if (_numOfFrames != otherClip._numOfFrames ||
                _rangeStart != otherClip._rangeStart ||
                _rangeEnd != otherClip._rangeEnd)
            {
                return false;
            }

            return true;
        }

        void Animator::Play()
        {
            _isPlaying = true;
        }

        void Animator::Pause()
        {
            _isPlaying = false;
        }

        void Animator::Restart()
        {
            _isPlaying = false;
            _currentClip->_currentFrame = _currentClip->_rangeStart;
            _currentFrame = static_cast<float>(_currentClip->_currentFrame);
        }
    }
}