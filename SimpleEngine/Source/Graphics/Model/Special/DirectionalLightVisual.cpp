#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Special/DirectionalLightVisual.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Engine/Global.hpp"

namespace Simple
{
	DirectionalLightVisual::DirectionalLightVisual()
		: myLightDirectionLine(std::make_unique<Drawer::Line>())
		, myLineDistance(5.0f)
	{
		Simple::ModelFactory* modelFactory = Global::GetModelFactory();

		std::shared_ptr<Simple::Model> directionalLight = std::move(modelFactory->CreateCubeModel());
		directionalLight->ClearTextures();
		directionalLight->AddTexture("Sunlight.dds");
		directionalLight->SetName("Directional Light");
		myModel = directionalLight;

		myLightDirectionLine->startPosition = myModel->GetPosition();
		myLightDirectionLine->endPosition = myLightDirectionLine->startPosition;
		myLightDirectionLine->endPosition += myModel->GetMatrix().GetForward() * myLineDistance;
		myLightDirectionLine->color = { 1,1,0,1 };

		myLineAxis.resize(3);
		mySpheres.resize(3);

		myLineAxis[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };
		myLineAxis[1].color = { 0.0f, 1.0f, 0.0f, 1.0f };
		myLineAxis[2].color = { 0.0f, 0.0f, 1.0f, 1.0f };

		mySpheres[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };
		mySpheres[1].color = { 0.0f, 1.0f, 0.0f, 1.0f };
		mySpheres[2].color = { 0.0f, 0.0f, 1.0f, 1.0f };

		mySpheres[0].radius = 0.2f;
		mySpheres[1].radius = 0.2f;
		mySpheres[2].radius = 0.2f;

		UpdateDebugLines();
	}

	DirectionalLightVisual::~DirectionalLightVisual()
	{
		myLineAxis.clear();
	}

	void DirectionalLightVisual::Update()
	{
		myLightDirectionLine->startPosition = myModel->GetPosition();
		myLightDirectionLine->endPosition = myLightDirectionLine->startPosition;

		auto graphicsEngine = Global::GetGraphicsEngine();
		const Math::Vector3f direction = graphicsEngine->GetDirectionalLightDirection() * 180.0f;
		myModel->LookAt(direction);

		myLightDirectionLine->endPosition += direction.GetNormalized() * myLineDistance;

		UpdateDebugLines();
	}

	void DirectionalLightVisual::Render() const
	{
		auto renderer = Global::GetRenderer();

		renderer->RenderLine(*myLightDirectionLine);
		renderer->RenderLine(myLineAxis);

		for (const auto& sphere : mySpheres)
		{
			renderer->RenderSphere(sphere);
		}
	}

	void DirectionalLightVisual::UpdateDebugLines()
	{
		myLineAxis[0].startPosition = myModel->GetPosition();
		myLineAxis[1].startPosition = myModel->GetPosition();
		myLineAxis[2].startPosition = myModel->GetPosition();

		myLineAxis[0].startPosition.y += 2.0f;
		myLineAxis[1].startPosition.y += 2.0f;
		myLineAxis[2].startPosition.y += 2.0f;

		myLineAxis[0].endPosition = myLineAxis[0].startPosition;
		myLineAxis[1].endPosition = myLineAxis[1].startPosition;
		myLineAxis[2].endPosition = myLineAxis[2].startPosition;

		myLineAxis[0].endPosition += Math::Vector3{ 1.0f, 0.0f, 0.0f } * myLineDistance;
		myLineAxis[1].endPosition += Math::Vector3{ 0.0f, 1.0f, 0.0f } * myLineDistance;
		myLineAxis[2].endPosition += Math::Vector3{ 0.0f, 0.0f, 1.0f } * myLineDistance;

		mySpheres[0].position = myLineAxis[0].endPosition;
		mySpheres[1].position = myLineAxis[1].endPosition;
		mySpheres[2].position = myLineAxis[2].endPosition;
	}
}