#include "Engine/Precomplier/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Graphics/Renderer/BoundingBoxDrawer.hpp"
#include "Engine/Graphics/Renderer/LineDrawer.hpp"
#include "Engine/Graphics/Renderer/SphereDrawer.hpp"
#include "Game/world.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"

namespace Drawer
{
	Renderer::Renderer()
		: myObjectBuffer(std::make_unique<Simple::ConstantBuffer>())
		, myBoundingBoxDrawer(std::make_unique<Drawer::BoundingBoxDrawer>())
		, myLineDrawer(std::make_unique<Drawer::LineDrawer>())
		, mySphereDrawer(std::make_unique<Drawer::SphereDrawer>())
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

	void Renderer::RenderModel(const std::shared_ptr<const Simple::Model> aModel) const
	{
		const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->UseThisShader(context.Get());

		for (const auto& texture : aModel->myTextures)
		{
			texture->Bind(context, texture->GetSlot());
		}

		UINT stride = sizeof(Vertex);
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

	void Renderer::RenderSphere(const Drawer::Sphere& aSphere)
	{
		mySphereDrawer->Render(aSphere);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const std::shared_ptr<const Simple::Model> aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

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

		Drawer::Renderer* renderer = SimpleGlobal::GetRenderer();
		for (const auto& model : SimpleWorld::GetActiveScene()->myModels)
		{
			renderer->RenderUpSideDown(model);
		}

		camera->SetRotation(oldCamRotation);
		camera->SetPosition(oldCamPosition);
	}

	void Renderer::RenderPlaneReflection(const std::shared_ptr<const Simple::Model> aModel) const
	{
		const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->UseThisShader(context.Get());

		context->PSSetShaderResources(0, 1, SimpleGlobal::GetGraphicsEngine()->GetWaterShaderResourceView().GetAddressOf());
		context->PSSetShaderResources(1, 1, SimpleGlobal::GetGraphicsEngine()->GetWaterRefractionShaderResourceView().GetAddressOf());

		auto wave = SimpleGlobal::GetGraphicsEngine()->GetTexture("TGA/Uppgift8/test.dds");
		wave->Bind(context, wave->GetSlot());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModel->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModel->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModel->myMesh->myMeshData.indices.size()), 0, 0);

		ID3D11ShaderResourceView* nullSRV = nullptr;
		context->PSSetShaderResources(0, 1, &nullSRV);
		context->PSSetShaderResources(1, 1, &nullSRV);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderRefraction() const
	{
		Drawer::Renderer* renderer = SimpleGlobal::GetRenderer();

		for (const auto& model : SimpleWorld::GetActiveScene()->myModels)
		{
			renderer->RenderRefraction(model);
		}
	}

	void Renderer::RenderUpSideDown(const std::shared_ptr<const Simple::Model> aModel) const
	{
		const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

		SimpleUtilities::Matrix4x4f mirror = SimpleUtilities::Matrix4x4f::Identity();

		mirror(2, 2) = -1.0f;
		mirror(4, 2) = -2.0f;

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix() * mirror;

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->UseThisShader(context.Get());

		auto mirrorShader = SimpleGlobal::GetGraphicsEngine()->GetShader("DefaultPS.cso", "PlaneReflectionVS.cso");
		mirrorShader->UseThisVertexShader(context);

		for (const auto& texture : aModel->myTextures)
		{
			texture->Bind(context, texture->GetSlot());
		}

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModel->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModel->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModel->myMesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderRefraction(const std::shared_ptr<const Simple::Model> aModel) const
	{
		const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->UseThisShader(context.Get());

		auto refractionShader = SimpleGlobal::GetGraphicsEngine()->GetShader("DefaultPS.cso", "PlaneReflectionVS.cso");
		refractionShader->UseThisVertexShader(context);

		for (const auto& texture : aModel->myTextures)
		{
			texture->Bind(context, texture->GetSlot());
		}

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModel->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModel->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModel->myMesh->myMeshData.indices.size()), 0, 0);

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

		myObjectBuffer->SetSlot(1);

		return true;
	}

	bool Renderer::IsDebugModeOn() const
	{
		return myDebugMode;
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
}