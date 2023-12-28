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
private:
	std::unique_ptr<Drawer::Line> myRaycastLine;
};