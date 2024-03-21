#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/Grid/Grid.hpp"

Grid::Grid()
	: myCellSize(0)
	, myRowAmount(0)
	, myColAmount(0)
	, myStartX(0)
	, myStartY(0)
	, myEndX(0)
	, myEndY(0)
{
}

Grid::~Grid()
{
}

void Grid::Init(const size_t aCellSize, const size_t aRowAmount, const size_t aColAmount)
{
	myCellSize = aCellSize;
	myRowAmount = aRowAmount;
	myColAmount = aColAmount;

	myCells.resize(myRowAmount, std::vector<Cell>(myColAmount));

	for (size_t i = 0; i <= myRowAmount; ++i)
	{
		Drawer::Line line;
		line.startPosition = { 0, 0,  static_cast<float>(i * myCellSize) };
		line.endPosition = { static_cast<float>(myCellSize * myRowAmount), 0, static_cast<float>(i * myCellSize) };

		myLines.push_back(line);
	}

	for (size_t i = 0; i <= myColAmount; ++i)
	{
		Drawer::Line line;
		line.startPosition = { static_cast<float>(i * myCellSize), 0, 0 };
		line.endPosition = { static_cast<float>(i * myCellSize), 0, static_cast<float>(myCellSize * myColAmount) };

		myLines.push_back(line);
	}

	for (size_t x = 0; x < myRowAmount; ++x)
	{
		for (size_t z = 0; z < myColAmount; ++z)
		{
			myCells[x][z].line.startPosition = { x * myCellSize + myCellSize / 2.0f, 0.0f, z * myCellSize + myCellSize / 2.0f };
			myCells[x][z].line.endPosition = { x * myCellSize + myCellSize / 2.0f, 10.0f, z * myCellSize + myCellSize / 2.0f };
		}
	}
}

void Grid::Render()
{
	Global::GetRenderer()->RenderLine(myLines);
}

Cell* Grid::GetCellByPosition(Math::Vector2f aPosition)
{
	const size_t indexX = static_cast<size_t>(std::ceil(aPosition.x / myCellSize));
	const size_t indexY = static_cast<size_t>(std::ceil(aPosition.y / myCellSize));

	return &myCells[indexX][indexY];
}