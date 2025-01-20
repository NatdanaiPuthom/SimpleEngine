#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class Build final : public PopUp
	{
	public:
		Build(const std::string& aName);
		void Render() override;
	};
}
