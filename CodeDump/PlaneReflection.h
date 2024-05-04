#pragma once
#include "Graphics/Model/Model.hpp"

namespace Graphics
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