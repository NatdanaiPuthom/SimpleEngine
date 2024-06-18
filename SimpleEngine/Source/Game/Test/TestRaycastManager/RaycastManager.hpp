#pragma once

namespace Drawer
{
	struct Line;
	struct Sphere;
}

namespace Simple
{
	class RaycastManager final
	{
	public:
		RaycastManager();
		~RaycastManager();

		void Init();
		void Update();
		void Render();

		SimpleUtilities::Ray GetScreenPointToRay(const Math::Vector2f& aPosition);
	private:
		void CheckAABB3DCollision();
		void CheckRayNavmesh();
		void MoveObject();
	private:
		std::unique_ptr<Drawer::Line> myRaycastLine;
		std::unique_ptr<Drawer::Sphere> myDebugSphere;

		const float mySelectDelay;
		float myTimer;
		int mySelectedModelIndex;
		bool myShouldRenderDebugSphere;
	};
}