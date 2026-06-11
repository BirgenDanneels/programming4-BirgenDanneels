#include "GameStateMachine.h"
#include "MainMenuState.h"
#include "Minigin/Renderer.h"
#include "Minigin/Components/ComponentsInclude.h"

GameStateMachine::GameStateMachine(dae::GameObject& owner)
	: Component(owner)
{
	dae::SceneManager::GetInstance().SetActiveScene(nullptr);
}

GameStateMachine::~GameStateMachine()
{
	if(m_currentScene)
	{
		m_currentScene->GetGameEventQueue().Unsubscribe(m_currentQueueSubscription);
	}
}

void GameStateMachine::Start()
{
	//Create start state
	m_currentState = std::make_unique<MainMenuState>();
	StartCreatingNewScene();
}

void GameStateMachine::HandleEvents(const dae::Event& event)
{
	if (!m_currentState || m_transitionPhase != TransitionPhase::None)
		return;

	auto newState = m_currentState->HandleEvents(event);

	if(newState)
	{
		m_nextState = std::move(newState);
		m_transitionPhase = TransitionPhase::FadingOut;
		m_fadeTimer = 0.0f;
		m_fadeAlpha = 0.0f;
	}
}

void GameStateMachine::Update(float deltaTime)
{

	switch (m_transitionPhase)
	{
	case GameStateMachine::TransitionPhase::None:
		break;
	case GameStateMachine::TransitionPhase::FadingOut:
	{
		m_fadeTimer += deltaTime;
		float fadeProgress = m_fadeTimer / m_fadeDuration;
		m_fadeAlpha = std::min(fadeProgress, 1.0f);

		if (fadeProgress >= 1.0f)
		{
			m_fadeTimer = 0.0f;
			m_fadeAlpha = 1.0f;

			if (m_currentScene)
			{
				m_currentScene->GetGameEventQueue().Unsubscribe(m_currentQueueSubscription);
				m_currentState->OnExit();
			}
			m_currentState = std::move(m_nextState);

			m_transitionPhase = TransitionPhase::Loading;
			StartCreatingNewScene();
		}

		break;
	}
	case GameStateMachine::TransitionPhase::Loading:
		break;
	case GameStateMachine::TransitionPhase::FadingIn:
	{
		m_fadeTimer += deltaTime;
		float fadeProgress = m_fadeTimer / m_fadeDuration;
		m_fadeAlpha = std::max(1.0f - fadeProgress, 0.0f);

		if (fadeProgress >= 1.0f)
		{
			m_fadeTimer = 0.0f;
			m_fadeAlpha = 0.0f;
			m_transitionPhase = TransitionPhase::None;
		}

		break;
	}
	default:
		break;
	}
}

void GameStateMachine::Render() const
{
	if (m_fadeAlpha <= 0.0f)
		return;

	int width{};
	int height{};

	auto renderer = dae::Renderer::GetInstance().GetSDLRenderer();
	SDL_GetWindowSize(SDL_GetRenderWindow(renderer), &width, &height);
	
	SDL_FRect rect{ 0, 0, static_cast<float>(width), static_cast<float>(height) };

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(m_fadeAlpha * 255));
	SDL_RenderFillRect(renderer, &rect);
}

void GameStateMachine::OnNotify(dae::OnSceneLoadedEvent event)
{
	m_sceneLoadedHandle.Unsubscribe();

	m_currentScene = event.loadedScene;
	dae::SceneManager::GetInstance().SetActiveScene(m_currentScene);
	
	m_currentQueueSubscription = m_currentScene->GetGameEventQueue().SubscribeAll([this](const dae::Event& event) { HandleEvents(event); });

	if (m_currentState)
		m_currentState->OnEnter();

	m_transitionPhase = TransitionPhase::FadingIn;
}

void GameStateMachine::StartCreatingNewScene()
{
	auto& sceneManager = dae::SceneManager::GetInstance();

	//Set Loading scene active while loading new scene
	sceneManager.SetActiveScene(nullptr);

	//Subscribe to scene loaded event and load scene of new state
	m_sceneLoadedHandle = sceneManager.GetSceneLoadedSubject().AddObserver(this);
	sceneManager.LoadScene(m_currentState->GetSceneFile());
}
