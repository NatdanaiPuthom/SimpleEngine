#pragma once

namespace ECS
{
	class System
	{
	public:
		System() {};
		virtual ~System() {};

		virtual void Init() {};
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void EarlyUpdate() {};
		virtual void FixedUpdate() {};
		virtual void LateUpdate() {};
	};
}