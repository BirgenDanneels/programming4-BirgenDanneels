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
	class InputDevice;
}

class HealthComponent;
class PointsComponent;

enum class TankEvents
{
	KillEnemy,
	PickupOrb,
	Win
};

class TankComponent final : public dae::Component, public dae::Observer<dae::HitEvent>
{
public:
	TankComponent(dae::GameObject& pOwner);
	~TankComponent() override;

	virtual void Start() override;
	virtual void FixedUpdate(float) override {};
	virtual void Update(float) override;
	virtual void Render() const override {};

	//These are functions to demonstrate how this component can broadcast events. This isnt a realistic example of how these events would be triggered.
	void RequestEnemyKill();
	void RequestOrbPickUp();
		
	void TakeDamage(int damage);

	void Initialize(const std::string& device, float speed, int lives, dae::GameObject& barrel);
	//Loading functions
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

	dae::Subject<TankEvents>& OnTankEvent() { return m_onTankEventSubject; }

	virtual void OnNotify(dae::HitEvent hit) override;

private:

	dae::Rigidbody* m_pRigidbody;
	HealthComponent* m_pHealthComponent;
	PointsComponent* m_pPointsComponent;

	std::unique_ptr<dae::Axis2DCommand> m_pMoveCommand;
	std::unique_ptr<dae::Axis1DCommand> m_pRotateBarrelCommand;
	std::unique_ptr<dae::Command> m_pDamageCommand;
	std::unique_ptr<dae::Command> m_pPickupCommand;
	std::unique_ptr<dae::Command> m_pShootCommand;

	dae::Subject<TankEvents> m_onTankEventSubject;

	dae::sound_id m_shotSound;

	dae::ObserverHandle m_pointObserverHandle; //A component on the points object should do this subscription and hold the handle.
	dae::ObserverHandle m_collisionHandle;

	dae::InputDevice* m_pInputDevice{ nullptr };
	dae::GameObject* m_pBarrel{ nullptr };
	float m_Speed{ 100.0f };
};
