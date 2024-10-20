#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class BuildMenuBar : public ToolInterface
	{
	public:
		BuildMenuBar();
		~BuildMenuBar();

		void Init() override;
		void Update() override;
		void Draw() override;
	};
}
