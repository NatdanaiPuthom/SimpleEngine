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

#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/AnimatedComponent.hpp"

namespace Drawer
{
	using namespace Simple;

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Init()
	{
		myTransformBuffer = std::make_unique<Graphics::ConstantBuffer>();
		myJointBuffer = std::make_unique<Graphics::ConstantBuffer>();

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

		myTransformBuffer->SetSlot(Graphics::Global_Constant_Buffer_Slot_Transform);
		myJointBuffer->SetSlot(Graphics::Global_Constant_Buffer_Slot_Joints);
	}

	void Renderer::RenderStaticModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		TransformBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aTransformComponent->transform.GetMatrix();

		myTransformBuffer->Bind(myTransformBuffer->GetSlot());
		myTransformBuffer->Update(sizeof(TransformBufferData), &objectBuffer);

		aMeshComponent->shader->BindThisShader(context.Get());

		for (size_t i = 0; i < aMeshComponent->textures.size(); ++i)
		{
			if (const Graphics::Texture* texture = aMeshComponent->textures[i]) //NOTE(9.36.4): Disgusting
			{
				size_t slot = texture->GetSlot();

				if (slot < Graphics::Global_Slot_Normal)   //NOTE(9.36.4): Even more disgusting
				{
					slot = i;
				}

				texture->Bind(context, static_cast<unsigned int>(slot));
			}
		}

		UINT stride = sizeof(Graphics::Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aMeshComponent->mesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aMeshComponent->mesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aMeshComponent->mesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderAnimatedModel(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent, ECS::AnimatedComponent* aSkeletonComponent)
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		TransformBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aTransformComponent->transform.GetMatrix();

		myTransformBuffer->Bind(myTransformBuffer->GetSlot());
		myTransformBuffer->Update(sizeof(TransformBufferData), &objectBuffer);

		JointsBufferData boneBufferData = {};

		for (size_t i = 0; i < Graphics::Global_Max_Joints; ++i)
		{
			boneBufferData.bonesTransform[i] = aSkeletonComponent->jointMatrices[i];;
		}

		myJointBuffer->Bind(myJointBuffer->GetSlot());
		myJointBuffer->Update(sizeof(JointsBufferData), &boneBufferData);

		aSkeletonComponent->shader->BindThisShader(context.Get());
		aMeshComponent->textures[0]->Bind(context, aMeshComponent->textures[0]->GetSlot());

		UINT stride = sizeof(Graphics::Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, aMeshComponent->mesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(aMeshComponent->mesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(aMeshComponent->mesh->myMeshData.indices.size()), 0, 0);

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

	const bool Renderer::CreateObjectBuffer()
	{
		TransformBufferData objectBuffer;

		if (!myTransformBuffer->Init(sizeof(TransformBufferData), &objectBuffer))
			return false;

		return true;
	}

	const bool Renderer::CreateBoneBuffer()
	{
		JointsBufferData boneBufferData;

		if (!myJointBuffer->Init(sizeof(JointsBufferData), &boneBufferData))
			return false;

		return true;
	}

	void Renderer::LoadSettingsFromJson()
	{
		/*const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();*/
	}
}