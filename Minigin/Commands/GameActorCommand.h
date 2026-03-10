#include "Commands/Command.h"
#include "Components/GameActor.h"

namespace dae
{
	class GameActorCommand : public Command
	{
	public:
		
		GameActorCommand(GameActor& actor)
			: m_pActor(&actor)
		{}

		virtual ~GameActorCommand() = default;

	private:

		GameActor* m_pActor{ nullptr };

	protected:
		
		GameActor* GetGameActor() const { return m_pActor; }

	};
}