#pragma once
#include "Editor/Command/Commands/SetEntityTransformCommand.hpp"

namespace Editor
{

	class TransformEntityTool final
	{
	public:

		TransformEntityTool() = default;

		void Render();

	private:

		SetEntityTransformCommand mySetEntityTransformCommand;
		bool myIsDraggingEntity = false;
	};



}