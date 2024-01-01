#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/Special/DirectionalLightVisual.hpp"

DirectionalLightVisual::DirectionalLightVisual()
	: myLine(std::make_unique<Drawer::Line>())
	, myLineDistance(5.0f)
{
	ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

	std::shared_ptr<Model> directionalLight = std::move(modelFactory->CreateCubeModel());
	directionalLight->ClearTextures();
	directionalLight->AddTexture("Sun.dds");
	directionalLight->SetName("Directional Light");
	myModel = directionalLight;

	myLine->startPosition = myModel->GetPosition();
	myLine->endPosition = myLine->startPosition;
	myLine->endPosition += myModel->GetMatrix().GetForward()* myLineDistance;
	myLine->color = { 1,1,0,1 };
}

void DirectionalLightVisual::Update() const
{
	myLine->startPosition = myModel->GetPosition();
	myLine->endPosition = myLine->startPosition;

	auto graphicsEngine = SimpleGlobal::GetGraphicsEngine();
	const SU::Vector3f direction = graphicsEngine->GetDirectionalLightDirection() * 180.0f;
	myModel->LookAt(direction);

	myLine->endPosition += direction.GetNormalized() * myLineDistance;
}