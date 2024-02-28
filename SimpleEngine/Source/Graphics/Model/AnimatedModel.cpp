#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/AnimatedModel.hpp"
#include "Engine/Global.hpp"

Simple::AnimatedModel::AnimatedModel()
    : myMesh(nullptr)
    , myName("Unnamed")
    , myBoundingBoxColor(1.0f, 1.0f, 0.0f, 1.0f)
{
    myShader = Global::GetGraphicsEngine()->GetDefaultShader();
}

Simple::AnimatedModel::~AnimatedModel()
{
    myMesh = nullptr;
    myShader = nullptr;
}

void Simple::AnimatedModel::Init(const Simple::Mesh* const aMesh)
{
    myMesh = aMesh;
    AddTexture("DefaultTexture.dds");
}

void Simple::AnimatedModel::Init(const Simple::Mesh* const aMesh, const char* aTexturePath)
{
    myMesh = aMesh;
    AddTexture(aTexturePath);
}

void Simple::AnimatedModel::AddTexture(const char* aFilePath)
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

void Simple::AnimatedModel::ClearTextures()
{
    myTextures.clear();
}

void Simple::AnimatedModel::SetPose(const LocalSpacePose& aPose)
{
    ModelSpacePose modelSpacePose;
    myMesh->mySkeleton.ConvertPoseToModelSpace(aPose, modelSpacePose);
    SetPose(modelSpacePose);
}

void Simple::AnimatedModel::SetPose(const ModelSpacePose& aPose)
{
    myMesh->mySkeleton.ApplyBindPoseInverse(aPose, myBoneTransforms);
}

void Simple::AnimatedModel::LookAt(const Math::Vector3f& aTargetPoint)
{
    myTransform.LookAt(aTargetPoint);
}

void Simple::AnimatedModel::SetShader(const char* aPSShaderFile, const char* aVSShaderFile)
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

void Simple::AnimatedModel::SetPosition(const Math::Vector3f& aPosition)
{
    myTransform.SetPosition(aPosition);
}

void Simple::AnimatedModel::SetRotation(const Math::Vector3f& aRotationInDegree)
{
    myTransform.SetRotation(aRotationInDegree);
}

void Simple::AnimatedModel::SetScale(const Math::Vector3f& aScale)
{
    myTransform.SetScale(aScale);
}

void Simple::AnimatedModel::SetScale(const float aScale)
{
    myTransform.SetScale(aScale);
}

void Simple::AnimatedModel::SetName(const std::string& aName)
{
    myName = aName;
}

void Simple::AnimatedModel::SetBoundingBoxLineColor(const Math::Vector4f& aColor)
{
    myBoundingBoxColor = aColor;
}

const Simple::BoundingBox3D& Simple::AnimatedModel::GetBoundingBox() const
{
    return myMesh->GetBoundingBox();
}

Math::Vector4f Simple::AnimatedModel::GetBoundingBoxLineColor() const
{
    return myBoundingBoxColor;
}

Math::Matrix4x4f Simple::AnimatedModel::GetMatrix() const
{
    return myTransform.GetMatrix();
}

Math::Vector3f Simple::AnimatedModel::GetPosition() const
{
    return myTransform.GetPosition();
}

Math::Vector3f Simple::AnimatedModel::GetRotation() const
{
    return myTransform.GetRotation();
}

Math::Vector3f Simple::AnimatedModel::GetScale() const
{
    return myTransform.GetScale();
}

std::string Simple::AnimatedModel::GetName() const
{
    return myName;
}