#pragma once
#include "Minigin/Components/Component.h"
#include "Minigin/Components/TextureComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/Physics/PhysicsManager.h"

class BarrelComponent final : public dae::Component
{
public:
	BarrelComponent(dae::GameObject& pOwner);
	~BarrelComponent() override;

	void Initialize(float shootCooldown, dae::GameObject& bulletSpawn, int damage, int maxBounces, float rotationSpeed);

	virtual void Start() override;
	virtual void FixedUpdate(float /*fixedDeltaTime*/) override {};
	virtual void Update(float deltaTime) override;
	virtual void Render() const override {};

	void Shoot();
	void Rotate(float direction);

private:

	dae::TextureComponent* m_pTextureComponent;
	dae::GameObject* m_pBulletSpawnPoint;

	short int m_shotSoundId;
	int m_damage{ 1 };
	int m_maxBounces{ 5 };

	float m_shootCooldown{ 0.5f };
	float m_shootTimer{ 0.0f };
	float m_rotationSpeed{ 90.0f }; // Degrees per second
	float m_rotationDirection{ 0.0f }; // -1 for left, 1 for right, 0 for no rotation
};
