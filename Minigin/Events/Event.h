#pragma once
#include <string>
#include <variant>
#include <vector>
#include "Minigin/Utils/HashUtil.h"

namespace dae
{
	class GameObject;
	class Component;

	using EventID = unsigned int;

	using EventArg = std::variant<
		int,
		float,
		bool,
		std::string,
		GameObject*,
		Component*
	>;

	class Event final
	{
	public:
		Event() = default;

		explicit Event(EventID id)
			: m_id(id)
		{
		}

		Event(EventID id, std::initializer_list<EventArg> payload)
			: m_id(id), m_args(payload)
		{
		}

		template <size_t N>
		static consteval EventID ToEventID(const char(&eventName)[N])
		{
			return make_sdbm_hash(eventName);
		}

		EventID GetId() const noexcept { return m_id; }
		const std::vector<EventArg>& GetArgs() const noexcept { return m_args; }

	private:

		EventID m_id{};
		std::vector<EventArg> m_args{};
	};
}