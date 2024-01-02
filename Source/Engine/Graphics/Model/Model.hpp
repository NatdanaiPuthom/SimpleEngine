#pragma once
#include "Engine/Graphics/Model/Mesh.hpp"

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
		Model();
		~Model();

		void Init(const Simple::Mesh* const aMesh);
		void Init(const Simple::Mesh* const aMesh, const char* aTexturePath);

		//FilePath starts at Bin/Assets/Textures/
		void AddTexture(const char* aFilePath);
		void ClearTextures();
	public:
		void LookAt(const SimpleUtilities::Vector3f& aTargetPoint);

		//FilePath starts at  Bin/Shaders/
		void SetShader(const char* aPSShaderFile, const char* aVSShaderFile);
		void SetPosition(const SimpleUtilities::Vector3f& aPosition);
		void SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree);
		void SetScale(const SimpleUtilities::Vector3f& aScale);
		void SetName(const std::string& aName);
		void SetBoundingBoxLineColor(const SimpleUtilities::Vector4f& aColor);

		const BoundingBox& GetBoundingBox() const;
		SimpleUtilities::Vector4f GetBoundingBoxLineColor() const;
		SimpleUtilities::Matrix4x4f GetMatrix() const;
		SimpleUtilities::Vector3f GetPosition() const;
		SimpleUtilities::Vector3f GetRotation() const;
		SimpleUtilities::Vector3f GetScale() const;
		std::string GetName() const;
	private:
		std::vector<std::shared_ptr<const Simple::Texture>> myTextures;
		std::string myName;
		const Simple::Mesh* myMesh;
		std::shared_ptr<const Simple::Shader> myShader;

		SimpleUtilities::Transform myTransform;
		SimpleUtilities::Vector4f myBoundingBoxColor;
	};
}