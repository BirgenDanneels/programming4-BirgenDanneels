#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin/Minigin.h"
#include "Minigin/SceneManager.h"
#include "Minigin/ResourceManager.h"
#include "Minigin/Scene.h"
#include "Game/Components/ComponentsInclude.h"
#include "Minigin/Components/ComponentsInclude.h"
#include "Minigin/InputManager.h"
#include "Minigin/Physics/Collider.h"

#include "Game/State/GameStateMachine.h"


#include <filesystem>
namespace fs = std::filesystem;

inline void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	dae::SceneManager::GetInstance().SetActiveScene(&scene);

	auto background = scene.CreateGameObject();
	background->GetTransform().SetLocalPosition(0, 0);
	background->AddComponent<dae::TextureComponent>()->Initialize("background.png", false);

	auto logo = scene.CreateGameObject();
	logo->GetTransform().SetLocalPosition(358, 180);
	logo->AddComponent<dae::TextureComponent>()->Initialize("logo.png");

	std::shared_ptr<dae::Font> font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 25);

	auto progText = scene.CreateGameObject();
	progText->GetTransform().SetLocalPosition(292, 20);
	auto progTextComp = progText->AddComponent<dae::TextComponent>();
	progTextComp->Initialize("Programming 4 Assignment", font);
	progTextComp->SetColor({ 255, 255, 0, 255 });

	auto fpsCounter = scene.CreateGameObject();
	fpsCounter->GetTransform().SetLocalPosition(5, 5);
	fpsCounter->AddComponent<dae::TextComponent>()->Initialize("0", font);
	fpsCounter->AddComponent<FPSCounter>()->Initialize();

	auto blueTank = scene.CreateGameObject();
	blueTank->GetTransform().SetLocalPosition(100, 280);
	blueTank->AddComponent<dae::TextureComponent>()->Initialize("BlueTank.png");
	blueTank->AddComponent<HealthComponent>();
	blueTank->AddComponent<PointsComponent>();

	auto redTank = scene.CreateGameObject();
	redTank->GetTransform().SetLocalPosition(300, 100);
	redTank->AddComponent<dae::TextureComponent>()->Initialize("RedTank.png");
	redTank->AddComponent<HealthComponent>();
	redTank->AddComponent<PointsComponent>();

	// Controls Text
	auto font2 = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);

	auto textAnchor = scene.CreateGameObject();
	textAnchor->GetTransform().SetLocalPosition(10, 100);

	auto wasdText = scene.CreateGameObject();
	wasdText->SetParent(textAnchor);
	wasdText->GetTransform().SetLocalPosition(0, 0);
	wasdText->AddComponent<dae::TextComponent>()->Initialize("Use WASD to move the blue tank, C to inflict damage, Z to pick up orb and X to kill (with sound)", font2);

	auto controllerText = scene.CreateGameObject();
	controllerText->SetParent(textAnchor);
	controllerText->GetTransform().SetLocalPosition(0, 20);
	controllerText->AddComponent<dae::TextComponent>()->Initialize("Use D-Pad or left stick to move the red tank, X to inflict damage, A to pick up orb and B  to kill (with sound)", font2);

	// Blue Tank Lives Text
	auto blueLivesText = scene.CreateGameObject();
	blueLivesText->SetParent(textAnchor);
	blueLivesText->GetTransform().SetLocalPosition(0, 60);
	blueLivesText->AddComponent<HealthStatsComponent>()->Initialize("#Red Tank Lives: ", *blueTank);

	// Blue Tank Points Text
	auto bluePointsText = scene.CreateGameObject();
	bluePointsText->SetParent(textAnchor);
	bluePointsText->GetTransform().SetLocalPosition(0, 80);
	bluePointsText->AddComponent<PointsStatsComponent>()->Initialize("Blue Tank Points: ", *blueTank);

	// Red Tank Lives Text
	auto RedLivesText = scene.CreateGameObject();
	RedLivesText->SetParent(textAnchor);
	RedLivesText->GetTransform().SetLocalPosition(0, 100);
	RedLivesText->AddComponent<HealthStatsComponent>()->Initialize("#Red Tank Lives: ", *redTank);

	// Red Tank Points Text
	auto pointsText2 = scene.CreateGameObject();
	pointsText2->SetParent(textAnchor);
	pointsText2->GetTransform().SetLocalPosition(0, 120);
	pointsText2->AddComponent <PointsStatsComponent>()->Initialize("Red Tank Points: ",*redTank);

	// Barrels
	auto playerBarrel = scene.CreateGameObject();
	playerBarrel->SetParent(redTank);
	playerBarrel->GetTransform().SetLocalPosition(0, 0);
	auto blueBarrelComp = playerBarrel->AddComponent<BarrelComponent>();
	auto bulletSpawnPoint1 = scene.CreateGameObject();
	bulletSpawnPoint1->SetParent(playerBarrel);
	bulletSpawnPoint1->GetTransform().SetLocalPosition(25, 0);
	blueBarrelComp->Initialize(0.5f, *bulletSpawnPoint1, 1, 5, 20);


	//add nodes
	auto node1 = scene.CreateGameObject();
	node1->GetTransform().SetLocalPosition(200, 280);
	node1->AddComponent<TankNodeComponent>();

	auto node2 = scene.CreateGameObject();
	node2->GetTransform().SetLocalPosition(200, 100);
	node2->AddComponent<TankNodeComponent>();

	auto node3 = scene.CreateGameObject();
	node3->GetTransform().SetLocalPosition(400, 100);
	node3->AddComponent<TankNodeComponent>();

	auto node4 = scene.CreateGameObject();
	node4->GetTransform().SetLocalPosition(100, 100);
	node4->AddComponent<TankNodeComponent>();

	node1->GetComponent<TankNodeComponent>()->SetNeighbour(TankDirection::Up, node2);
	node2->GetComponent<TankNodeComponent>()->SetNeighbour(TankDirection::Down, node1);
	node2->GetComponent<TankNodeComponent>()->SetNeighbour(TankDirection::Right, node3);
	node2->GetComponent<TankNodeComponent>()->SetNeighbour(TankDirection::Left, node4);
	node3->GetComponent<TankNodeComponent>()->SetNeighbour(TankDirection::Left, node2);
	node4->GetComponent<TankNodeComponent>()->SetNeighbour(TankDirection::Right, node2);

	//Init tanks
	blueTank->AddComponent<dae::Collider>()->InitializeBoxCollider(30, 30);
	//blueTank->AddComponent<TankComponent>()->Initialize(dae::InputManager::GetInstance().GetKeyboard(), 100.f, 3);

	redTank->AddComponent<dae::Collider>()->InitializeBoxCollider(30, 30);
	redTank->AddComponent<TankComponent>()->Initialize("gamepad_0", 200.f, 3, *playerBarrel, node1);
	redTank->SetTag(dae::make_sdbm_hash("Player"));
	

	auto worldTop = scene.CreateGameObject();
	worldTop->GetTransform().SetLocalPosition(512, 0);
	worldTop->AddComponent<dae::Collider>()->InitializeBoxCollider(1024, 10);
	auto worldBottom = scene.CreateGameObject();
	worldBottom->GetTransform().SetLocalPosition(512, 576);
	worldBottom->AddComponent<dae::Collider>()->InitializeBoxCollider(1024, 10);
	auto worldLeft = scene.CreateGameObject();
	worldLeft->GetTransform().SetLocalPosition(0, 288);
	worldLeft->AddComponent<dae::Collider>()->InitializeBoxCollider(10, 576);
	auto worldRight = scene.CreateGameObject();
	worldRight->GetTransform().SetLocalPosition(1024, 288);
	worldRight->AddComponent<dae::Collider>()->InitializeBoxCollider(10, 576);

	// Load sounds
	dae::ServiceLocator::GetSoundSystem().LoadSound("Shot.wav");
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

	auto& factory = engine.GetComponentFactory();

	factory.Register<dae::TextureComponent>("TextureComponent");
	factory.Register<dae::TextComponent>("TextComponent");
	factory.Register<HealthComponent>("HealthComponent");
	factory.Register<PointsComponent>("PointsComponent");
	factory.Register<HealthStatsComponent>("HealthStatsComponent");
	factory.Register<PointsStatsComponent>("PointsStatsComponent");
	factory.Register<FPSCounter>("FPSCounter");
	factory.Register<BarrelComponent>("BarrelComponent");
	factory.Register<TankComponent>("TankComponent");
	factory.Register<dae::Collider>("Collider");
	factory.Register<dae::SpriteComponent>("SpriteComponent");
	factory.Register<dae::AnimatedSpriteComponent>("AnimatedSpriteComponent");
	factory.Register<ButtonComponent>("ButtonComponent");
	factory.Register<UINavigator>("UINavigator");
	factory.Register<TankNodeComponent>("TankNodeComponent");
	factory.Register<NodeMovementComponent>("NodeMovementComponent");
	factory.Register<EnemyTankComponent>("EnemyTankComponent");
	factory.Register<TankSpawnerComponent>("TankSpawnerComponent");

	// Load input maps
	dae::InputManager& inputManager = dae::InputManager::GetInstance();
	inputManager.LoadDeviceMapsFromFile((data_location / "InputMaps.json").string());

	//engine.Run(load);
	auto& sceneManager = dae::SceneManager::GetInstance();
	sceneManager.CreatePersistentObject()->AddComponent<GameStateMachine>()->SetEngine(&engine);
	engine.Run("Level1.json");

    return 0;
}
