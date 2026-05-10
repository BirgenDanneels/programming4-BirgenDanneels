#include "Minigin/Events/Observer.h"
#include "Minigin/Components/Component.h"
#include "Minigin/Loading/Interfaces/IComponentLoadable.h"
#include "Minigin/Events/Subject.h"

namespace dae
{
	class TextComponent;
}

class StatsComponent : public dae::Component, public dae::IComponentLoadable, public dae::Observer<int>
{
public:
	StatsComponent(dae::GameObject& pOwner);
	~StatsComponent() override;
	virtual void Update(float) override {};
	virtual void FixedUpdate(float) override {};
	virtual void Render() const override {};
	virtual void OnNotify(int stat) override;

	void Initialize(std::string preStatString);

	// Inherited via IComponentLoadable
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

private:
		
	dae::TextComponent* m_ptrTextComponent;
	std::string m_preStatString{};
};

class PointsStatsComponent final : public StatsComponent
{
public:
	PointsStatsComponent(dae::GameObject& pOwner);

	 void Initialize(std::string preStatString, dae::GameObject& targetObject);

private:

	dae::ObserverHandle m_pointsObserverHandle;
};	

class HealthStatsComponent final : public StatsComponent
{
public:
	HealthStatsComponent(dae::GameObject& pOwner);
	void Initialize(std::string preStatString, dae::GameObject& targetObject);

private:
	dae::ObserverHandle m_healthObserverHandle;
};