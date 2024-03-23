#pragma once
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Model/Model.hpp"

namespace Simple
{
	class DirectionalLightVisual final
	{
	public:
		std::unique_ptr<Drawer::Line> myLightDirectionLine;
		std::shared_ptr<Simple::Model> myModel;

		float myLineDistance;

		std::vector<Drawer::Line> myLineAxis;
		std::vector<Drawer::Sphere> mySpheres;

	public:
		DirectionalLightVisual();
		~DirectionalLightVisual();

		void Update();
		void Render() const;

	private:
		void UpdateDebugLines();
	};
}