#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Components/ComponentsInclude.h"
#include "InputManager.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto background = scene.CreateGameObject();
	background->SetLocalPosition(0, 0);
	background->AddComponent<dae::TextureComponent>()->Initialize("background.png");

	auto logo = scene.CreateGameObject();
	logo->SetLocalPosition(358, 180);
	logo->AddComponent<dae::TextureComponent>()->Initialize("logo.png");

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 25);

	auto progText = scene.CreateGameObject();
	progText->SetLocalPosition(292, 20);
	auto progTextComp = progText->AddComponent<dae::TextComponent>();
	progTextComp->Initialize("Programming 4 Assignment", font);
	progTextComp->SetColor({ 255, 255, 0, 255 });

	auto fpsCounter = scene.CreateGameObject();
	fpsCounter->SetLocalPosition(5, 5);
	fpsCounter->AddComponent<dae::TextComponent>()->Initialize("0", font);
	fpsCounter->AddComponent<dae::FPSCounter>()->Initialize();

	auto blueTank = scene.CreateGameObject();
	blueTank->SetWorldPosition(100, 280);
	blueTank->AddComponent<dae::TextureComponent>()->Initialize("BlueTank.png");
	dae::HealthComponent* blueHealth = blueTank->AddComponent<dae::HealthComponent>();
	dae::PointsComponent* bluePoints = blueTank->AddComponent<dae::PointsComponent>();

	auto redTank = scene.CreateGameObject();
	redTank->SetWorldPosition(300, 100);
	redTank->AddComponent<dae::TextureComponent>()->Initialize("RedTank.png");
	dae::HealthComponent* redHealth = redTank->AddComponent<dae::HealthComponent>();
	dae::PointsComponent* redPoints = redTank->AddComponent<dae::PointsComponent>();

	// Achievement System
#ifdef USE_STEAMWORKS
	auto achievementSystem = scene.CreateGameObject();
	achievementSystem->AddComponent<dae::AchievementSystem>();
#endif
	
	// Controls Text
	auto font2 = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);

	auto textAnchor = scene.CreateGameObject();
	textAnchor->SetLocalPosition(10, 100);

	auto wasdText = scene.CreateGameObject();
	wasdText->SetParent(textAnchor);
	wasdText->SetLocalPosition(0, 0);
	wasdText->AddComponent<dae::TextComponent>()->Initialize("Use WASD to move the blue tank, C to inflict damage, Z to pick up orb and X to kill", font2);

	auto controllerText = scene.CreateGameObject();
	controllerText->SetParent(textAnchor);
	controllerText->SetLocalPosition(0, 20);
	controllerText->AddComponent<dae::TextComponent>()->Initialize("Use D-Pad or left stick to move the red tank, X to inflict damage, A to pick up orb and B  to kill", font2);

	// Blue Tank Lives Text
	auto blueLivesText = scene.CreateGameObject();
	blueLivesText->SetParent(textAnchor);
	blueLivesText->SetLocalPosition(0, 60);
	dae::StatsComponent* blueLives = blueLivesText->AddComponent<dae::StatsComponent>();
	blueLives->Initialize("#Red Tank Lives: ");
	blueHealth->OnHealthChanged().AddObserver(blueLives);

	// Blue Tank Points Text
	auto bluePointsText = scene.CreateGameObject();
	bluePointsText->SetParent(textAnchor);
	bluePointsText->SetLocalPosition(0, 80);
	dae::StatsComponent* points1 = bluePointsText->AddComponent < dae::StatsComponent>();
	points1->Initialize("Blue Tank Points: ");
	bluePoints->OnPointsChanged().AddObserver(points1);

	// Red Tank Lives Text
	auto RedLivesText = scene.CreateGameObject();
	RedLivesText->SetParent(textAnchor);
	RedLivesText->SetLocalPosition(0, 100);
	dae::StatsComponent* redLives = RedLivesText->AddComponent<dae::StatsComponent>();
	redLives->Initialize("#Red Tank Lives: ");
	redHealth->OnHealthChanged().AddObserver(redLives);

	// Red Tank Points Text
	auto pointsText2 = scene.CreateGameObject();
	pointsText2->SetParent(textAnchor);
	pointsText2->SetLocalPosition(0, 120);
	dae::StatsComponent* points2 = pointsText2->AddComponent < dae::StatsComponent>();
	points2->Initialize("Red Tank Points: ");
	redPoints->OnPointsChanged().AddObserver(points2);


	//Init tanks
	blueTank->AddComponent<dae::TankComponent>()->Initialize(dae::InputManager::GetInstance().GetKeyboard(), 100.f, 3);
	redTank->AddComponent<dae::TankComponent>()->Initialize(dae::InputManager::GetInstance().GetGamepad(0), 200.f, 3);
	
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
    return 0;
}
