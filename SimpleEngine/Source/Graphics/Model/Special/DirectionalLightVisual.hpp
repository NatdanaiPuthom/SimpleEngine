#pragma once
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Model/Model.hpp"

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