#pragma once
#include "Minigin/Scene.h"
#include <memory>

namespace dae
{
	class Event;
}

class GameState
{
public:
	virtual ~GameState() = default;

	virtual std::string GetSceneFile() = 0;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
	virtual std::unique_ptr<GameState> HandleEvents(const dae::Event& event) = 0;
};