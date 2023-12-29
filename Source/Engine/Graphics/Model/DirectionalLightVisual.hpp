#pragma once

namespace Drawer
{
	struct Line;
}

class ModelInstance;

class DirectionalLightVisual
{
public:
	DirectionalLightVisual();

	void Update() const;
	void Render() const;

	std::unique_ptr<Drawer::Line> myLine;
	std::shared_ptr<ModelInstance> myDirectionalLight;
	float myLineDistance;
};