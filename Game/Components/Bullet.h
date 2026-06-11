#pragma once 
#include "Minigin/Components/Component.h"
#include "Minigin/Events/Observer.h"
#include "Minigin/Physics/PhysicsManager.h"
#include "Minigin/Physics/Collider.h"

class Bullet final : public dae::Component, public dae::Observer<dae::HitEvent>
{
public:
	Bullet(dae::GameObject& pOwner);
	~Bullet() override;

	void Initialize(int damage, int bounces, std::string targetTag);
	//Loading functions
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

	virtual void Start() override;
	virtual void FixedUpdate(float) override {};
	virtual void Update(float) override {};
	virtual void Render() const override {};

	// Inherited via Observer
	virtual void OnNotify(dae::HitEvent hit) override;

private:
	dae::Collider* m_pCollider{nullptr};
	dae::ObserverHandle m_collisionHandle;

	int m_damage{ 1 };
	int m_bouncesRemaining{ 5 };
	std::string m_targetTag{"Default"};
};
