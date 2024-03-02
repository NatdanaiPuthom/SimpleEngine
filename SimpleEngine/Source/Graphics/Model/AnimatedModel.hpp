#pragma once
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include <memory>
#include <string>

namespace Simple
{
	class Shader;
	class Texture;
}

namespace Drawer
{
	class Renderer;
}

namespace Simple
{
	class AnimatedModel final
	{
		friend class Drawer::Renderer;
		friend class Drawer::BoundingBoxDrawer;
	public:

		AnimatedModel();
		~AnimatedModel();

		void Init(const Mesh* const aMesh, const Skeleton* const aSkeleton);
		void Init(const Mesh* const aMesh, const Skeleton* const aSkeleton, const char* aTexturePath);

		//FilePath starts at Bin/Assets/Textures/
		void AddTexture(const char* aFilePath);
		void ClearTextures();

		void SetPose(const LocalSpacePose& aPose);
		void SetPose(const ModelSpacePose& aPose);
	public:
		void LookAt(const Math::Vector3f& aTargetPoint);

		//FilePath starts at  Bin/Shaders/
		void SetShader(const char* aPSShaderFile, const char* aVSShaderFile);
		void SetPosition(const Math::Vector3f& aPosition);
		void SetRotation(const Math::Vector3f& aRotationInDegree);
		void SetScale(const Math::Vector3f& aScale);
		void SetScale(const float aScale);
		void SetName(const std::string& aName);
		void SetBoundingBoxLineColor(const Math::Vector4f& aColor);

		const BoundingBox3D& GetBoundingBox() const;
		Math::Vector4f GetBoundingBoxLineColor() const;
		Math::Matrix4x4f GetMatrix() const;
		Math::Vector3f GetPosition() const;
		Math::Vector3f GetRotation() const;
		Math::Vector3f GetScale() const;
		std::string GetName() const;

		const Skeleton* GetSkeleton() const;
	private:
		Math::Matrix4x4f myBoneTransforms[SIMPLE_MAX_BONES];

		std::vector<std::shared_ptr<const Texture>> myTextures;

		std::string myName;

		std::shared_ptr<const Shader> myShader;
		const Mesh* myMesh;
		const Skeleton* mySkeleton;

		Math::Transform myTransform;
		Math::Vector4f myBoundingBoxColor;
	};
}