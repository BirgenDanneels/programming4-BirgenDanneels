#pragma once
#include "Component.h"
#include "Minigin/Texture2D.h"
#include "Minigin/Events/Subject.h"
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

namespace dae
{
    class AnimatedSpriteComponent : public Component
    {
    public:
        AnimatedSpriteComponent(GameObject& refOwner);
        virtual ~AnimatedSpriteComponent() = default;

        // Animation data: list of source rects from the sprite sheet
        void Initialize(const std::string& filename,
			           SDL_FRect firstFrameSrcRect,
                       int numberOfFrames,
                       float secondsPerFrame, 
                       bool canLoop = true,
                       bool centered = true, 
                       float xOffset = 0, 
                       float yOffset = 0);

        virtual std::vector<ParamDefinition> GetExpectedParams() const override;
        virtual void Load(const ParamMap& params) override;

        virtual void FixedUpdate(float) override {}
        virtual void Update(float deltaTime) override;
        virtual void Render() const override;

        Subject<>& GetAnimationEndedEvent() { return m_animationEndedEvent; }

    private:
        std::shared_ptr<Texture2D> m_spriteSheet;
        std::vector<SDL_FRect> m_frames;
        float m_secondsPerFrame{ 0.f };
        bool m_canLoop{ true };
        
        float m_currentTime{ 0.f };
        size_t m_currentFrameIndex{ 0 };
        bool m_animationFinished{ false };

        float m_xOffset{ 0 };
        float m_yOffset{ 0 };

        Subject<> m_animationEndedEvent; // Event fired when animation ends
    };
}
