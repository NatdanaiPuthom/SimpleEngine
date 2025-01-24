#pragma once
#include <string>

namespace Editor
{
	class MenuItem;
}

namespace Editor
{
	template <typename T>
	concept DerivedFromMenuItem = std::is_base_of_v<MenuItem, T>&& std::is_class_v<T>;

	class MenuItem
	{
	public:
		MenuItem(const char* aName) : myName(aName) {}

		virtual ~MenuItem() = default;
		virtual void Render() = 0;

		const std::string& GetName() const { return myName; }
	protected:
		std::string myName;
	};
}
