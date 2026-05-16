#include "Component.h"
#include "Minigin/Texture2D.h"
#include <memory>
#include <SDL3/SDL.h>

namespace dae
{

    class SpriteComponent : public Component
    {
    public:

        SpriteComponent(GameObject& refOwner);

        virtual ~SpriteComponent() = default;


		void Initialize(const std::string& filename, SDL_FRect srcRect, bool centered = true, float xOffset = 0, float yOffset = 0);

        virtual std::vector<ParamDefinition> GetExpectedParams() const override;
		virtual void Load(const ParamMap& params) override;

        virtual void FixedUpdate(float) override {};

        void Update(float) override {};
        void Render() const override;

    private:
        float m_xOffset{ 0 };
        float m_yOffset{ 0 };
        
        std::shared_ptr<Texture2D> m_spriteSheet;
        SDL_FRect m_srcRect;
    };
}
