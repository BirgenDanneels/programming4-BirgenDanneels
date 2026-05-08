#pragma once 
#include "Minigin/Components/Component.h"
#include "Minigin/Events/Observer.h"
#include "Minigin/Physics/PhysicsManager.h"
#include "Minigin/Physics/Collider.h"

class Bullet final : public dae::Component, public dae::Observer<dae::Hit>
{
public:
	Bullet(dae::GameObject& pOwner);
	~Bullet() override;

	void Initialize(int damage, int bounces);

	virtual void Start() override;
	virtual void FixedUpdate(float) override {};
	virtual void Update(float) override {};
	virtual void Render() const override {};

	// Inherited via Observer
	virtual void OnNotify(dae::Hit hit) override;

private:
	dae::Collider* m_pCollider{nullptr};
	int m_damage{ 1 };
	int m_bouncesRemaining{ 5 };

	short int m_shotSoundId{-1};
};
