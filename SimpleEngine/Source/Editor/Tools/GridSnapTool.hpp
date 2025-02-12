#pragma once

namespace Editor
{
	class TransformEntityTool;
}

namespace Editor
{

	class GridSnapTool final
	{
	public:

		GridSnapTool(TransformEntityTool* aTransformEntityTool);

		void Render();

	private:

		TransformEntityTool* myTransformEntityTool = nullptr;
	};

}