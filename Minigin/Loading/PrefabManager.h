#pragma once

#include "ComponentFactory.h"
#include "LoadingStructs.h"
#include "Minigin/Scene.h"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace dae
{
	using Json = nlohmann::json;

	class PrefabManager final
	{
	public:
		PrefabManager() = default;

		void SetComponentFactory(ComponentFactory& factory);
		void SetDataPath(const std::filesystem::path& dataPath);

		GameObject* Instantiate(
			const std::string& name,
			Scene& scene,
			const glm::vec3& worldPosition,
			const ParamMap& overrides = {}
		);

	private:
		Json LoadPrefabFile(const std::string& file) const;
		ParamMap ParseParams(const Json& params) const;
		void FinalizeParams(
			ParamMap& params,
			const ParamMap& overrides,
			const std::unordered_map<std::string, GameObject*>& objectByName
		) const;

		ComponentFactory* m_pComponentFactory{ nullptr };
		std::filesystem::path m_dataPath;
		std::unordered_map<std::string, Json> m_prefabs;
	};
}
