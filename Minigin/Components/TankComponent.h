#include "Component.h"
#include <memory>
#include "Commands/Command.h"
#include "Events/Subject.h"


//Components should have an awake method so the getcomponents can be used without worrying about order of creation.

namespace dae
{
	enum class TankEvents
	{
		KillEnemy,
		PickupOrb,
		Win
	};

	class CharacterController;
	class HealthComponent;
	class InputDevice;
	class PointsComponent;

	class TankComponent final : public Component
	{
	public:
		TankComponent(GameObject& pOwner);
		~TankComponent() override;
		
		virtual void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		virtual void Update(float deltaTime) override;
		virtual void Render() const override {};

		//These are functions to demonstrate how this component can broadcast events. This isnt a realistic example of how these events would be triggered.
		void RequestEnemyKill();
		void RequestOrbPickUp();

		void Initialize(InputDevice* device, float speed, int lives);
		
		Subject<TankEvents>& OnTankEvent() { return m_onTankEventSubject; }

	private:

		CharacterController* m_pCharacterController;
		HealthComponent* m_pHealthComponent;
		PointsComponent* m_pPointsComponent;

		std::unique_ptr<Axis2DCommand> m_pMoveCommand;
		std::unique_ptr<Command> m_pDamageCommand;
		std::unique_ptr<Command> m_pPickupCommand;
		std::unique_ptr<Command> m_pKillCommand;

		Subject<TankEvents> m_onTankEventSubject;
	};
}