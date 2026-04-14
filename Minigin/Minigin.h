#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <chrono>
#include "Loading/ComponentFactory.h"

namespace dae
{
	class Minigin final
	{
	private:
		bool m_quit{};
		
		float m_deltaTime{};
		float m_fixedTimeStep{ 1.f / 60.f }; // 60 fps
		float m_FPS{0 };
		float m_lag{};
		std::chrono::steady_clock::time_point m_lastTime{};
		ComponentFactory m_componentFactory{};
		std::filesystem::path m_dataPath{};

	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void Run(const std::string& sceneFile);
		void RunOneFrame();

		ComponentFactory& GetComponentFactory() { return m_componentFactory; }

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}