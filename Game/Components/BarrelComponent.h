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

	void Initialize(float fireRate, dae::GameObject& bulletSpawn, int damage, int maxBounces, float rotationSpeed);

	virtual void Start() override;
	virtual void FixedUpdate(float) override {};
	virtual void Update(float deltaTime) override;
	virtual void Render() const override {};

	void Shoot();
	void Rotate(float direction);

	//Loading functions
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

private:

	dae::TextureComponent* m_pTextureComponent{nullptr};
	dae::GameObject* m_pBulletSpawnPoint{nullptr};

	short int m_shotSoundId{-1};
	int m_damage{ 1 };
	int m_maxBounces{ 5 };

	float m_fireRate{ 0.5f };
	float m_shootTimer{ 0.0f };
	float m_rotationSpeed{ 90.0f }; // Degrees per second
	float m_rotationDirection{ 0.0f }; // -1 for left, 1 for right, 0 for no rotation
};
