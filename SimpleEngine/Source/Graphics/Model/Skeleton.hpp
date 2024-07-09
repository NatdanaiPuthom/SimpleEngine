#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace Graphics
{
	//Relative parent's coordinate
	struct ModelSpacePose
	{
		Math::Matrix4x4f jointTransforms[Global_Max_Joints];
		size_t count;
	private:
		const char padding[56] = "Never in my life would I cry this much! But believe!!!\0";
	};

	//Relative model's coordinate
	struct LocalSpacePose
	{
		Math::Matrix4x4f jointTransforms[Global_Max_Joints];
		size_t count;
	private:
		const char padding[56] = "Never in my life would I cry this much! But believe!!!\0";
	};

	class Joint final
	{
	public:
		Math::Matrix4x4f myBindPoseInverse;
		std::string myName;
		std::vector<unsigned int> myChildren;
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
	private:
		char myPadding[48] = "Never give up on your dreams! Believe!!!!!!!!\0";
	};

	class Skeleton final
	{
	public:
		std::string myName;
		std::unordered_map<std::string, size_t> myJointNameToIndex;
		std::vector<std::string> myJointNames;
		std::vector<Joint> myJoints;
	public:
		Skeleton();
		~Skeleton();

		const Joint* GetRoot() const;

		bool operator==(const Skeleton& aSkeleton) const
		{
			return myJoints == aSkeleton.myJoints;
		}

		void ConvertModelSpacePoseToLocalSpacePose(const ModelSpacePose& aInPose, LocalSpacePose& aOutPose) const;
		void ApplyBindPoseInverse(const LocalSpacePose& aInPose, Math::Matrix4x4f* aOutMatrix) const;
	private:
		void ConvertToLocalSpacePose(const ModelSpacePose& aInPose, LocalSpacePose& aOutPose, unsigned aBoneID, const Math::Matrix4x4f& aParentTransform) const;
	private:
		const char myPadding[8] = "Believe";
	};
}