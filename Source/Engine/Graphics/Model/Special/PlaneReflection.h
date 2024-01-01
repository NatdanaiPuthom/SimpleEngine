#pragma once

class Model;

namespace Simple
{
	class PlaneReflection
	{
	public:
		PlaneReflection();
		~PlaneReflection();

		void Update();

		std::shared_ptr<Model> myModel;
		float myHeight;
	};
}