#include "PrefabManager.h"

#include "LoadingHelpers.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>

void dae::PrefabManager::SetComponentFactory(ComponentFactory& factory)
{
	m_pComponentFactory = &factory;
}

void dae::PrefabManager::SetDataPath(const std::filesystem::path& dataPath)
{
	m_dataPath = dataPath;
}

dae::GameObject* dae::PrefabManager::Instantiate(const std::string& name, Scene& scene, const glm::vec3& worldPosition, const ParamMap& overrides)
{
	if (!m_pComponentFactory)
		throw std::runtime_error("PrefabManager needs a ComponentFactory before instantiating prefabs.");

	const auto prefabIt = m_prefabs.find(name);
	if (prefabIt == m_prefabs.end())
	{
		m_prefabs[name] = LoadPrefabFile(name);
	}

	const Json& prefabJson = m_prefabs.at(name);
	const Json& objects = prefabJson["objects"];
	if (!objects.is_array() || objects.empty())
		throw std::runtime_error("Prefab '" + name + "' has no objects.");

	std::vector<GameObject*> createdObjects;
	std::unordered_map<std::string, GameObject*> objectByName;

	for (const auto& objJson : objects)
	{
		GameObject* object = scene.CreateGameObject();
		createdObjects.push_back(object);

		if (!objJson.contains("name"))
			throw std::runtime_error("Prefab '" + name + "' contains an object without a name.");

		objectByName[objJson["name"].get<std::string>()] = object;
	}

	for (size_t index = 0; index < objects.size(); ++index)
	{
		const Json& objJson = objects[index];
		GameObject* object = createdObjects[index];

		if (objJson.contains("parent"))
		{
			const std::string parentName = objJson["parent"].get<std::string>();
			if (!parentName.empty())
			{
				const auto parentIt = objectByName.find(parentName);
				if (parentIt == objectByName.end())
					throw std::runtime_error("Prefab parent '" + parentName + "' not found.");

				object->SetParent(parentIt->second, false);
			}
		}

		if (objJson.contains("tag"))
		{
			object->SetTag(objJson["tag"].get<std::string>());
		}

		if (objJson.contains("position"))
		{
			const auto& pos = objJson["position"];
			object->GetTransform().SetLocalPosition(pos[0], pos[1]);
		}

		if (objJson.contains("components"))
		{
			for (const auto& compJson : objJson["components"])
			{
				const std::string type = compJson["type"];

				if (!m_pComponentFactory->Has(type))
					continue;

				Component* component = m_pComponentFactory->Create(type, *object);

				if (!component || !compJson.contains("params"))
					continue;

				ParamMap params = ParseParams(compJson["params"]);
				FinalizeParams(params, overrides, objectByName);
				component->Load(params);
			}
		}
	}

	GameObject* root = createdObjects.front();
	root->GetTransform().SetWorldPosition(worldPosition);
	return root;
}

dae::Json dae::PrefabManager::LoadPrefabFile(const std::string& file) const
{
	const std::filesystem::path path = m_dataPath / file;
	std::ifstream stream(path);
	if (!stream.is_open())
		throw std::runtime_error("Could not open prefab file: " + path.string());

	Json prefab;
	stream >> prefab;
	return prefab;
}

dae::ParamMap dae::PrefabManager::ParseParams(const Json& params) const
{
	ParamMap out;

	if (!params.is_object())
		return out;

	for (auto it = params.begin(); it != params.end(); ++it)
	{
		const std::string key = it.key();
		const Json& value = it.value();

		if (value.is_string())
			out[key] = value.get<std::string>();
		else if (value.is_number())
		{
			if (value.is_number_float() || value.get<double>() != static_cast<double>(value.get<int>()))
				out[key] = value.get<float>();
			else
				out[key] = value.get<int>();
		}
		else if (value.is_boolean())
			out[key] = value.get<bool>();
		else if (value.is_array())
		{
			if (std::all_of(value.begin(), value.end(), [](const Json& item) { return item.is_string(); }))
				out[key] = value.get<std::vector<std::string>>();
			else
				out[key] = value.get<std::vector<int>>();
		}
	}

	return out;
}

void dae::PrefabManager::FinalizeParams(ParamMap& params, const ParamMap& overrides, const std::unordered_map<std::string, GameObject*>& objectByName) const
{
	for (auto& [key, param] : params)
	{
		if (std::holds_alternative<std::string>(param))
		{
			const std::string str = std::get<std::string>(param);

			if (str.starts_with("$"))
			{
				const std::string overrideName = str.substr(1);
				const auto overrideIt = overrides.find(overrideName);
				if (overrideIt == overrides.end())
					throw std::runtime_error("Prefab override '" + overrideName + "' not found for parameter '" + key + "'.");

				param = overrideIt->second;
				continue;
			}

			if (str.starts_with("GO_"))
			{
				const std::string objectName = str.substr(3);
				const auto objectIt = objectByName.find(objectName);
				if (objectIt == objectByName.end())
					throw std::runtime_error("Prefab GameObject '" + objectName + "' not found for parameter '" + key + "'.");

				param = objectIt->second;
			}
		}
		else if (std::holds_alternative<std::vector<std::string>>(param))
		{
			const std::vector<std::string>& strings = std::get<std::vector<std::string>>(param);
			const bool containsGameObjectReference = std::any_of(
				strings.begin(),
				strings.end(),
				[](const std::string& str) { return str.starts_with("GO_"); });

			if (!containsGameObjectReference)
				continue;

			std::vector<GameObject*> gameObjects;

			for (const std::string& str : strings)
			{
				if (str.empty())
					continue;

				if (!str.starts_with("GO_"))
					throw std::runtime_error("Invalid prefab GameObject reference '" + str + "' for parameter '" + key + "'.");

				const std::string objectName = str.substr(3);
				const auto objectIt = objectByName.find(objectName);
				if (objectIt == objectByName.end())
					throw std::runtime_error("Prefab GameObject '" + objectName + "' not found for parameter '" + key + "'.");

				gameObjects.push_back(objectIt->second);
			}

			param = gameObjects;
		}
	}
}
