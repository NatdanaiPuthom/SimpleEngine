#pragma once
#include "Game/Navmesh/Navmesh.hpp"

namespace Simple
{
	class Pathfinder
	{
	public:
		Pathfinder();

		void CalculatePath(Simple::Navmesh& aNavmeshData, const SimpleUtilities::Vector3f& aStartPosition, const SimpleUtilities::Vector3f& aTargetPosition);
		void CalculatePath(const SimpleUtilities::Vector3f& aStartPosition, const SimpleUtilities::Vector3f& aTargetPosition);
		bool CanIWalkhere(const SimpleUtilities::Vector3f& aPosition);

		void Render() const;
		void RenderAStarPath() const;
		void RenderFunnelPath() const;
		void RenderStartEndPosition() const;

		void ClearPaths();
	public:
		void SetNavmesh(Simple::Navmesh* aNavmeshData);
	public:
		const std::vector<SimpleUtilities::Vector3f>& GetFunnelPath() const;
		const std::vector<SimpleUtilities::Vector3f>& GetAStarPath() const;
	private:
		float CalculateEuclideanDistance(const SimpleUtilities::Vector3f& aStartPosition, const SimpleUtilities::Vector3f& aTargetPosition) const;
		float CalculateAngle(SimpleUtilities::Vector3f aPointA, SimpleUtilities::Vector3f aPointB) const;
		float CalculateHeight(const SimpleUtilities::Vector3f& aPosition, const SimpleUtilities::Vector3f& aNormal, const SimpleUtilities::Vector3f& aVertex);

		void CalculateAStarPath(std::vector<Simple::Node>& aNodes, Simple::Node* aStartNode, Simple::Node* aTargetNode);
		void CalculateFunnelPath(const Simple::Navmesh& aNavmeshData);
		void CalculateLeftOrRight(int& aLeftPoint, int& aRightPoint, const std::array<int, 3>& aStartIndices, const std::array<int, 3>& aNextIndices) const;
		void CreateLines();
	private:
		std::vector<Simple::Node*> myAStarNodes;
		std::vector<Drawer::Line> myLines;
		std::vector<SimpleUtilities::Vector3f> myPathAStar;
		std::vector<SimpleUtilities::Vector3f> myPathFunnel;

		Simple::Navmesh* myNavmesh;
		SimpleUtilities::Vector3f myStartPosition;
		SimpleUtilities::Vector3f myTargetPosition;
	};
}