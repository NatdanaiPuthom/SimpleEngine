#pragma once

class Model;

class PlaneReflection
{
public:
	PlaneReflection();
	~PlaneReflection();

	void Update();

	std::unique_ptr<Model> myModel;
	float myHeight;
};