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
	blueTank->AddComponent<dae::GameActor>()->Initialize(true, 100.f);

	auto redTank = scene.CreateGameObject();
	redTank->SetWorldPosition(300, 100);
	redTank->AddComponent<dae::TextureComponent>()->Initialize("RedTank.png");
	redTank->AddComponent<dae::GameActor>()->Initialize(false, 200.f);

	// Controls Text
	auto font2 = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);

	auto textAnchor = scene.CreateGameObject();
	textAnchor->SetLocalPosition(10, 100);

	auto wasdText = scene.CreateGameObject();
	wasdText->SetParent(textAnchor);
	wasdText->SetLocalPosition(0, 0);
	wasdText->AddComponent<dae::TextComponent>()->Initialize("Use WASD to move the blue tank.", font2);

	auto controllerText = scene.CreateGameObject();
	controllerText->SetParent(textAnchor);
	controllerText->SetLocalPosition(0, 20);
	controllerText->AddComponent<dae::TextComponent>()->Initialize("Use D-Pad or left stick to move the red tank.", font2);
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
