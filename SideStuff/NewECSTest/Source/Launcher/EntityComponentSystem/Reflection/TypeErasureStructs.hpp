#pragma once
#include <string>
#include <vector>
#include <limits>

namespace Simple
{
	struct ComponentProperty final
	{
		std::string name = "Unknown Property Name";
		std::string customVariableName = "Unknown Custom Property Name";

		size_t id = Invalid;
		size_t byteOffset = 0;

		bool isVisible = true;
		bool isEditable = true;

		static constexpr size_t Invalid = std::numeric_limits<size_t>::max();
	};

	struct TypeErasureComponentObject final
	{
		std::string name;
		std::string formattedName;
		std::vector<ComponentProperty> properties;
		size_t size = 0;
		bool isAdded = false;
	};
}