#include "Engine/stdafx.h"
#include "Engine/Graphics/Model/ModelInstance.hpp"
#include "Engine/Graphics/Texture/Texture.hpp"

ModelInstance::ModelInstance()
	: myMesh(nullptr)
	, myName("Unnamed")
{
	myShader = SimpleGlobal::GetGraphicsEngine()->GetDefaultShader();
}

ModelInstance::~ModelInstance()
{
	myMesh = nullptr;
	myShader = nullptr;
}

void ModelInstance::Init(Mesh* aMesh)
{
	myMesh = aMesh;
	AddTexture("Assets/Textures/DefaultTexture.dds");
}

void ModelInstance::Init(Mesh* aMesh, const char* aTexturePath)
{
	myMesh = aMesh;
	AddTexture(aTexturePath);
}

void ModelInstance::Render()
{
	const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

	myShader->SetShader(context.Get());

	for (const auto& texture : myTextures)
	{
		texture->Bind(context, texture->GetSlot());
	}

	myMesh->BindMatrix(myTransform.GetMatrix());

	myMesh->Draw();
}

void ModelInstance::AddTexture(const char* aFilePath)
{
	std::shared_ptr<Texture> texture = SimpleGlobal::GetGraphicsEngine()->GetTexture(aFilePath);

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

void ModelInstance::ClearTextures()
{
	myTextures.clear();
}

void ModelInstance::SetShader(const char* aPSShaderFile, const char* aVSShaderFile)
{
	myShader = nullptr;
	myShader = SimpleGlobal::GetGraphicsEngine()->GetShader(aPSShaderFile, aVSShaderFile);

	if (myShader == nullptr)
	{
		if (SimpleGlobal::GetGraphicsEngine()->AddShader(aPSShaderFile, aVSShaderFile))
			assert(false && "Failed to add Shader to the bank");

		myShader = SimpleGlobal::GetGraphicsEngine()->GetShader(aPSShaderFile, aVSShaderFile);

		if (myShader == nullptr)
			assert(false && "Skill Issues");
	}
}

void ModelInstance::SetPosition(const SimpleUtilities::Vector3f& aPosition)
{
	myTransform.SetPosition(aPosition);
}

void ModelInstance::SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree)
{
	myTransform.SetRotation(aRotationInDegree);
}

void ModelInstance::SetScale(const SimpleUtilities::Vector3f& aScale)
{
	myTransform.SetScale(aScale);
}

void ModelInstance::SetName(const std::string& aName)
{
	myName = aName;
}

SimpleUtilities::Vector3f ModelInstance::GetPosition() const
{
	return myTransform.GetPosition();
}

SimpleUtilities::Vector3f ModelInstance::GetRotation() const
{
	return myTransform.GetRotation();
}

SimpleUtilities::Vector3f ModelInstance::GetScale() const
{
	return myTransform.GetScale();
}

std::string ModelInstance::GetName() const
{
	return myName;
}
