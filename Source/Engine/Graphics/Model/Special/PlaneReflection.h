#pragma once

class ModelInstance;

class PlaneReflection
{
public:
	PlaneReflection();
	~PlaneReflection();

	void Update();

	std::unique_ptr<ModelInstance> myModel;
	float myHeight;
};