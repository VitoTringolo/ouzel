// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Fade.hpp"
#include "scene/Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        Fade::Fade(float initLength, float initOpacity, bool initRelative):
            Animator(initLength), opacity(initOpacity), relative(initRelative)
        {
        }

        void Fade::play()
        {
            Animator::play();

            if (targetActor)
            {
                startOpacity = targetActor->getOpacity();
                targetOpacity = relative ? startOpacity + opacity : opacity;

                diff = targetOpacity - startOpacity;
            }
        }

        void Fade::updateProgress()
        {
            Animator::updateProgress();

            if (targetActor)
                targetActor->setOpacity(startOpacity + (diff * progress));
        }
    } // namespace scene
} // namespace ouzel
