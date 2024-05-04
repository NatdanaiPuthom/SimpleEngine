#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Global.hpp"
#include "Engine/Math/Math.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Game/world.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include <External/nlohmann/json.hpp>
#include <fstream>
#include <cassert>

#include "Engine/Components/Core/MeshComponent.hpp"
#include "Engine/Components/Core/TransformComponent.hpp"
#include "Engine/Components/Core/AnimatedComponent.hpp"

namespace Drawer
{
	using namespace Simple;

	void Renderer::TestRender(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent)
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aTransformComponent->transform.GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aMeshComponent->shader->BindThisShader(context.Get());
		aMeshComponent->texture->Bind(context, aMeshComponent->texture->GetSlot());

		UINT stride = sizeof(Graphics::Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aMeshComponent->mesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aMeshComponent->mesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aMeshComponent->mesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::TestRenderAnimated(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent, ECS::AnimatedComponent* aSkeletonComponent)
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aTransformComponent->transform.GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		BonesBufferData boneBufferData = {};

		for (size_t i = 0; i < SIMPLE_MAX_BONES; ++i)
		{
			boneBufferData.bonesTransform[i] = aSkeletonComponent->jointMatrices[i];;
		}

		myBoneBuffer->Bind(myBoneBuffer->GetSlot());
		myBoneBuffer->Update(sizeof(BonesBufferData), &boneBufferData);

		aSkeletonComponent->shader->BindThisShader(context.Get());
		aMeshComponent->texture->Bind(context, aMeshComponent->texture->GetSlot());

		UINT stride = sizeof(Graphics::Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aMeshComponent->mesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aMeshComponent->mesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aMeshComponent->mesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	Renderer::Renderer()
		: myDebugMode(false)
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Init()
	{
		myObjectBuffer = std::make_unique<Graphics::ConstantBuffer>();
		myBoneBuffer = std::make_unique<Graphics::ConstantBuffer>();

		myBoundingBoxDrawer = std::make_unique<Drawer::BoundingBoxDrawer>();
		myLineDrawer = std::make_unique<Drawer::LineDrawer>();
		mySphereDrawer = std::make_unique<Drawer::SphereDrawer>();
		mySpriteDrawer = std::make_unique<Drawer::SpriteDrawer>();

		LoadSettingsFromJson();

		if (!CreateObjectBuffer())
			assert(false && "Failed to create ObjectBuffer");

		if (!CreateBoneBuffer())
			assert(false && "Failed to create ObjectBuffer");

		myBoundingBoxDrawer->Init();

		myObjectBuffer->SetSlot(1);
		myBoneBuffer->SetSlot(5);
	}

	void Renderer::RenderModel(const std::shared_ptr<const Graphics::Model> aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->BindThisShader(context.Get());

		for (const auto& texture : aModel->myTextures)
		{
			texture->Bind(context, texture->GetSlot());
		}

		UINT stride = sizeof(Graphics::Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModel->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModel->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModel->myMesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderLine(const Drawer::Line& aLine)
	{
		myLineDrawer->Render(aLine);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderLine(const std::vector<Drawer::Line> aLines)
	{
		const size_t sizeLimit = myLineDrawer->GetInstanceSizeLimit();

		if (aLines.size() * 2 < sizeLimit)
		{
			myLineDrawer->RenderInstance(aLines);

			Impl::SimpleGlobalRenderer::IncreaseDrawCall();
		}
		else
		{
			std::vector<std::vector<Drawer::Line>> lineSplit = Math::SplitVector(aLines, sizeLimit / 2);

			while (lineSplit.empty() == false)
			{
				myLineDrawer->RenderInstance(aLines);
				lineSplit.erase(lineSplit.begin());

				Impl::SimpleGlobalRenderer::IncreaseDrawCall();
			}
		}
	}

	void Renderer::RenderSphere(const Drawer::Sphere& aSphere)
	{
		mySphereDrawer->Render(aSphere);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderSprite2D(const Drawer::Sprite2D& aSprite)
	{
		mySpriteDrawer->Render(aSprite);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const std::shared_ptr<const Graphics::Model> aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const std::shared_ptr<const Graphics::AnimatedModel> aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const Graphics::Model& aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const Graphics::AnimatedModel& aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::SetDebugMode(const bool aSetDebugMode)
	{
		myDebugMode = aSetDebugMode;
	}

	const bool Renderer::CreateObjectBuffer()
	{
		ObjectBufferData objectBuffer;

		if (!myObjectBuffer->Init(sizeof(ObjectBufferData), &objectBuffer))
			return false;

		return true;
	}

	const bool Renderer::CreateBoneBuffer()
	{
		BonesBufferData boneBufferData;

		if (!myBoneBuffer->Init(sizeof(BonesBufferData), &boneBufferData))
			return false;

		return true;
	}

	bool Renderer::IsDebugModeOn() const
	{
		return myDebugMode;
	}

	void Renderer::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		SetDebugMode(json["game_settings"]["debugMode"]);
	}
}