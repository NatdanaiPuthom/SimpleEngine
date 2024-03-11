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

		myLineAxis.resize(3);

		myLineAxis[0] = std::make_unique<Drawer::Line>();
		myLineAxis[1] = std::make_unique<Drawer::Line>();
		myLineAxis[2] = std::make_unique<Drawer::Line>();

		myLineAxis[0]->color = { 1.0f, 0.0f, 0.0f, 1.0f };
		myLineAxis[1]->color = { 0.0f, 1.0f, 0.0f, 1.0f };
		myLineAxis[2]->color = { 0.0f, 0.0f, 1.0f, 1.0f };

		UpdateAxisLines();
	}

	DirectionalLightVisual::~DirectionalLightVisual()
	{
		myLineAxis.clear();
	}

	void DirectionalLightVisual::Update() const
	{
		myLine->startPosition = myModel->GetPosition();
		myLine->endPosition = myLine->startPosition;

		auto graphicsEngine = Global::GetGraphicsEngine();
		const Math::Vector3f direction = graphicsEngine->GetDirectionalLightDirection() * 180.0f;
		myModel->LookAt(direction);

		myLine->endPosition += direction.GetNormalized() * myLineDistance;

		UpdateAxisLines();
	}

	void DirectionalLightVisual::UpdateAxisLines() const
	{
		myLineAxis[0]->startPosition = myModel->GetPosition();
		myLineAxis[1]->startPosition = myModel->GetPosition();
		myLineAxis[2]->startPosition = myModel->GetPosition();

		myLineAxis[0]->startPosition.y += 2.0f;
		myLineAxis[1]->startPosition.y += 2.0f;
		myLineAxis[2]->startPosition.y += 2.0f;

		myLineAxis[0]->endPosition = myLineAxis[0]->startPosition;
		myLineAxis[1]->endPosition = myLineAxis[1]->startPosition;
		myLineAxis[2]->endPosition = myLineAxis[2]->startPosition;

		myLineAxis[0]->endPosition += Math::Vector3{ 1.0f, 0.0f, 0.0f } * myLineDistance;
		myLineAxis[1]->endPosition += Math::Vector3{ 0.0f, 1.0f, 0.0f } * myLineDistance;
		myLineAxis[2]->endPosition += Math::Vector3{ 0.0f, 0.0f, 1.0f } * myLineDistance;
	}
}