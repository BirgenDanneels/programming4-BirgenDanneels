#include "Events/Observer.h"
#include "Component.h"
#include "Loading/Interfaces/IComponentLoadable.h"

namespace dae
{
	class TextComponent;

	class StatsComponent final : public Component, public IComponentLoadable, public Observer<int>
	{
	public:
		StatsComponent(GameObject& pOwner);
		~StatsComponent() override;
		void Update(float deltaTime) override;
		void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		void Render() const override {};
		virtual void OnNotify(int stat) override;

		void Initialize(std::string preStatString);

		// Inherited via IComponentLoadable
		virtual void Load(const ParamMap& params) override;

	private:
		
		TextComponent* m_ptrTextComponent;
		std::string m_preStatString{};
	};
}