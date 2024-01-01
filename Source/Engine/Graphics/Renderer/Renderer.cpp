#include "Engine/Precomplier/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Graphics/Renderer/BoundingBoxDrawer.hpp"
#include "Engine/Graphics/Renderer/LineDrawer.hpp"
#include "Engine/Graphics/Renderer/SphereDrawer.hpp"
#include "Game/world.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"

namespace Simple
{
	Renderer::Renderer()
		: myObjectBuffer(std::make_unique<ConstantBuffer>())
		, myBoundingBoxDrawer(std::make_unique<BoundingBoxDrawer>())
		, myLineDrawer(std::make_unique<LineDrawer>())
		, mySphereDrawer(std::make_unique<SphereDrawer>())
		, myDebugMode(false)
	{
		Impl::SimpleGlobalRenderer::SetRenderer(this);
		LoadSettingsFromJson();

		if (!CreateObjectBuffer())
			assert(false && "Failed to create ObjectBuffer");
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::RenderModel(const std::shared_ptr<const Model> aModelInstance) const
	{
		const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModelInstance->myShader->SetShader(context.Get());

		for (const auto& texture : aModelInstance->myTextures)
		{
			texture->Bind(context, texture->GetSlot());
		}

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModelInstance->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModelInstance->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModelInstance->myMesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderLine(const Drawer::Line& aLine)
	{
		myLineDrawer->Render(aLine);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderSphere(const Drawer::Sphere& aSphere)
	{
		mySphereDrawer->Render(aSphere);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const std::shared_ptr<const Model> aModelInstance) const
	{
		myBoundingBoxDrawer->Render(aModelInstance);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderEverythingUpSideDown() const
	{
		auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();

		const SU::Vector3f oldCamPosition = camera->GetPosition();
		const SU::Vector3f oldCamRotation = camera->GetRotation();
		const SU::Vector3f newCamRotation = SimpleUtilities::Vector3f(oldCamRotation.x, -oldCamRotation.y, oldCamRotation.z);

		const float waterHeight = SimpleWorld::GetWaterHeight();
		const float distFromWater = 2.0f * (oldCamPosition.y - waterHeight);

		camera->SetPosition(oldCamPosition - SimpleUtilities::Vector3f(0.0f, distFromWater, 0.0f));
		camera->SetRotation(newCamRotation);

		Simple::Renderer* renderer = SimpleGlobal::GetRenderer();
		for (const auto& model : SimpleWorld::GetActiveScene()->myModels)
		{
			renderer->RenderUpSideDown(model.get());
		}

		camera->SetRotation(oldCamRotation);
		camera->SetPosition(oldCamPosition);
	}

	void Renderer::RenderPlaneReflection(const Model* const aModelInstance) const
	{
		const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModelInstance->myShader->SetShader(context.Get());

		SimpleGlobal::GetGraphicsEngine()->GetContext()->PSSetShaderResources(0, 1, SimpleGlobal::GetGraphicsEngine()->GetWaterShaderResourceView().GetAddressOf());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModelInstance->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModelInstance->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModelInstance->myMesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	bool Renderer::IsDebugModeOn() const
	{
		return myDebugMode;
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

		myObjectBuffer->SetSlot(1);

		return true;
	}

	void Renderer::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetPath(SIMPLE_SETTINGS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		SetDebugMode(json["game_settings"]["debugMode"]);
	}

	void Renderer::RenderUpSideDown(const Model* const aModelInstance) const
	{
		const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

		SimpleUtilities::Matrix4x4f mirror = SimpleUtilities::Matrix4x4f::Identity();

		mirror(2, 2) = -1.0f;
		mirror(4, 2) = -2.0f;

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance->GetMatrix() * mirror;

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		auto mirrorShader = SimpleGlobal::GetGraphicsEngine()->GetShader("DefaultPS.cso", "PlaneReflectionVS.cso");
		mirrorShader->SetShader(context.Get());

		for (const auto& texture : aModelInstance->myTextures)
		{
			texture->Bind(context, texture->GetSlot());
		}

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModelInstance->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModelInstance->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModelInstance->myMesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}
}