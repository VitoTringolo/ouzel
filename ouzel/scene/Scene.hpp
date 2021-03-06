// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_SCENE_HPP
#define OUZEL_SCENE_SCENE_HPP

#include <vector>
#include <unordered_map>
#include <cstdint>
#include "math/Vector2.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;
        class Layer;

        class Scene
        {
            friend SceneManager;
        public:
            Scene();
            virtual ~Scene();

            Scene(const Scene&) = delete;
            Scene& operator=(const Scene&) = delete;

            Scene(Scene&&) = delete;
            Scene& operator=(Scene&&) = delete;

            virtual void draw();

            void addLayer(Layer* layer);

            template<typename T> void addLayer(const std::unique_ptr<T>& layer)
            {
                addLayer(layer.get());
            }

            template<typename T> void addLayer(std::unique_ptr<T>&& layer)
            {
                addLayer(layer.get());
                ownedLayers.push_back(std::move(layer));
            }

            bool removeLayer(Layer* layer);

            template<typename T> bool removeLayer(const std::unique_ptr<T>& layer)
            {
                return removeLayer(layer.get());
            }

            void removeAllLayers();
            bool hasLayer(Layer* layer) const;
            inline const std::vector<Layer*>& getLayers() const { return layers; }

            virtual void recalculateProjection();

            std::pair<Actor*, Vector3<float>> pickActor(const Vector2<float>& position, bool renderTargets = false) const;
            std::vector<std::pair<Actor*, Vector3<float>>> pickActors(const Vector2<float>& position, bool renderTargets = false) const;
            std::vector<Actor*> pickActors(const std::vector<Vector2<float>>& edges, bool renderTargets = false) const;

        protected:
            virtual void enter();
            virtual void leave();

            bool handleWindow(const WindowEvent& event);
            bool handleMouse(const MouseEvent& event);
            bool handleTouch(const TouchEvent& event);

            void pointerEnterActor(uint64_t pointerId, Actor* actor, const Vector2<float>& position);
            void pointerLeaveActor(uint64_t pointerId, Actor* actor, const Vector2<float>& position);
            void pointerDownOnActor(uint64_t pointerId, Actor* actor, const Vector2<float>& position, const Vector3<float>& localPosition);
            void pointerUpOnActor(uint64_t pointerId, Actor* actor, const Vector2<float>& position);
            void pointerDragActor(uint64_t pointerId, Actor* actor, const Vector2<float>& position,
                                  const Vector2<float>& difference, const Vector3<float>& localPosition);

            SceneManager* sceneManger = nullptr;

            std::vector<Layer*> layers;
            std::vector<std::unique_ptr<Layer>> ownedLayers;
            EventHandler eventHandler;

            std::unordered_map<uint64_t, std::pair<Actor*, Vector3<float>>> pointerDownOnActors;

            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_SCENE_HPP
