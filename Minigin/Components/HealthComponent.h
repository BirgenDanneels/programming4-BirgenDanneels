#include "Component.h"
#include "Loading/Interfaces/IComponentLoadable.h"
#include "Loading/Interfaces/IEventLinkable.h"
#include "Events/Subject.h"

namespace dae
{
	class GameObject;

	class HealthComponent final : public Component, public IComponentLoadable, public IEventLinkable
	{
	public:
		HealthComponent(GameObject& pOwner);
		~HealthComponent() override;

		void Start() override;
		void Update(float deltaTime) override;
		void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		void Render() const override {};

		void Initialize(int health);

		void TakeDamage(int damage);
		int GetHealth() const { return m_Health; }

		Subject<GameObject*>& OnDead() { return m_onDeadSubject; }
		Subject<int>& OnHealthChanged() { return m_onHealthChangedSubject; }

		// Inherited via IComponentLoadable
		virtual void Load(const ParamMap& params) override;

		// Inherited via IEventLinkable
		virtual bool Bind(const std::string& eventName, IObserver* observer) override;
		virtual bool Unbind(const std::string& eventName, IObserver* observer) override;

	private:
		int m_Health{ 3 };

		Subject<int> m_onHealthChangedSubject;
		Subject<GameObject*> m_onDeadSubject;
	};
}