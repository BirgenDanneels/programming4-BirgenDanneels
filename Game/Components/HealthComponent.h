#include "Minigin/Components/Component.h"
#include "Minigin/Events/Subject.h"

namespace dae
{
	class GameObject;
}

class HealthComponent final : public dae::Component
{
public:
	HealthComponent(dae::GameObject& pOwner);
	~HealthComponent() override;

	void Start() override;
	void Update(float) override {};
	void FixedUpdate(float) override {};
	void Render() const override {};

	void Initialize(int health);

	void TakeDamage(int damage);
	int GetHealth() const { return m_Health; }

	dae::Subject<dae::GameObject*>& OnDead() { return m_onDeadSubject; }
	dae::Subject<int>& OnHealthChanged() { return m_onHealthChangedSubject; }

	// Inherited via IComponentLoadable
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

private:
	int m_Health{ 3 };

	dae::Subject<int> m_onHealthChangedSubject;
	dae::Subject<dae::GameObject*> m_onDeadSubject;
};
