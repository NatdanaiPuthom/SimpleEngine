#pragma once

namespace Drawer
{
	struct Line;
}

class ModelInstance;

class DirectionalLightVisual final
{
public:
	DirectionalLightVisual();

	void Update() const;

	std::unique_ptr<Drawer::Line> myLine;
	std::shared_ptr<ModelInstance> myModel;
	float myLineDistance;
};