#include "Scene.h"
#include "Loading/ComponentFactory.h"
#include <nlohmann/json.hpp>
#include "Loading/LoadingStructs.h"
using Json = nlohmann::json;

namespace dae
{

	class IComponentLoadable;
    class SceneLoader
    {
    public:
		SceneLoader() = delete;
		SceneLoader(ComponentFactory& factory);

		~SceneLoader() = default;

        Scene& LoadFromFile(const std::string& path);

    private:
        void ParseScene(const Json& root, Scene& scene);

        void CreateObjects(const Json& objects, Scene& scene);

        void CreateComponents(const Json& objJson, GameObject& go);

        void LoadComponents();

        void ResolveLinks();

        ComponentFactory& m_factory;

		std::unordered_map<std::string, GameObject*> m_gameObjectByName;
		std::vector<std::tuple<IComponentLoadable*, ParamMap>> m_loadableComponents;

        std::vector<LinkRequest> m_pendingLinks;


        Json LoadJsonFile(const std::string& path);
    };
}
