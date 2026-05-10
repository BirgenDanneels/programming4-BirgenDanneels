#include "SceneLoader.h"
#include <fstream>
#include "SceneManager.h"
#include "Loading/Interfaces/IComponentLoadable.h"
#include "LoadingHelpers.h"

dae::SceneLoader::SceneLoader(ComponentFactory& factory)
	: m_factory(factory)
{
}

dae::Scene& dae::SceneLoader::LoadFromFile(const std::string& path)
{
    Json root = LoadJsonFile(path);
    auto& scene = dae::SceneManager::GetInstance().CreateScene();
    ParseScene(root, scene);
    return scene;
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

        // Collect parameters and put them in a vector with the component for later loading. (May change to being loaded immediately)
        if (auto* loadable = dynamic_cast<IComponentLoadable*>(comp))
        {
            if (compJson.contains("params"))
            {
                ParamMap params = ParseParams(compJson["params"]);
				m_loadableComponents.emplace_back(loadable, params);
            }
        }
    }
}

void dae::SceneLoader::LoadComponents()
{
    for (auto& [loadable, params] : m_loadableComponents)
    {
        loadable->Load(params);
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

	return root;
}
