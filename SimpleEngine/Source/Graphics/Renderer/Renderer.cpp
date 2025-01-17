#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Global.hpp"
#include "Engine/Math/Math.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Engine/SimpleUtilities/Bounds.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/AnimationComponent.hpp"
#include "External/nlohmann/json.hpp"
#include <fstream>
#include <cassert>
#include <d3d11.h>

constexpr size_t MAX_INSTANCE = 1024;

namespace Drawer
{
	using namespace Simple;

    void Renderer::GenerateInstanceData(std::vector<TransformBufferData>& instanceData)
    {
        instanceData.clear();

        int instanceCount = MAX_INSTANCE;
        int modelsPerLine = 40;
        float spacing = 2.0f;
        float lineSpacing = 2.0f;

        for (int i = 0; i < instanceCount; ++i)
        {
            TransformBufferData data;
            data.modelWorldMatrix = Math::Matrix4x4f();
            float x = (i % modelsPerLine) * spacing - (modelsPerLine / 2) * spacing;
            float z = (i / modelsPerLine) * lineSpacing;
            data.modelWorldMatrix.SetPosition(Math::Vector3f(x, 0.0f, z));
            instanceData.push_back(data);
        }
    }

	void Renderer::UpdateInstanceBuffer(const std::vector<TransformBufferData>& instanceData)
	{
		auto context = Global::GetGraphicsEngine()->GetContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = context->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (FAILED(hr))
		{
			std::cout << "error" << std::endl;
			return;
		}

		memcpy(mappedResource.pData, instanceData.data(), sizeof(TransformBufferData) * instanceData.size());
		context->Unmap(myInstanceBuffer.Get(), 0);
	}

	void Renderer::RenderInstances()
	{
		auto graphicsEngine = Global::GetGraphicsEngine();
		auto context = graphicsEngine->GetContext();
		const Graphics::Mesh* mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);

		ID3D11Buffer* vertexBuffer = mesh->myVertexBuffer.Get();
		ID3D11Buffer* indexBuffer = mesh->myIndexBuffer.Get();
		UINT indexCount = static_cast<UINT>(mesh->myMeshData.indices.size());

		std::vector<TransformBufferData> instanceData;
		GenerateInstanceData(instanceData);

		UpdateInstanceBuffer(instanceData);

		UINT strides[2] = { sizeof(Graphics::Vertex), sizeof(TransformBufferData) };
		UINT offsets[2] = { 0, 0 };
		ID3D11Buffer* buffers[2] = { vertexBuffer, myInstanceBuffer.Get() };

