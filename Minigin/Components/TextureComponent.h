#pragma once
#include "Component.h"
#include "Texture2D.h"
#include <memory>

namespace dae
{
	class TextureComponent : public	Component
	{
	public:

		TextureComponent() = delete;
		TextureComponent(dae::GameObject& refOwner);

		TextureComponent(const TextureComponent& other) = delete;
		TextureComponent(TextureComponent&& other) = delete;
		TextureComponent& operator=(const TextureComponent& other) = delete;
		TextureComponent& operator=(TextureComponent&& other) = delete;

		~TextureComponent() = default;

		virtual void FixedUpdate(float fixedDeltaTime) override;
		virtual void Update(float deltaTime) override;

		virtual void Initialize(const std::string& filename, float xOffset = 0, float yOffset = 0);

		virtual void Render() const override;

		void SetOffset(float x, float y);

	protected:

		int m_Depth{ 10 }; //0 is the top layer

		void UpdateTexture(std::shared_ptr<dae::Texture2D> texture);

	private:

		float m_xOffset{ 0 };
		float m_yOffset{ 0 };

		std::shared_ptr<dae::Texture2D> m_texture;
	};
}