#pragma once

namespace Drawer
{
	struct Line;
}

namespace Simple
{
	class Model;
}

namespace Simple
{
	class DirectionalLightVisual final
	{
	public:
		DirectionalLightVisual();

		void Update() const;

		std::unique_ptr<Drawer::Line> myLine;
		std::shared_ptr<Simple::Model> myModel;
		float myLineDistance;
	};
}