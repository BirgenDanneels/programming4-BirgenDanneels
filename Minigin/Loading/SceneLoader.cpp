#include "SceneLoader.h"
#include <fstream>
#include "SceneManager.h"
#include "LoadingHelpers.h"

dae::SceneLoader::SceneLoader(ComponentFactory& factory)
	: m_factory(factory)
{
}

void dae::SceneLoader::LoadFromFile(const std::string& path, Scene& scene)
{
    Json root = LoadJsonFile(path);
    ParseScene(root, scene);
}

void dae::SceneLoader::ParseScene(const Json& root, Scene& scene)
{
    auto& objects = root["scene"]["objects"];

    CreateObjects(objects, scene);

    LoadComponents();
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
            out[key] = value.get<std::vector<int>>();
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
