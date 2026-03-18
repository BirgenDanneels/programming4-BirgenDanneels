#pragma once

#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace dae
{
	class GameObject;
	class Component;

	using EventArg = std::variant<
		int,
		float,
		bool,
		std::string,
		GameObject*,
		Component*
	>;

	constexpr unsigned int make_sdbm_hash(const char* str)
	{
		unsigned int hash = 0;
		int c;
		while ((c = *str++)) {
			hash = c + (hash << 6) + (hash << 16) - hash;
		}
		return hash;
	}

	struct Event final
	{
		unsigned int id{};
		std::vector<EventArg> args{};

		Event() = default;

		explicit Event(unsigned int eventId)
			: id(eventId)
		{
		}

		Event(unsigned int eventId, std::initializer_list<EventArg> payload)
			: id(eventId), args(payload)
		{
		}
	};
}