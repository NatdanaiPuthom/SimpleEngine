#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Model.hpp"
#include "Engine/Global.hpp"

namespace Simple
{
	Model::Model()
		: myMesh(nullptr)
		, myName("Unnamed")
		, myBoundingBoxColor(1.0f, 1.0f, 0.0f, 1.0f)
	{
		myShader = Global::GetGraphicsEngine()->GetDefaultShader();
	}

	Model::~Model()
	{
		myMesh = nullptr;
		myShader = nullptr;
	}

	void Model::Init(const Simple::Mesh* const aMesh)
	{
		myMesh = aMesh;
		AddTexture("DefaultTexture.dds");
	}

	void Model::Init(const Simple::Mesh* const aMesh, const char* aTexturePath)
	{
		myMesh = aMesh;
		AddTexture(aTexturePath);
	}

	void Model::AddTexture(const char* aFilePath)
	{
		std::shared_ptr<const Simple::Texture> texture = Global::GetGraphicsEngine()->GetTexture(aFilePath);

		if (texture == nullptr)
		{
			if (!Global::GetGraphicsEngine()->AddTexture(aFilePath))
				assert(false && "Failed to add Texture to the bank");

			texture = Global::GetGraphicsEngine()->GetTexture(aFilePath);

			if (texture == nullptr)
				assert(false && "Skill Issues");
		}

		myTextures.push_back(texture);
	}

	void Model::ClearTextures()
	{
		myTextures.clear();
	}

	void Model::SetPose(const LocalSpacePose& aPose)
	{
		ModelSpacePose modelSpacePose;
		myMesh->mySkeleton.ConvertPoseToModelSpace(aPose, modelSpacePose);
		SetPose(modelSpacePose);
	}

	void Model::SetPose(const ModelSpacePose& aPose)
	{
		myMesh->mySkeleton.ApplyBindPoseInverse(aPose, myBoneTransforms);
	}

	void Model::LookAt(const Math::Vector3f& aTargetPoint)
	{
		myTransform.LookAt(aTargetPoint);
	}

	void Model::SetShader(const char* aPSShaderFile, const char* aVSShaderFile)
	{
		myShader = nullptr;
		myShader = Global::GetGraphicsEngine()->GetShader(aPSShaderFile, aVSShaderFile);

		if (myShader == nullptr)
		{
			if (!Global::GetGraphicsEngine()->AddShader(aPSShaderFile, aVSShaderFile))
				assert(false && "Failed to add Shader to the bank");

			myShader = Global::GetGraphicsEngine()->GetShader(aPSShaderFile, aVSShaderFile);

			if (myShader == nullptr)
				assert(false && "Skill Issues");
		}
	}

	void Model::SetPosition(const Math::Vector3f& aPosition)
	{
		myTransform.SetPosition(aPosition);
	}

	void Model::SetRotation(const Math::Vector3f& aRotationInDegree)
	{
		myTransform.SetRotation(aRotationInDegree);
	}

	void Model::SetScale(const Math::Vector3f& aScale)
	{
		myTransform.SetScale(aScale);
	}

	void Model::SetScale(const float aScale)
	{
		myTransform.SetScale(aScale);
	}

	void Model::SetName(const std::string& aName)
	{
		myName = aName;
	}

	void Model::SetBoundingBoxLineColor(const Math::Vector4f& aColor)
	{
		myBoundingBoxColor = aColor;
	}

	const BoundingBox3D& Model::GetBoundingBox() const
	{
		return myMesh->GetBoundingBox();
	}

	Math::Vector4f Model::GetBoundingBoxLineColor() const
	{
		return myBoundingBoxColor;
	}

	Math::Matrix4x4f Model::GetMatrix() const
	{
		return myTransform.GetMatrix();
	}

	Math::Vector3f Model::GetPosition() const
	{
		return myTransform.GetPosition();
	}

	Math::Vector3f Model::GetRotation() const
	{
		return myTransform.GetRotation();
	}

	Math::Vector3f Model::GetScale() const
	{
		return myTransform.GetScale();
	}

	std::string Model::GetName() const
	{
		return myName;
	}
}