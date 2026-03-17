#include "Component.h"
#include "Events/Subject.h"

namespace dae
{
	class GameObject;

	class HealthComponent final : public Component
	{
	public:
		HealthComponent(GameObject& pOwner);
		~HealthComponent() override;

		void Update(float deltaTime) override;
		void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		void Render() const override {};

		void Initialize(int health);

		void TakeDamage(int damage);
		int GetHealth() const { return m_Health; }

		Subject<GameObject*>& OnDead() { return m_onDeadSubject; }
		Subject<int>& OnHealthChanged() { return m_onHealthChangedSubject; }

	private:
		int m_Health{ 3 };

		Subject<int> m_onHealthChangedSubject;
		Subject<GameObject*> m_onDeadSubject;
	};
}