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

	auto background = std::make_unique<dae::GameObject>();
	background->SetLocalPosition(0, 0);
	background->AddComponent<dae::TextureComponent>()->Initialize("background.png");
	scene.Add(std::move(background));

	auto logo = std::make_unique<dae::GameObject>();
	logo->SetLocalPosition(358, 180);
	logo->AddComponent<dae::TextureComponent>()->Initialize("logo.png");
	scene.Add(std::move(logo));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 25);

	auto progText = std::make_unique<dae::GameObject>();
	progText->SetLocalPosition(292, 20);
	auto progTextComp = progText->AddComponent<dae::TextComponent>();
	progTextComp->Initialize("Programming 4 Assignment", font);
	progTextComp->SetColor({ 255, 255, 0, 255 });
	scene.Add(std::move(progText));

	auto fpsCounter = std::make_unique<dae::GameObject>();
	fpsCounter->SetLocalPosition(5, 5);
	fpsCounter->AddComponent<dae::TextComponent>()->Initialize("0", font);
	fpsCounter->AddComponent<dae::FPSCounter>()->Initialize();
	scene.Add(std::move(fpsCounter));

	//Rotation objects

	auto pivot = std::make_unique<dae::GameObject>();
	pivot->SetWorldPosition(100, 250);

	auto blueTank = std::make_unique<dae::GameObject>();
	blueTank->SetWorldPosition(100, 280);
	blueTank->AddComponent<dae::TextureComponent>()->Initialize("BlueTank.png");
	blueTank->AddComponent<dae::RotatorComponent>()->Initialize(*pivot.get(), 1.73f);

	auto redTank = std::make_unique<dae::GameObject>();
	redTank->SetParent(blueTank.get());
	redTank->SetLocalPosition(50, 0);
	redTank->AddComponent<dae::TextureComponent>()->Initialize("RedTank.png");
	redTank->AddComponent<dae::RotatorComponent>()->Initialize(*blueTank.get(), 1.25f);


	scene.Add(std::move(pivot));
	scene.Add(std::move(blueTank));
	scene.Add(std::move(redTank));
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
