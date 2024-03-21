#pragma once
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"

struct Cell
{
	Drawer::Line line;
};

class Grid
{
public:
	Grid();
	~Grid();

	void Init(const size_t aCellSize, const size_t aRowAmount, const size_t aColAmount);
	void Render();
public:
	Cell* GetCellByPosition(Math::Vector2f aPosition);
private:
	size_t myCellSize;
	size_t myRowAmount;
	size_t myColAmount;

	size_t myStartX;
	size_t myStartY;
	size_t myEndX;
	size_t myEndY;

	std::vector<std::vector<Cell>> myCells;
	std::vector<Drawer::Line> myLines;
};