#include "Minigin/Components/Component.h"
#include <memory>
#include "Minigin/Commands/Command.h"
#include "Minigin/Events/Subject.h"
#include "Minigin/Sound/ServiceLocator.h"
#include "Minigin/Physics/PhysicsManager.h"
#include "Minigin/Events/Observer.h"

namespace dae
{
	class CharacterController;
	class HealthComponent;
	class PointsComponent;
	class InputDevice;
}


	enum class TankEvents
	{
		KillEnemy,
		PickupOrb,
		Win
	};

	class TankComponent final : public dae::Component, public dae::Observer<dae::Hit>
	{
	public:
		TankComponent(dae::GameObject& pOwner);
		~TankComponent() override;

		virtual void Start() override;
		virtual void FixedUpdate(float /*fixedDeltaTime*/) override {};
		virtual void Update(float deltaTime) override;
		virtual void Render() const override {};

		//These are functions to demonstrate how this component can broadcast events. This isnt a realistic example of how these events would be triggered.
		void RequestEnemyKill();
		void RequestOrbPickUp();
		
		void TakeDamage(int damage);

		void Initialize(dae::InputDevice* device, float speed, int lives, dae::GameObject& barrel);

		dae::Subject<TankEvents>& OnTankEvent() { return m_onTankEventSubject; }

		virtual void OnNotify(dae::Hit hit) override;

	private:

		dae::Rigidbody* m_pRigidbody;
		dae::HealthComponent* m_pHealthComponent;
		dae::PointsComponent* m_pPointsComponent;

		std::unique_ptr<dae::Axis2DCommand> m_pMoveCommand;
		std::unique_ptr<dae::Axis1DCommand> m_pRotateBarrelCommand;
		std::unique_ptr<dae::Command> m_pDamageCommand;
		std::unique_ptr<dae::Command> m_pPickupCommand;
		std::unique_ptr<dae::Command> m_pShootCommand;

		dae::Subject<TankEvents> m_onTankEventSubject;

		dae::sound_id m_shotSound;

		dae::InputDevice* m_pInputDevice{ nullptr };
		dae::GameObject* m_pBarrel{ nullptr };
		float m_Speed{ 100.0f };
	};