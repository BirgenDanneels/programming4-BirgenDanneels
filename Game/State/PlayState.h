#include "GameState.h"

class PlayState final : public GameState
{
public:
	std::string GetSceneFile() override { return "Level1.json"; }
	void OnEnter() override
	{
	}
	void OnExit() override
	{
	}
	std::unique_ptr<GameState> HandleEvents(const dae::Event&) override
	{
		return nullptr;
	}
};