#include "Component.h"

namespace dae
{
	class ThrashCacheComponent final : public Component
	{
	public:

		ThrashCacheComponent(GameObject& owner);

		virtual void Update(float deltaTime) override;
		virtual void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		virtual void Render() const override {};
		virtual void RenderUI() const override;
	};
}