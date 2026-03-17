#include "Component.h"
#include <memory>
#include "Commands/Command.h"


//Components should have an awake method so the getcomponents can be used without worrying about order of creation.

namespace dae
{
	class CharacterController;
	class HealthComponent;
	class InputDevice;
	class Move2DCommand;
	class DamageCommand;

	class TankComponent final : public Component
	{
	public:
		TankComponent(GameObject& pOwner);
		~TankComponent() override;
		
		virtual void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		virtual void Update(float deltaTime) override;
		virtual void Render() const override {};

		void Initialize(InputDevice* device, float speed, int lives);

	private:

		CharacterController* m_pCharacterController;
		HealthComponent* m_pHealthComponent;


		std::unique_ptr<Move2DCommand> m_pMoveCommand;
		std::unique_ptr<DamageCommand> m_pDamageCommand;
	};
}