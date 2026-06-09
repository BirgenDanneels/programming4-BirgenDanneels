#pragma once
#include "Minigin/Minigin.h"
#include "Minigin/Components/Component.h"
#include "Game/State/GameState.h"
#include "Minigin/Events/Observer.h"
#include "Minigin/SceneManager.h"
#include "Minigin/Events/Event.h"
#include <memory>

class GameStateMachine final : public dae::Component, public dae::Observer<dae::OnSceneLoadedEvent>
{
public:
	GameStateMachine(dae::GameObject& owner);
	~GameStateMachine() override;

	void SetEngine(dae::Minigin* engine) { m_engine = engine; }
	
	void Start() override;
	void HandleEvents(const dae::Event& event);
	void Update(float deltaTime) override;
	void Render() const override;
	void FixedUpdate(float) override {};

	std::vector<dae::ParamDefinition> GetExpectedParams() const override { return {}; }
	void Load(const dae::ParamMap&) override {};

	// Observer implementation
	void OnNotify(dae::OnSceneLoadedEvent event) override;


private:
	enum class TransitionPhase
	{
		None,
		FadingOut,
		Loading,
		FadingIn
	};

	dae::Minigin* m_engine = nullptr;
	dae::Scene* m_currentScene = nullptr;
	std::unique_ptr<GameState> m_currentState;
	std::unique_ptr<GameState> m_nextState;

	TransitionPhase m_transitionPhase = TransitionPhase::None;
	float m_fadeDuration = 1.0f;
	float m_fadeTimer = 0.0f;
	float m_fadeAlpha = 0.0f;

	dae::SubscriptionHandle m_currentQueueSubscription;

	dae::ObserverHandle m_sceneLoadedHandle;

	//void StartTransition(std::unique_ptr<GameState> newState);
	//void UpdateTransition(float deltaTime);

	void StartCreatingNewScene();
};