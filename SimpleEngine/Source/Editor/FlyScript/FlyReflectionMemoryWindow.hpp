#pragma once
#include "NodeScript/Fly.hpp"

namespace Editor
{

	class FlyReflectionMemoryWindow
	{

	public:

		void Update();


	private:


		Fly::DataTypeFacade myCurrentDataType;

	};

}