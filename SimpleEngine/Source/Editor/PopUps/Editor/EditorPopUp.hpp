#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Editor/Tools/TransformEntityTool.hpp"
#include "Editor/Tools/GridSnapTool.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ECS
{
	class EntityComponentSystem;
	class Entity;
}

namespace Editor
{
	class EditorPopUp final : public PopUp
	{
	public:
		EditorPopUp(const std::string& aName);

		void Init() override;
		void Render() override;
	private:
	};
}
