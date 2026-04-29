#include "Minigin/Components/Component.h"
#include <memory>
#include "Minigin/Commands/Command.h"
#include "Minigin/Events/Subject.h"
#include "Minigin/Sound/ServiceLocator.h"

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

	class TankComponent final : public dae::Component
	{
	public:
		TankComponent(dae::GameObject& pOwner);
		~TankComponent() override;

		virtual void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		virtual void Update(float deltaTime) override;
		virtual void Render() const override {};

		//These are functions to demonstrate how this component can broadcast events. This isnt a realistic example of how these events would be triggered.
		void RequestEnemyKill();
		void RequestOrbPickUp();

		void Initialize(dae::InputDevice* device, float speed, int lives);

		dae::Subject<TankEvents>& OnTankEvent() { return m_onTankEventSubject; }

	private:

		dae::CharacterController* m_pCharacterController;
		dae::HealthComponent* m_pHealthComponent;
		dae::PointsComponent* m_pPointsComponent;

		std::unique_ptr<dae::Axis2DCommand> m_pMoveCommand;
		std::unique_ptr<dae::Command> m_pDamageCommand;
		std::unique_ptr<dae::Command> m_pPickupCommand;
		std::unique_ptr<dae::Command> m_pKillCommand;

		dae::Subject<TankEvents> m_onTankEventSubject;

		dae::sound_id m_shotSound;

		dae::InputDevice* m_pInputDevice{ nullptr };
	};