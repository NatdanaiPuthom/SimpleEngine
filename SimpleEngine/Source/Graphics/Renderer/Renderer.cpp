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
		myAnimatedSkeletonLines.reserve(SIMPLE_MAX_BONES);
		myStaticSkeletonLines.reserve(SIMPLE_MAX_BONES);

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

		aModel->myShader->BindThisShader(context.Get());

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

	void Renderer::RenderModel(const std::shared_ptr<const Simple::AnimatedModel> aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		BonesBufferData boneBufferData = {};

		for (size_t i = 0; i < SIMPLE_MAX_BONES; ++i)
		{
			boneBufferData.bonesTransform[i] = aModel->myBoneTransforms[i];
		}

		myBoneBuffer->Bind(myBoneBuffer->GetSlot());
		myBoneBuffer->Update(sizeof(BonesBufferData), &boneBufferData);

		aModel->myShader->BindThisShader(context.Get());

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

		aModel.myShader->BindThisShader(context.Get());

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

	void Renderer::RenderModel(const Simple::AnimatedModel& aModel) const
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModel.GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		BonesBufferData boneBufferData = {};

		for (size_t i = 0; i < SIMPLE_MAX_BONES; ++i)
		{
			boneBufferData.bonesTransform[i] = aModel.myBoneTransforms[i];
		}

		myBoneBuffer->Bind(myBoneBuffer->GetSlot());
		myBoneBuffer->Update(sizeof(BonesBufferData), &boneBufferData);

		aModel.myShader->BindThisShader(context.Get());

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

	void Renderer::RenderAnimatedSkeletonLines(const Simple::AnimatedModel& aModel, const Simple::LocalSpacePose& aLocalPose)
	{
		myAnimatedSkeletonLines.resize(aLocalPose.count);

		Simple::ModelSpacePose pose;

		const Simple::Skeleton* skeleton = aModel.GetSkeleton();
		skeleton->ConvertPoseToModelSpace(aLocalPose, pose);

		const Math::Matrix4x4f modelTransform = aModel.GetMatrix();

		Drawer::Sphere sphere;
		sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		sphere.radius = 0.01f;

		Drawer::Line line;
		line.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		for (size_t index = 0; index < aLocalPose.count; ++index)
		{
			Simple::Joint joint = skeleton->myJoints[index];

			if (joint.myParent == -1)
				continue;

			const Math::Matrix4x4 boneWorldTransform = pose.jointTransforms[index] * modelTransform;
			const Math::Matrix4x4 boneWorldTransformNext = pose.jointTransforms[joint.myParent] * modelTransform;

			line.startPosition = boneWorldTransform.GetPosition();
			line.endPosition = boneWorldTransformNext.GetPosition();
			myAnimatedSkeletonLines.push_back(line);

			if (joint.myName.find("Jnt"))
			{
				sphere.position = boneWorldTransform.GetPosition();
				mySphereDrawer->Render(sphere);
			}
		}

		Global::GetRenderer()->RenderLineInstance(myAnimatedSkeletonLines);
	}

	void Renderer::RenderAnimatedSkeletonLines(const std::shared_ptr<const Simple::AnimatedModel> aModel, const Simple::LocalSpacePose& aLocalPose)
	{
		myAnimatedSkeletonLines.resize(aLocalPose.count);

		Simple::ModelSpacePose pose;
		const Simple::Skeleton* skeleton = aModel->GetSkeleton();
		skeleton->ConvertPoseToModelSpace(aLocalPose, pose);

		const Math::Matrix4x4f modelTransform = aModel->GetMatrix();

		Drawer::Sphere sphere;
		sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		sphere.radius = 0.01f;

		Drawer::Line line;
		line.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		for (size_t index = 0; index < aLocalPose.count; ++index)
		{
			Simple::Joint joint = skeleton->myJoints[index];

			if (joint.myParent == -1)
				continue;

			const Math::Matrix4x4 boneWorldTransform = pose.jointTransforms[index] * modelTransform;
			const Math::Matrix4x4 boneWorldTransformNext = pose.jointTransforms[joint.myParent] * modelTransform;

			line.startPosition = boneWorldTransform.GetPosition();
			line.endPosition = boneWorldTransformNext.GetPosition();
			myAnimatedSkeletonLines.push_back(line);

			if (joint.myName.find("Jnt"))
			{
				sphere.position = boneWorldTransform.GetPosition();
				mySphereDrawer->Render(sphere);
			}
		}

		Global::GetRenderer()->RenderLineInstance(myAnimatedSkeletonLines);
	}

	void Renderer::RenderStaticSkeletonLines(const Simple::AnimatedModel& aModel)
	{
		const std::vector<Simple::Joint>& joints = aModel.GetSkeleton()->myJoints;
		const Math::Vector3f scale = aModel.GetScale();

		myStaticSkeletonLines.resize(joints.size());

		for (size_t index = 0; index < joints.size(); ++index)
		{
			Simple::Joint joint = joints[index];

			if (joint.myParent == -1)
				continue;

			const Math::Matrix4x4 boneWorldTransform = Math::Matrix4x4f::GetInverse(joints[index].myBindPoseInverse);
			const Math::Matrix4x4 boneWorldTransformNext = Math::Matrix4x4f::GetInverse(joints[joint.myParent].myBindPoseInverse);

			Drawer::Line line;

			if (index % 3 == 0)
				line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			else if (index % 3 == 1)
				line.color = { 0.0f, 1.0f, 0.0f, 1.0f };
			else if (index % 3 == 2)
				line.color = { 0.0f, 0.0f, 1.0f, 1.0f };

			line.startPosition = boneWorldTransform.GetPosition() * scale;
			line.endPosition = boneWorldTransformNext.GetPosition() * scale;
			myStaticSkeletonLines.push_back(line);
		}

		Global::GetRenderer()->RenderLineInstance(myStaticSkeletonLines);
	}

	void Renderer::RenderStaticSkeletonLines(const std::shared_ptr<const Simple::AnimatedModel> aModel)
	{
		const std::vector<Simple::Joint>& joints = aModel->GetSkeleton()->myJoints;
		const Math::Vector3f scale = aModel->GetScale();

		myStaticSkeletonLines.resize(joints.size());

		for (size_t index = 0; index < joints.size(); ++index)
		{
			Simple::Joint joint = joints[index];

			if (joint.myParent == -1)
				continue;

			const Math::Matrix4x4 boneWorldTransform = Math::Matrix4x4f::GetInverse(joints[index].myBindPoseInverse);
			const Math::Matrix4x4 boneWorldTransformNext = Math::Matrix4x4f::GetInverse(joints[joint.myParent].myBindPoseInverse);

			Drawer::Line line;

			if (index % 3 == 0)
				line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			else if (index % 3 == 1)
				line.color = { 0.0f, 1.0f, 0.0f, 1.0f };
			else if (index % 3 == 2)
				line.color = { 0.0f, 0.0f, 1.0f, 1.0f };

			line.startPosition = boneWorldTransform.GetPosition() * scale;
			line.endPosition = boneWorldTransformNext.GetPosition() * scale;
			myStaticSkeletonLines.push_back(line);
		}

		Global::GetRenderer()->RenderLineInstance(myStaticSkeletonLines);
	}

	void Renderer::RenderBoundingBox(const std::shared_ptr<const Model> aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const std::shared_ptr<const Simple::AnimatedModel> aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const Simple::Model& aModel) const
	{
		myBoundingBoxDrawer->Render(aModel);

		Impl::SimpleGlobalRenderer::IncreaseDrawCall();
	}

	void Renderer::RenderBoundingBox(const Simple::AnimatedModel& aModel) const
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

		const float waterHeight = World::GetWaterHeight();
		const float distFromWater = 2.0f * (oldCamPosition.y - waterHeight);

		camera->SetPosition(oldCamPosition - Math::Vector3f(0.0f, distFromWater, 0.0f));
		camera->SetRotation(newCamRotation);

		Drawer::Renderer* renderer = Global::GetRenderer();
		for (const auto& model : World::GetActiveScene()->myModels)
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

		aModel->myShader->BindThisShader(context.Get());

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

		for (const auto& model : World::GetActiveScene()->myModels)
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

		aModel->myShader->BindThisShader(context.Get());

		auto mirrorShader = Global::GetGraphicsEngine()->GetShader("DefaultPS.cso", "PlaneReflectionVS.cso");
		mirrorShader->BindOnlyThisVertexShader(context);

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

		aModel->myShader->BindThisShader(context.Get());

		auto refractionShader = Global::GetGraphicsEngine()->GetShader("DefaultPS.cso", "PlaneReflectionVS.cso");
		refractionShader->BindOnlyThisVertexShader(context);

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
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_GAME_SETTINGS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		SetDebugMode(json["game_settings"]["debugMode"]);
	}
}