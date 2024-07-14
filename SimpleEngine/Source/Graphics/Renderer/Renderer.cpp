#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Global.hpp"
#include "Engine/Math/Math.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/AnimationPlayerComponent.hpp"
#include "External/nlohmann/json.hpp"
#include <fstream>
#include <cassert>

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
		ID3D11DeviceContext* context = Global::GetGraphicsEngine()->GetContext().Get();

		aMeshComponent->shader->BindThisShader(context);
		BindTextures(aMeshComponent, context);

		RenderModel(aTransformComponent->transform.GetMatrix(), aMeshComponent->mesh, context);
	}

	void Renderer::RenderUnlitModel(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, const Graphics::Shader* aShader, const Graphics::Texture* aTexture) const
	{
		ID3D11DeviceContext* context = Global::GetGraphicsEngine()->GetContext().Get();

		aShader->BindThisShader(context);
		aTexture->Bind(context);

		RenderModel(aTransformMatrix, aMesh, context);
	}

	void Renderer::RenderModel(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, ID3D11DeviceContext* aContext) const
	{
		TransformBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aTransformMatrix;

		myTransformBuffer->Bind(myTransformBuffer->GetSlot());
		myTransformBuffer->Update(sizeof(TransformBufferData), &objectBuffer);

		UINT stride = sizeof(Graphics::Vertex);
		UINT offset = 0;

		aContext->IASetVertexBuffers(0, 1, aMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
		aContext->IASetIndexBuffer(aMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		aContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		aContext->DrawIndexed(static_cast<UINT>(aMesh->myMeshData.indices.size()), 0, 0);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderAnimatedModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent, const ECS::AnimationPlayerComponent* aAnimationPlayerComponent) const
	{
		ID3D11DeviceContext* context = Global::GetGraphicsEngine()->GetContext().Get();

		JointsBufferData boneBufferData = {};

		for (size_t i = 0; i < Graphics::Global_Max_Joints; ++i)
		{
			boneBufferData.bonesTransform[i] = aAnimationPlayerComponent->jointMatrices[i];;
		}

		myJointBuffer->Bind(myJointBuffer->GetSlot());
		myJointBuffer->Update(sizeof(JointsBufferData), &boneBufferData);

		aAnimationPlayerComponent->shader->BindThisShader(context);
		BindTextures(aMeshComponent, context);

		RenderModel(aTransformComponent->transform.GetMatrix(), aMeshComponent->mesh, context);
	}

	void Renderer::RenderLine(const Drawer::Line& aLine) const
	{
		myLineDrawer->Render(aLine);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderLine(const std::vector<Drawer::Line>& aLines) const
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

	void Renderer::RenderSphere(const Drawer::Sphere& aSphere) const
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

	void Renderer::BindTextures(const ECS::MeshComponent* aMeshComponent, ID3D11DeviceContext* aContext) const
	{
		for (size_t i = 0; i < aMeshComponent->textures.size(); ++i)
		{
			if (const Graphics::Texture* texture = aMeshComponent->textures[i]) //To-DO(9.36.4): Disgusting, pls fix
			{
				texture->Bind(aContext);
			}
			else
			{
				static ID3D11ShaderResourceView* nullview[1] = { nullptr };
				aContext->PSSetShaderResources(static_cast<unsigned int>(i), 1, nullview);
			}
		}
	}
}