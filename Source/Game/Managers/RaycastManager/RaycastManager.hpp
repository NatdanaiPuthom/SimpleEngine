#pragma once

namespace Drawer
{
	struct Line;
}

class RaycastManager final
{
public:
	RaycastManager();
	~RaycastManager();

	void Update();
	void Render();

	SimpleUtilities::Ray GetMouseRay();
private:
	void CheckAABB3DCollision();
	void MoveObject();
private:
	std::unique_ptr<Drawer::Line> myRaycastLine;
	const float mySelectDelay;
	float myTimer;
	int mySelectedModelIndex;
};