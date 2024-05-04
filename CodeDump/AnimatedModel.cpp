#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/AnimatedModel.hpp"
#include "Engine/Global.hpp"

namespace Graphics
{
	AnimatedModel::AnimatedModel()
		: myMesh(nullptr)
		, mySkeleton(nullptr)
		, myName("Unnamed")
		, myBoundingBoxColor(1.0f, 1.0f, 0.0f, 1.0f)
	{
		myShader = Global::GetGraphicsEngine()->GetShader("DefaultPS.cso", "AnimatedModelVS.cso");
	}

	AnimatedModel::~AnimatedModel()
	{
		myShader = nullptr;
		myMesh = nullptr;
		mySkeleton = nullptr;
	}

	void AnimatedModel::Init(const Mesh* const aMesh, const Skeleton* const aSkeleton)
	{
		myMesh = aMesh;
		mySkeleton = aSkeleton;
		myTestSkeletonForIK = *aSkeleton;
		AddTexture("DefaultTexture.dds");
	}

	void AnimatedModel::Init(const Mesh* const aMesh, const Skeleton* const aSkeleton, const char* aTexturePath)
	{
		myMesh = aMesh;
		mySkeleton = aSkeleton;
		myTestSkeletonForIK = *aSkeleton;
		AddTexture(aTexturePath);
	}

	void AnimatedModel::AddTexture(const char* aFilePath)
	{
		std::shared_ptr<const Texture> texture = Global::GetGraphicsEngine()->GetTexture(aFilePath);

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

	void AnimatedModel::ClearTextures()
	{
		myTextures.clear();
	}

	void AnimatedModel::SetPose(const ModelSpacePose& aPose)
	{
		LocalSpacePose localSpacePose;
		mySkeleton->ConvertModelSpacePoseToLocalSpacePose(aPose, localSpacePose);
		SetPose(localSpacePose);
	}

	void AnimatedModel::SetPose(const LocalSpacePose& aPose)
	{
		mySkeleton->ApplyBindPoseInverse(aPose, myBoneTransforms);
	}

	void AnimatedModel::LookAt(const Math::Vector3f& aTargetPoint)
	{
		myTransform.LookAt(aTargetPoint);
	}

	void AnimatedModel::SetShader(const char* aPSShaderFile, const char* aVSShaderFile)
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

	void AnimatedModel::SetPosition(const Math::Vector3f& aPosition)
	{
		myTransform.SetPosition(aPosition);
	}

	void AnimatedModel::SetRotation(const Math::Vector3f& aRotationInDegree)
	{
		myTransform.SetRotation(aRotationInDegree);
	}

	void AnimatedModel::SetScale(const Math::Vector3f& aScale)
	{
		myTransform.SetScale(aScale);
	}

	void AnimatedModel::SetScale(const float aScale)
	{
		myTransform.SetScale(aScale);
	}

	void AnimatedModel::SetName(const std::string& aName)
	{
		myName = aName;
	}

	void AnimatedModel::SetBoundingBoxLineColor(const Math::Vector4f& aColor)
	{
		myBoundingBoxColor = aColor;
	}

	const Simple::BoundingBox3D& AnimatedModel::GetBoundingBox() const
	{
		return myMesh->GetBoundingBox();
	}

	Math::Vector4f AnimatedModel::GetBoundingBoxLineColor() const
	{
		return myBoundingBoxColor;
	}

	Math::Matrix4x4f AnimatedModel::GetMatrix() const
	{
		return myTransform.GetMatrix();
	}

	Math::Vector3f AnimatedModel::GetPosition() const
	{
		return myTransform.GetPosition();
	}

	Math::Vector3f AnimatedModel::GetRotation() const
	{
		return myTransform.GetRotation();
	}

	Math::Vector3f AnimatedModel::GetScale() const
	{
		return myTransform.GetScale();
	}

	std::string AnimatedModel::GetName() const
	{
		return myName;
	}

	const Skeleton* AnimatedModel::GetSkeleton() const
	{
		return mySkeleton;
	}

	Skeleton& AnimatedModel::GetTestIKSkeleton()
	{
		return myTestSkeletonForIK;
	}

	const Skeleton& AnimatedModel::GetTestIKSkeleton() const
	{
		return myTestSkeletonForIK;
	}
}