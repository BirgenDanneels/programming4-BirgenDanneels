#include "Component.h"
#include <memory>


//Components should have an awake method so the getcomponents can be used without worrying about order of creation.

namespace dae
{
	class CharacterController;
	class InputDevice;
	class Move2DCommand;

	class TankComponent final : public Component
	{
	public:
		TankComponent(GameObject& pOwner);
		~TankComponent() override;
		
		virtual void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		virtual void Update(float deltaTime) override;
		virtual void Render() const override {};

		void Initialize(InputDevice* device, float speed);

	private:

		CharacterController* m_pCharacterController;
		std::unique_ptr<Move2DCommand> m_pMoveCommand;
	};
}