#include "Minigin/Scene.h"
#include "Minigin/Loading/ComponentFactory.h"
#include <nlohmann/json.hpp>
#include "Minigin/Loading/LoadingStructs.h"
using Json = nlohmann::json;

namespace dae
{
    class SceneLoader
    {
    public:
		SceneLoader() = delete;
		SceneLoader(ComponentFactory& factory);

		~SceneLoader() = default;

        void LoadFromFile(const std::string& path, Scene& scene);

        static ParamMap ParseParams(const Json& params);

    private:
        void ParseScene(const Json& root, Scene& scene);

        void CreateObjects(const Json& objects, Scene& scene);

        void CreateComponents(const Json& objJson, GameObject& go);

        void LoadComponents();

        void FinalizeParams(ParamMap& params);

        ComponentFactory& m_factory;

		std::unordered_map<std::string, GameObject*> m_gameObjectByName;
		std::vector<std::tuple<Component*, ParamMap>> m_loadableComponents;


        Json LoadJsonFile(const std::string& path);
    };
}
