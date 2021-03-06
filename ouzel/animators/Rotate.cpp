// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Rotate.hpp"
#include "scene/Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        Rotate::Rotate(float initLength, const Vector3<float>& initRotation, bool initRelative):
            Animator(initLength), rotation(initRotation), relative(initRelative)
        {
        }

        void Rotate::play()
        {
            Animator::play();

            if (targetActor)
            {
                startRotation = targetActor->getRotation().getEulerAngles();

                targetRotation = relative ? startRotation + rotation : rotation;

                diff = targetRotation - startRotation;
            }
        }

        void Rotate::updateProgress()
        {
            Animator::updateProgress();

            if (targetActor)
                targetActor->setRotation(startRotation + diff * progress);
        }
    } // namespace scene
} // namespace ouzel
