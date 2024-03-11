#pragma once
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Model/Model.hpp"

namespace Simple
{
	class DirectionalLightVisual final
	{
	public:
		DirectionalLightVisual();
		~DirectionalLightVisual();

		void Update() const;
		void UpdateAxisLines() const;

		std::unique_ptr<Drawer::Line> myLine;
		std::shared_ptr<Simple::Model> myModel;
		float myLineDistance;

		std::vector<std::unique_ptr<Drawer::Line>> myLineAxis;
	};
}