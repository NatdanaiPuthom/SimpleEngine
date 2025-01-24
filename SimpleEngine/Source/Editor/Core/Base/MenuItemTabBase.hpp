#pragma once
#include <string>

namespace Editor
{
	class MainMenuTabBase
	{
	public:
		MainMenuTabBase(const char* aName) : myName(aName) {}

		virtual ~MainMenuTabBase() = default;
		virtual void Init() {};
		virtual void Render() = 0;

	protected:
		std::string myName;
	};
}
