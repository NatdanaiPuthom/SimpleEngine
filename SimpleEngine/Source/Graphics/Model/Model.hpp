#pragma once
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
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
	class Model final
	{
		friend class Drawer::Renderer;
		friend class Drawer::BoundingBoxDrawer;
	public:
		const Simple::Mesh* myMesh;

		Model();
		~Model();

		void Init(const Simple::Mesh* const aMesh);
		void Init(const Simple::Mesh* const aMesh, const char* aTexturePath);

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
	private:
		std::vector<std::shared_ptr<const Simple::Texture>> myTextures;
		std::string myName;
		std::shared_ptr<const Simple::Shader> myShader;

		Math::Transform myTransform;
		Math::Vector4f myBoundingBoxColor;

		Math::Matrix4x4f myBoneTransforms[SIMPLE_MAX_BONES];
	};
}