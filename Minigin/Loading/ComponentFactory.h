#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include "Components/Component.h"
#include "GameObject.h"

namespace dae
{
    class ComponentFactory
    {
    public:

		ComponentFactory() = default;
		~ComponentFactory() = default;

        template<typename T>
        void Register(const std::string& type)
        {
            ComponentEntry entry;

            entry.create = [](GameObject& obj)
                {
                    return obj.AddComponent<T>();
                };

            entry.get = [](GameObject& obj)
                {
                    return obj.GetComponent<T>();
                };

            m_entries[type] = entry;
        }

        bool Has(const std::string& type) const
        {
            return m_entries.find(type) != m_entries.end();
        }

        Component* Create(const std::string& type, GameObject& obj)
        {
            return m_entries.at(type).create(obj);
        }

        Component* Get(const std::string& type, GameObject& obj)
        {
            return m_entries.at(type).get(obj);
		}

        std::vector<std::string> GetRegisteredTypes() const
        {
            std::vector<std::string> types;
            types.reserve(m_entries.size());
            for (const auto& pair : m_entries)
            {
                types.push_back(pair.first);
            }
            return types;
        }

    private:
        struct ComponentEntry
        {
            std::function<Component*(GameObject&)> create;
            std::function<Component*(GameObject&)> get;
        };

        std::unordered_map<std::string, ComponentEntry> m_entries;
    };
}