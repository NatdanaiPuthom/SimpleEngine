#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/DirectionalLightVisual.hpp"

DirectionalLightVisual::DirectionalLightVisual()
	: myLine(std::make_unique<Drawer::Line>())
	, myLineDistance(10.0f)
{
	ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

	std::shared_ptr<ModelInstance> directionalLight = std::move(modelFactory->CreateDirectionalLightModel());
	myDirectionalLight = directionalLight;

	myLine->startPosition = myDirectionalLight->GetPosition();
	myLine->endPosition = myLine->startPosition;
	myLine->endPosition += myDirectionalLight->GetMatrix().GetForward()* myLineDistance;
	myLine->color = { 1,1,0,1 };
}

void DirectionalLightVisual::Update() const
{
	myLine->startPosition = myDirectionalLight->GetPosition();
	myLine->endPosition = myLine->startPosition;
	myLine->endPosition += myDirectionalLight->GetMatrix().GetForward() * myLineDistance;
}

void DirectionalLightVisual::Render() const
{
	auto renderer = SimpleGlobal::GetRenderer();

	renderer->RenderModel(myDirectionalLight);
	renderer->RenderBoundingBox(myDirectionalLight);
	renderer->RenderLine(*myLine);
}