#pragma once

class ModelInstance;

class PlaneReflection
{
public:
	PlaneReflection();
	~PlaneReflection();

	std::unique_ptr<ModelInstance> myModel;
};