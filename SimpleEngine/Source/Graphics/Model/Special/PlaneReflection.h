#pragma once
#include "Graphics/Model/Model.hpp"

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