#pragma once

namespace Simple
{
	class Model;
}

namespace Simple
{
	class PlaneReflection
	{
	public:
		PlaneReflection();
		~PlaneReflection();

		void Update();

		std::shared_ptr<Simple::Model> myModel;
		float myHeight;
	};
}