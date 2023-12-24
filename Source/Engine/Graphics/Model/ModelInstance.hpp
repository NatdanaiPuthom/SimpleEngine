#pragma once

class ConstantBuffer;
class Shader;
class Mesh;
class Texture;

class ModelInstance final
{
public:
	ModelInstance();
	~ModelInstance();

	void Init(Mesh* aMesh);
	void Init(Mesh* aMesh, const char* aTexturePath);
	void Render();

	const bool AddTexture(const char* aFilePath);
	void ClearTextures();
public:
	void SetShader(const char* aPSShaderFile, const char* aVSShaderFile);
	void SetPosition(const SimpleUtilities::Vector3f& aPosition);
	void SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree);
	void SetScale(const SimpleUtilities::Vector3f& aScale);
	void SetName(const std::string& aName);

	SimpleUtilities::Vector3f GetPosition() const;
	SimpleUtilities::Vector3f GetRotation() const;
	SimpleUtilities::Vector3f GetScale() const;
	std::string GetName() const;

private:
	std::vector<Texture*> myTextures;

	std::string myName;

	Mesh* myMesh;
	std::unique_ptr<Shader> myShader;

	SimpleUtilities::Transform myTransform;
};
