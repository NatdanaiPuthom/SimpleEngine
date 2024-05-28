#pragma once
#include "Engine/ECS/Core/EntityManager.hpp"

namespace Test
{
	class TestShadow final
	{
	public:
		TestShadow();
		~TestShadow();

		void Init();
		void Update();
		void Render() const;
	};
}