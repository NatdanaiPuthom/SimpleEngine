#pragma once

namespace Drawer
{
	struct Line;
}

class Model;

class DirectionalLightVisual final
{
public:
	DirectionalLightVisual();

	void Update() const;

	std::unique_ptr<Drawer::Line> myLine;
	std::shared_ptr<Model> myModel;
	float myLineDistance;
};