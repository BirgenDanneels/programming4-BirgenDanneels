#include "Minigin/Components/Component.h"
#include <memory>
#include "Minigin/Commands/Command.h"
#include "Minigin/Events/Subject.h"
#include "Minigin/Sound/ServiceLocator.h"
#include "Minigin/Physics/PhysicsManager.h"
#include "Minigin/Events/Observer.h"

namespace dae
{
	class InputDevice;
	class Rigidbody;
	class GameObject;
}

class HealthComponent;
class PointsComponent;
class NodeMovementComponent;

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
	virtual void Update(float) override {};
	virtual void Render() const override {};
		
	void TakeDamage(int damage);

	void Initialize(const std::string& device, float speed, int lives, dae::GameObject& barrel, dae::GameObject* startNode);

	//Loading functions
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

	dae::Subject<TankEvents>& OnTankEvent() { return m_onTankEventSubject; }

	virtual void OnNotify(dae::HitEvent hit) override;

	void SetHeldMoveInput(const glm::vec3& input);

private:
	HealthComponent* m_pHealthComponent{ nullptr };
	PointsComponent* m_pPointsComponent{ nullptr };
	NodeMovementComponent* m_pNodeMovementComponent{ nullptr };

	std::unique_ptr<dae::Axis2DCommand> m_pMoveCommand;
	std::unique_ptr<dae::Axis1DCommand> m_pRotateBarrelCommand;
	std::unique_ptr<dae::Command> m_pShootCommand;

	dae::Subject<TankEvents> m_onTankEventSubject;

	dae::sound_id m_shotSound{0};

	dae::ObserverHandle m_pointObserverHandle; //A component on the points object should do this subscription and hold the handle.
	dae::ObserverHandle m_collisionHandle;

	dae::InputDevice* m_pInputDevice{ nullptr };
	dae::GameObject* m_pBarrel{ nullptr };
	float m_Speed{ 100.0f };
	float m_NodeReachDistance{ 1.0f };
	int m_Lives{ 3 };

	dae::GameObject* m_pStartNode{ nullptr };
};
