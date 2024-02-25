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

namespace Drawer
{
	using namespace Simple;

	Renderer::Renderer()
		: myDebugMode(false)
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Init()
	{
		myObjectBuffer = std::make_unique<ConstantBuffer>();
		myBoneBuffer = std::make_unique<ConstantBuffer>();

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

	void Renderer::RenderModel(const std::shared_ptr<const Model> aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

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

	void Renderer::RenderModel(const Model& aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel.GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		BonesBufferData boneBufferData = {};
		
		for (size_t i = 0; i < 64; ++i)
		{
			boneBufferData.bonesTransform[i] = aModel.myBoneTransforms[i];
		}

		myBoneBuffer->Bind(myBoneBuffer->GetSlot());
		myBoneBuffer->Update(sizeof(BonesBufferData), &boneBufferData);

		aModel.myShader->UseThisShader(context.Get());

		for (const auto& texture : aModel.myTextures)
		{
			texture->Bind(context, texture->GetSlot());
		}

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aModel.myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aModel.myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aModel.myMesh->myMeshData.indices.size()), 0, 0);

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

	void Renderer::RenderSprite2D(const Drawer::Sprite2D& aSprite)
	{
		mySpriteDrawer->Render(aSprite);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const std::shared_ptr<const Model> aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderLineInstance(const std::vector<Drawer::Line> aLines)
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

	void Renderer::RenderEverythingUpSideDown() const
	{
		auto camera = Global::GetGraphicsEngine()->GetCurrentCamera();

		const Math::Vector3f oldCamPosition = camera->GetPosition();
		const Math::Vector3f oldCamRotation = camera->GetRotation();
		const Math::Vector3f newCamRotation = Math::Vector3f(oldCamRotation.x, -oldCamRotation.y, oldCamRotation.z);

		const float waterHeight = SimpleWorld::GetWaterHeight();
		const float distFromWater = 2.0f * (oldCamPosition.y - waterHeight);

		camera->SetPosition(oldCamPosition - Math::Vector3f(0.0f, distFromWater, 0.0f));
		camera->SetRotation(newCamRotation);

		Drawer::Renderer* renderer = Global::GetRenderer();
		for (const auto& model : SimpleWorld::GetActiveScene()->myModels)
		{
			renderer->RenderUpSideDown(model);
		}

		camera->SetRotation(oldCamRotation);
		camera->SetPosition(oldCamPosition);
	}

	void Renderer::RenderPlaneReflection(const std::shared_ptr<const Model> aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->UseThisShader(context.Get());

		context->PSSetShaderResources(0, 1, Global::GetGraphicsEngine()->GetWaterShaderResourceView().GetAddressOf());
		context->PSSetShaderResources(1, 1, Global::GetGraphicsEngine()->GetWaterRefractionShaderResourceView().GetAddressOf());

		auto wave = Global::GetGraphicsEngine()->GetTexture("TGA/Uppgift8/WaveTest.dds");
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
		Drawer::Renderer* renderer = Global::GetRenderer();

		for (const auto& model : SimpleWorld::GetActiveScene()->myModels)
		{
			renderer->RenderRefraction(model);
		}
	}

	void Renderer::RenderUpSideDown(const std::shared_ptr<const Model> aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		Math::Matrix4x4f mirror = Math::Matrix4x4f::Identity();

		mirror(2, 2) = -1.0f;
		mirror(4, 2) = -2.0f;

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix() * mirror;

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->UseThisShader(context.Get());

		auto mirrorShader = Global::GetGraphicsEngine()->GetShader("DefaultPS.cso", "PlaneReflectionVS.cso");
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

	void Renderer::RenderRefraction(const std::shared_ptr<const Model> aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		aModel->myShader->UseThisShader(context.Get());

		auto refractionShader = Global::GetGraphicsEngine()->GetShader("DefaultPS.cso", "PlaneReflectionVS.cso");
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
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		SetDebugMode(json["game_settings"]["debugMode"]);
	}
}