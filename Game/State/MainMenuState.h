#pragma once
#include "Game/State/GameState.h"
#include "PlayState.h"

class MainMenuState final : public GameState
{
public:
	std::string GetSceneFile() override { return "Level.json"; }
	void OnEnter() override
	{

	}

	void OnExit() override
	{

	}

	std::unique_ptr<GameState> HandleEvents(const dae::Event& event) override
	{
		if (event.GetId() == dae::Event::ToEventID("StartButtonPressed"))
		{
			return std::make_unique<PlayState>();
		}
		else if (event.GetId() == dae::Event::ToEventID("ExitButtonPressed"))
		{
			//exit(0);
		}
		return nullptr;
	}
};