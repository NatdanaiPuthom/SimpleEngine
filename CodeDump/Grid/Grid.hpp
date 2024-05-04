#pragma once
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"

struct Cell
{
	Drawer::Line line;
};

enum class eGridRotate
{
	None = 0,
	Inverse_X = 1,
	Inverse_Z = 2,
};

constexpr eGridRotate operator|(const eGridRotate lhs, const eGridRotate rhs)
{
	return static_cast<eGridRotate>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

class Grid
{
public:
	size_t myCellSize;
	size_t myRowAmount;
	size_t myColAmount;
public:
	Grid();
	~Grid();

	void Init(const size_t aCellSize, const size_t aRowAmount, const size_t aColAmount, const float aGridHeight = 0.0f, const eGridRotate aGridRotation = eGridRotate::None);
	void Render();
public:
	Cell* GetCellByPosition(Math::Vector2f aPosition);
private:
	std::vector<std::vector<Cell>> myCells;
	std::vector<Drawer::Line> myLines;
};