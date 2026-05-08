#include "Minigin/Events/Observer.h"
#include "Minigin/Components/Component.h"
#include "Minigin/Loading/Interfaces/IComponentLoadable.h"

namespace dae
{
	class TextComponent;
}

class StatsComponent final : public dae::Component, public dae::IComponentLoadable, public dae::Observer<int>
{
public:
	StatsComponent(dae::GameObject& pOwner);
	~StatsComponent() override;
	void Update(float) override {};
	void FixedUpdate(float) override {};
	void Render() const override {};
	virtual void OnNotify(int stat) override;

	void Initialize(std::string preStatString);

	// Inherited via IComponentLoadable
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

private:
		
	dae::TextComponent* m_ptrTextComponent;
	std::string m_preStatString{};
};
