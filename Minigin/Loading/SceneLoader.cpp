#include "SceneLoader.h"
#include <fstream>
#include <Minigin/Sound/ServiceLocator.h>
#include "SceneManager.h"
#include "LoadingHelpers.h"
#include <algorithm>
#include <filesystem>

dae::SceneLoader::SceneLoader(ComponentFactory& factory)
	: m_factory(factory)
{
}

void dae::SceneLoader::LoadFromFile(const std::string& path, Scene& scene)
{
    ServiceLocator::GetPrefabManager().SetDataPath(std::filesystem::path(path).parent_path());
    Json root = LoadJsonFile(path);
    ParseScene(root, scene);
}

void dae::SceneLoader::ParseScene(const Json& root, Scene& scene)
{
    auto& objects = root["scene"]["objects"];
    ServiceLocator::GetPrefabManager().SetComponentFactory(m_factory);

	if (root["scene"].contains("sounds"))
    {
        auto& sounds = root["scene"]["sounds"];
        LoadSounds(sounds);
    }
    else
        m_soundsLoadedLatch.count_down(); // No sounds to load, immediately allow loading to continue.

    CreateObjects(objects, scene);

	WaitForSoundsToLoad(); // Do this before loading components since its very well possible that components get sound_id's in their initialize/load function.

    LoadComponents();
}

void dae::SceneLoader::LoadSounds(const Json& sounds)
{
    m_soundLoadedHandle = ServiceLocator::GetSoundSystem().OnSoundsLoaded().AddObserver(this);

    for (const auto& soundJson : sounds)
    {
		ServiceLocator::GetSoundSystem().LoadSound(soundJson);
    }
}

void dae::SceneLoader::WaitForSoundsToLoad()
{
    m_soundsLoadedLatch.wait();
}

void dae::SceneLoader::OnNotify()
{
    m_soundsLoadedLatch.count_down();

    m_soundLoadedHandle.Unsubscribe();
}

void dae::SceneLoader::CreateObjects(const Json& objects, Scene& scene)
{
    for (const auto& objJson : objects)
    {
        GameObject* go = scene.CreateGameObject();

        if (objJson.contains("parent"))
        {
            std::string parentName = objJson["parent"];
             

            if (!parentName.empty() && m_gameObjectByName.find(parentName) != m_gameObjectByName.end())
            {
                // Set parent if found
                GameObject* parent = m_gameObjectByName[parentName];
                go->SetParent(parent);
            }
            else if (!parentName.empty())
				throw std::runtime_error("Parent GameObject '" + parentName + "' not found for object. Object will be created without parent.");
        }

        if(objJson.contains("name"))
        {
            std::string name = objJson["name"];
            m_gameObjectByName[name] = go;
        }
        else
			throw std::runtime_error("SceneLoader CreateObjects : 'name' parameter is not found.");

        if (objJson.contains("tag"))
        {
            std::string tagStr = objJson["tag"];
			go->SetTag(tagStr);
        }

        if (objJson.contains("position"))
        {
            auto& pos = objJson["position"];
            go->GetTransform().SetLocalPosition(pos[0], pos[1]);
        }

        // Create components
        CreateComponents(objJson, *go);
    }
}

void dae::SceneLoader::CreateComponents(const Json& objJson, GameObject& go)
{
    const Json& components = objJson["components"];

    for (const auto& compJson : components)
    {
        std::string type = compJson["type"];

        if (!m_factory.Has(type))
            continue; //Skip and log error

        // Create component via factory
        Component* comp = m_factory.Create(type, go);

        // Collect parameters and put them in a vector with the component for later loading.
        if (compJson.contains("params"))
        {
            ParamMap params = ParseParams(compJson["params"]);
			m_loadableComponents.emplace_back(comp, params);
        }
    }
}

void dae::SceneLoader::LoadComponents()
{
    for (auto& [loadable, params] : m_loadableComponents)
    {
        FinalizeParams(params);
        loadable->Load(params);
    }
}

dae::ParamMap dae::SceneLoader::ParseParams(const Json& params)
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
            // Try to parse as float first for consistency with numeric data
            if (value.is_number_float() || value.get<double>() != static_cast<double>(value.get<int>()))
            {
                out[key] = value.get<float>();
            }
            else
            {
                out[key] = value.get<int>();
            }
        }
        else if (value.is_boolean())
            out[key] = value.get<bool>();
        else if (value.is_array())
        {
            if (std::all_of(value.begin(), value.end(), [](const Json& item) { return item.is_string(); }))
            {
                out[key] = value.get<std::vector<std::string>>();
            }
            else
            {
                out[key] = value.get<std::vector<int>>();
            }
        }
    }

    return out;
}

void dae::SceneLoader::FinalizeParams(ParamMap& params)
{
	for (auto& [key, param] : params)
    {
        if (std::holds_alternative<std::string>(param))
        {
            std::string str = std::get<std::string>(param);
            if(str.starts_with("GO_"))
            {
                std::string goName = str.substr(3);
                if (m_gameObjectByName.find(goName) != m_gameObjectByName.end())
                {
                    param = m_gameObjectByName[goName];
                }
                else
                {
                    throw std::runtime_error("GameObject '" + goName + "' not found for parameter '" + key + "'.");
                }
            }
        }
        else if (std::holds_alternative<std::vector<std::string>>(param))
        {
            // Check for any GO references
            const std::vector<std::string>& strings = std::get<std::vector<std::string>>(param);
            const bool containsGameObjectReference = std::any_of(
                strings.begin(),
                strings.end(),
                [](const std::string& str) { return str.starts_with("GO_"); });

            if (!containsGameObjectReference)
                continue;

            std::vector<dae::GameObject*> gameObjects;

            for (const std::string& str : strings)
            {
                if (str.empty())
                    continue;

                if (!str.starts_with("GO_"))
                    throw std::runtime_error("Invalid GameObject reference '" + str + "' for parameter '" + key + "'.");

                std::string goName = str.substr(3);
                if (m_gameObjectByName.find(goName) != m_gameObjectByName.end())
                {
                    gameObjects.push_back(m_gameObjectByName[goName]);
                }
                else
                {
                    throw std::runtime_error("GameObject '" + goName + "' not found for parameter '" + key + "'.");
                }
            }

            param = gameObjects;
        }
    }
}



Json dae::SceneLoader::LoadJsonFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + path);

    Json root;
    file >> root;
    return root;
}
