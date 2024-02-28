#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Skeleton.hpp"

namespace Simple
{
	Joint::Joint()
		: myParent(-1)
	{
	}

	Joint::~Joint()
	{
	}

	Skeleton::Skeleton()
	{
	}

	Skeleton::~Skeleton()
	{
	}

	const Joint* Skeleton::GetRoot() const
	{
		if (myJoints.empty())
		{
			return nullptr;
		}

		return &myJoints[0];
	}

	void Skeleton::ConvertPoseToModelSpace(const LocalSpacePose& aInPose, ModelSpacePose& aOutPose) const
	{
		ConvertPoseToModelSpace(aInPose, aOutPose, 0, Math::Matrix4x4f::Identity());
		aOutPose.count = aInPose.count;
	}

	void Skeleton::ApplyBindPoseInverse(const ModelSpacePose& aInPose, Math::Matrix4x4f* aOutMatrix) const
	{
		for (size_t i = 0; i < myJoints.size(); ++i)
		{
			const Joint& joint = myJoints[i];
			aOutMatrix[i] = joint.myBindPoseInverse * aInPose.jointTransforms[i];
		}
	}

	void Skeleton::ConvertPoseToModelSpace(const LocalSpacePose& aInPose, ModelSpacePose& aOutPose, unsigned aBoneID, const Math::Matrix4x4f& aParentTransform) const
	{
		const Joint& joint = myJoints[aBoneID];

		aOutPose.jointTransforms[aBoneID] = aInPose.jointTransforms[aBoneID] * aParentTransform;

		for (size_t childIndex = 0; childIndex < joint.myChildren.size(); childIndex++)
		{
			ConvertPoseToModelSpace(aInPose, aOutPose, joint.myChildren[childIndex], aOutPose.jointTransforms[aBoneID]);
		}
	}
}