		context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		graphicsEngine->GetShaderManager()->GetShader(Graphics::eShaderType::Instanced_Unlit_Default)->BindThisShader(context.Get());
		graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Albedo)->Bind(context.Get());
		graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Normal)->Bind(context.Get());
		graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Material)->Bind(context.Get());
	
		context->DrawIndexedInstanced(indexCount, static_cast<UINT>(instanceData.size()), 0, 0, 0);
		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	Renderer::Renderer()
		: myIsUsingPBR(true)
		, myShouldRenderMesh(true)
		, myShouldRenderDebugLines(true)
		, myShouldRenderBoundingBox(true)
		, myShouldRenderSkeletonLines(false)
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
		{
			assert(false && "Failed to create ObjectBuffer");
		}

		if (!CreateBoneBuffer())
		{
			assert(false && "Failed to create BoneBuffer");
		}

		if (!CreateInstanceBuffer())
		{
			assert(false && "Failed to create InstanceBuffer");
		}

		myBoundingBoxDrawer->Init();

		myTransformBuffer->SetSlot(Graphics::Global_Constant_Buffer_Slot_Transform);
		myJointBuffer->SetSlot(Graphics::Global_Constant_Buffer_Slot_Joints);
	}

	void Renderer::RenderPBRStaticModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent) const
	{
		ID3D11DeviceContext* context = Global::GetGraphicsEngine()->GetContext().Get();

		aMeshComponent->shader->BindThisShader(context);
		BindTextures(aMeshComponent, context);

		RenderModel(aTransformComponent->transform.GetMatrix(), aMeshComponent->mesh, context);
	}

	void Renderer::RenderUnlitStaticModel(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, const Graphics::Shader* aShader, const Graphics::Texture* aTexture) const
	{
		ID3D11DeviceContext* context = Global::GetGraphicsEngine()->GetContext().Get();

		aShader->BindThisShader(context);
		aTexture->Bind(context);

		RenderModel(aTransformMatrix, aMesh, context);
	}

	void Renderer::RenderUnlitStaticAnimatedModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent, const ECS::AnimationComponent* aAnimationPlayerComponent) const
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		ID3D11DeviceContext* context = graphicsEngine->GetContext().Get();

		graphicsEngine->GetShaderManager()->GetShader(Graphics::eShaderType::Unlit_Default)->BindOnlyThisPixelShader(context);
		aAnimationPlayerComponent->shader->BindOnlyThisVertexShader(context);
		aMeshComponent->textures[Graphics::Global_Slot_Albedo]->Bind(context);

		UpdateJointTransforms(aAnimationPlayerComponent->jointMatrices);
		RenderModel(aTransformComponent->transform.GetMatrix(), aMeshComponent->mesh, context);
	}

	void Renderer::RenderStaticSkeletonLines(const ECS::TransformComponent* aTransformComponent, const ECS::AnimationComponent* aAnimationPlayerComponent)
	{
		const std::vector<Graphics::Joint>& joints = aAnimationPlayerComponent->skeleton->myJoints;
		const Math::Vector3f position = aTransformComponent->transform.GetPosition();
		const Math::Vector3f scale = aTransformComponent->transform.GetScale();

		std::vector<Drawer::Line> staticSkeletonLines(joints.size());

		Drawer::Line line;
		line.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		Drawer::Sphere sphere;
		sphere.radius = 0.05f;
		sphere.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		for (size_t index = 0; index < joints.size(); ++index)
		{
			Graphics::Joint joint = joints[index];

			const Math::Matrix4x4 boneWorldTransform = Math::Matrix4x4f::GetInverse(joint.myBindPoseInverse);

			sphere.position = boneWorldTransform.GetPosition() + position;

			Push(sphere);

			if (joint.myParent == -1)
				continue;

			const Math::Matrix4x4 boneWorldTransformNext = Math::Matrix4x4f::GetInverse(joints[joint.myParent].myBindPoseInverse);

			line.startPosition = boneWorldTransform.GetPosition() * scale + position;
			line.endPosition = boneWorldTransformNext.GetPosition() * scale + position;

			staticSkeletonLines[index] = line;
		}

		Push(staticSkeletonLines);
	}

	void Renderer::RenderAnimatedSkeletonLines(const ECS::TransformComponent* aTransformComponent, const ECS::AnimationComponent* aAnimationPlayerComponent)
	{
		const Graphics::Skeleton* skeleton = aAnimationPlayerComponent->skeleton;
		const Graphics::ModelSpacePose& modelSpacePose = aAnimationPlayerComponent->animationPlayer.myModelSpacePose;
		const Math::Matrix4x4f modelTransform = aTransformComponent->transform.GetMatrix();
		const Math::Vector3f position = aTransformComponent->transform.GetPosition();
		const Math::Vector3f scale = aTransformComponent->transform.GetScale();
		const size_t jointSize = aAnimationPlayerComponent->skeleton->myJoints.size();

		std::vector<Drawer::Line> animatedkeletonLines(jointSize);
		Graphics::LocalSpacePose pose;

		skeleton->ConvertModelSpacePoseToLocalSpacePose(modelSpacePose, pose);

		Drawer::Sphere sphere;
		sphere.radius = 0.05f;
		sphere.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		Drawer::Line line;
		line.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		for (size_t index = 0; index < jointSize; ++index)
		{
			Graphics::Joint joint = skeleton->myJoints[index];

			const Math::Matrix4x4 boneWorldTransform = pose.jointTransforms[index] * modelTransform;

			sphere.position = boneWorldTransform.GetPosition();

			Push(sphere);

			if (joint.myParent == -1)
				continue;

			const Math::Matrix4x4 boneWorldTransformNext = pose.jointTransforms[joint.myParent] * modelTransform;

			line.startPosition = boneWorldTransform.GetPosition();
			line.endPosition = boneWorldTransformNext.GetPosition();

			animatedkeletonLines[index] = line;
		}

		Push(animatedkeletonLines);
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

	void Renderer::RenderPBRAnimatedModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent, const ECS::AnimationComponent* aAnimationPlayerComponent) const
	{
		ID3D11DeviceContext* context = Global::GetGraphicsEngine()->GetContext().Get();

		UpdateJointTransforms(aAnimationPlayerComponent->jointMatrices);

		aAnimationPlayerComponent->shader->BindOnlyThisVertexShader(context);
		aMeshComponent->shader->BindOnlyThisPixelShader(context);

		BindTextures(aMeshComponent, context);

		RenderModel(aTransformComponent->transform.GetMatrix(), aMeshComponent->mesh, context);
	}

	void Renderer::Push(const Drawer::Line& aLine)
	{
		myDebugLines.push_back(aLine);
	}

	void Renderer::RenderDebugLines()
	{
		const size_t sizeLimit = myLineDrawer->GetInstanceSizeLimit();

		if (myDebugLines.size() < sizeLimit)
		{
			myLineDrawer->RenderInstance(myDebugLines);
			Impl::SimpleGlobalRenderer::IncreaseDrawCall();
		}
		else
		{
			std::vector<std::vector<Drawer::Line>> lineSplit = Math::SplitVector(myDebugLines, sizeLimit / 2);

			while (lineSplit.empty() == false)
			{
				myLineDrawer->RenderInstance(lineSplit[0]);
				lineSplit.erase(lineSplit.begin());
				Impl::SimpleGlobalRenderer::IncreaseDrawCall();
			}
		}

		for (size_t i = 0; i < myDebugSpheres.size(); i++)
		{
			mySphereDrawer->Render(myDebugSpheres[i]);
			Impl::SimpleGlobalRenderer::IncreaseDrawCall();
		}

		for (size_t i = 0; i < myBoundingBoxesData.size(); i++)
		{
			myBoundingBoxDrawer->Render(myBoundingBoxesData[i].boundingBox, myBoundingBoxesData[i].modelToWorld, myBoundingBoxesData[i].color);
			Impl::SimpleGlobalRenderer::IncreaseDrawCall();
		}

		myDebugLines.clear();
		myDebugSpheres.clear();
		myBoundingBoxesData.clear();
	}

	void Renderer::Push(const std::vector<Drawer::Line>& aLines)
	{
		for (size_t i = 0; i < aLines.size(); ++i)
		{
			myDebugLines.push_back(aLines[i]);
		}
	}

	void Renderer::Push(const Drawer::Sphere& aSphere)
	{
		myDebugSpheres.push_back(aSphere);
	}

	void Renderer::Push(const Drawer::BoundingBox3DData& aBoundingBoxData)
	{
		myBoundingBoxesData.push_back(aBoundingBoxData);
	}

	void Renderer::RenderSprite2D(const Drawer::Sprite2D& aSprite)
	{
		mySpriteDrawer->Render(aSprite);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::SetShouldRenderMesh(const bool aShouldRender)
	{
		myShouldRenderMesh = aShouldRender;
	}

	void Renderer::SetShouldRenderDebugLines(const bool aShouldRender)
	{
		myShouldRenderDebugLines = aShouldRender;
	}

	void Renderer::SetShouldRenderBoundingBox(const bool aShouldRender)
	{
		myShouldRenderBoundingBox = aShouldRender;
	}

	void Renderer::SetShouldRenderSkeletonLines(const bool aShouldRender)
	{
		myShouldRenderSkeletonLines = aShouldRender;
	}

	void Renderer::SetIsUsingPBR(const bool aIsUsingPBR)
	{
		myIsUsingPBR = aIsUsingPBR;
	}

	bool Renderer::GetShouldRenderMesh() const
	{
		return myShouldRenderMesh;
	}

	bool Renderer::GetShouldRenderDebugLines() const
	{
		return myShouldRenderDebugLines;
	}

	bool Renderer::GetShouldRenderBoundingBox() const
	{
		return myShouldRenderBoundingBox;
	}

	bool Renderer::GetShouldRenderSkeletonLines() const
	{
		return myShouldRenderSkeletonLines;
	}

	bool Renderer::GetIsUsingPBR() const
	{
		return myIsUsingPBR;
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

	const bool Renderer::CreateInstanceBuffer()
	{
		D3D11_BUFFER_DESC instanceBufferDesc = {};
		instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.ByteWidth = sizeof(TransformBufferData) * MAX_INSTANCE;
		instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instanceBufferDesc.MiscFlags = 0;
		instanceBufferDesc.StructureByteStride = 0;

		const HRESULT result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&instanceBufferDesc, nullptr, &myInstanceBuffer);
		return SUCCEEDED(result);
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

	void Renderer::UpdateJointTransforms(const Math::Matrix4x4f* aJointMatrices) const
	{
		JointsBufferData boneBufferData = {};

		for (size_t i = 0; i < Graphics::Global_Max_Joints; ++i)
		{
			boneBufferData.bonesTransform[i] = aJointMatrices[i];
		}

		myJointBuffer->Bind(myJointBuffer->GetSlot());
		myJointBuffer->Update(sizeof(JointsBufferData), &boneBufferData);
	}

	void Renderer::LoadSettingsFromJson()
	{
		const nlohmann::json jsonData = SimpleUtilities::FileManager::GetDataAsJson(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR));

		myShouldRenderDebugLines = jsonData["Editor_Settings"]["Render_DebugLine"];
		myShouldRenderMesh = jsonData["Editor_Settings"]["Render_Mesh"];
		myShouldRenderSkeletonLines = jsonData["Editor_Settings"]["Render_Skeleton"];
		myShouldRenderBoundingBox = jsonData["Editor_Settings"]["Render_BoundingBox"];
		myIsUsingPBR = jsonData["Editor_Settings"]["Render_PBR"];
	}
}