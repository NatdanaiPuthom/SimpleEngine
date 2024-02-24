#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Special/DirectionalLightVisual.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Engine/Global.hpp"

namespace Simple
{
	DirectionalLightVisual::DirectionalLightVisual()
		: myLine(std::make_unique<Drawer::Line>())
		, myLineDistance(5.0f)
	{
		Simple::ModelFactory* modelFactory = Global::GetModelFactory();

		std::shared_ptr<Simple::Model> directionalLight = std::move(modelFactory->CreateCubeModel());
		directionalLight->ClearTextures();
		directionalLight->AddTexture("Sunlight.dds");
		directionalLight->SetName("Directional Light");
		myModel = directionalLight;

		myLine->startPosition = myModel->GetPosition();
		myLine->endPosition = myLine->startPosition;
		myLine->endPosition += myModel->GetMatrix().GetForward() * myLineDistance;
		myLine->color = { 1,1,0,1 };
	}

	void DirectionalLightVisual::Update() const
	{
		myLine->startPosition = myModel->GetPosition();
		myLine->endPosition = myLine->startPosition;

		auto graphicsEngine = Global::GetGraphicsEngine();
		const Math::Vector3f direction = graphicsEngine->GetDirectionalLightDirection() * 180.0f;
		myModel->LookAt(direction);

		myLine->endPosition += direction.GetNormalized() * myLineDistance;
	}
}