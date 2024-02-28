#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include <string>
#include <vector>
#include <unordered_map>

#define SIMPLE_MAX_BONES 64

namespace Simple
{
	struct LocalSpacePose
	{
		Math::Matrix4x4f jointTransforms[SIMPLE_MAX_BONES];
		size_t count;
	};

	struct ModelSpacePose
	{
		Math::Matrix4x4f jointTransforms[SIMPLE_MAX_BONES];
		size_t count;
	};

	class Joint final
	{
	public:
		std::vector<unsigned int> myChildren;
		Math::Matrix4x4f myBindPoseInverse;
		std::string myName;
		int myParent;
	public:
		Joint();
		~Joint();

		bool operator==(const Joint& aJoint) const
		{
			const bool a = myBindPoseInverse == aJoint.myBindPoseInverse;
			const bool b = myParent == aJoint.myParent;
			const bool c = myName == aJoint.myName;
			const bool d = myChildren == aJoint.myChildren;

			return (a && b && c && d);
		}
	};

	class Skeleton final
	{
	public:
		std::vector<Joint> myJoints;
		std::vector<std::string> myJointNames;
		std::unordered_map<std::string, size_t> myJointNameToIndex;
		std::unordered_map<std::wstring, size_t> myAnimationNameToIndex;
		std::string myName;
	public:
		Skeleton();
		~Skeleton();

		const Joint* GetRoot() const;

		bool operator==(const Skeleton& aSkeleton) const
		{
			return myJoints == aSkeleton.myJoints;
		}

		void ConvertPoseToModelSpace(const LocalSpacePose& aInPose, ModelSpacePose& aOutPose) const;
		void ApplyBindPoseInverse(const ModelSpacePose& aInPose, Math::Matrix4x4f* aOutMatrix) const;
	private:
		void ConvertPoseToModelSpace(const LocalSpacePose& aInPose, ModelSpacePose& aOutPose, unsigned aBoneID, const Math::Matrix4x4f& aParentTransform) const;
	};
}