#include "Minigin/Scene.h"
#include "Minigin/Loading/ComponentFactory.h"
#include <nlohmann/json.hpp>
#include "Minigin/Loading/LoadingStructs.h"
#include "Minigin/Events/Observer.h"
#include "Minigin/Events/Subject.h"
#include <latch>
using Json = nlohmann::json;

namespace dae
{
	class SceneLoader final: public Observer<>
    {
    public:
		SceneLoader() = delete;
		SceneLoader(ComponentFactory& factory);

		~SceneLoader() = default;

        void LoadFromFile(const std::string& path, Scene& scene);

        static ParamMap ParseParams(const Json& params);

        void OnNotify() override;

    private:

        void ParseScene(const Json& root, Scene& scene);

        void LoadSounds(const Json& sounds);

		void WaitForSoundsToLoad();

        void CreateObjects(const Json& objects, Scene& scene);

        void CreateComponents(const Json& objJson, GameObject& go);

        void LoadComponents();

        void FinalizeParams(ParamMap& params);

        ComponentFactory& m_factory;

		std::unordered_map<std::string, GameObject*> m_gameObjectByName;
		std::vector<std::tuple<Component*, ParamMap>> m_loadableComponents;

        std::latch m_soundsLoadedLatch{1}; //Used to block loading until all sounds are loaded.
        ObserverHandle m_soundLoadedHandle;


        Json LoadJsonFile(const std::string& path);
    };
}
