#include "Engine/Precomplier/stdafx.h"

namespace Simple
{
	Model::Model()
		: myMesh(nullptr)
		, myName("Unnamed")
		, myBoundingBoxColor(1.0f, 1.0f, 0.0f, 1.0f)
	{
		myShader = SimpleGlobal::GetGraphicsEngine()->GetDefaultShader();
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
		std::shared_ptr<const Texture> texture = SimpleGlobal::GetGraphicsEngine()->GetTexture(aFilePath);

		if (texture == nullptr)
		{
			if (!SimpleGlobal::GetGraphicsEngine()->AddTexture(aFilePath))
				assert(false && "Failed to add Texture to the bank");

			texture = SimpleGlobal::GetGraphicsEngine()->GetTexture(aFilePath);

			if (texture == nullptr)
				assert(false && "Skill Issues");
		}

		myTextures.push_back(texture);
	}

	void Model::ClearTextures()
	{
		myTextures.clear();
	}

	void Model::LookAt(const SimpleUtilities::Vector3f& aTargetPoint)
	{
		myTransform.LookAt(aTargetPoint);
	}

	void Model::SetShader(const char* aPSShaderFile, const char* aVSShaderFile)
	{
		myShader = nullptr;
		myShader = SimpleGlobal::GetGraphicsEngine()->GetShader(aPSShaderFile, aVSShaderFile);

		if (myShader == nullptr)
		{
			if (!SimpleGlobal::GetGraphicsEngine()->AddShader(aPSShaderFile, aVSShaderFile))
				assert(false && "Failed to add Shader to the bank");

			myShader = SimpleGlobal::GetGraphicsEngine()->GetShader(aPSShaderFile, aVSShaderFile);

			if (myShader == nullptr)
				assert(false && "Skill Issues");
		}
	}

	void Model::SetPosition(const SimpleUtilities::Vector3f& aPosition)
	{
		myTransform.SetPosition(aPosition);
	}

	void Model::SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree)
	{
		myTransform.SetRotation(aRotationInDegree);
	}

	void Model::SetScale(const SimpleUtilities::Vector3f& aScale)
	{
		myTransform.SetScale(aScale);
	}

	void Model::SetName(const std::string& aName)
	{
		myName = aName;
	}

	void Model::SetBoundingBoxLineColor(const SimpleUtilities::Vector4f& aColor)
	{
		myBoundingBoxColor = aColor;
	}

	const BoundingBox& Model::GetBoundingBox() const
	{
		return myMesh->GetBoundingBox();
	}

	SimpleUtilities::Vector4f Model::GetBoundingBoxLineColor() const
	{
		return myBoundingBoxColor;
	}

	SimpleUtilities::Matrix4x4f Model::GetMatrix() const
	{
		return myTransform.GetMatrix();
	}

	SimpleUtilities::Vector3f Model::GetPosition() const
	{
		return myTransform.GetPosition();
	}

	SimpleUtilities::Vector3f Model::GetRotation() const
	{
		return myTransform.GetRotation();
	}

	SimpleUtilities::Vector3f Model::GetScale() const
	{
		return myTransform.GetScale();
	}

	std::string Model::GetName() const
	{
		return myName;
	}
}