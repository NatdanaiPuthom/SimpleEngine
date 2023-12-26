#pragma once

class ModelInstance;

class PlaneReflection
{
public:
	PlaneReflection();
	~PlaneReflection();

	std::unique_ptr<ModelInstance> myModelInstance;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};