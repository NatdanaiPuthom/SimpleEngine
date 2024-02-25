#pragma once
#include "Game/Navmesh/Navmesh.hpp"

namespace Simple
{
	class Pathfinder
	{
	public:
		Pathfinder();

		void CalculatePath(const Math::Vector3f& aStartPosition, const Math::Vector3f& aTargetPosition);
		bool CanIWalkhere(const Math::Vector3f& aPosition);

		void Render() const;
		void RenderAStarPath() const;
		void RenderFunnelPath() const;
		void RenderStartEndPosition() const;

		void ClearPaths();
	public:
		void SetNavmesh(Simple::Navmesh* aNavmeshData);
		void SetStartPosition(const Math::Vector3f& aStartPosition);
		void SetTargetPosition(const Math::Vector3f& aTargetPosition);
	public:
		const std::vector<Math::Vector3f>& GetFunnelPath() const;
		const std::vector<Math::Vector3f>& GetAStarPath() const;
	private:
		float CalculateEuclideanDistance(const Math::Vector3f& aStartPosition, const Math::Vector3f& aTargetPosition) const;
		float CalculateAngle(Math::Vector3f aPointA, Math::Vector3f aPointB) const;
		float CalculateHeight(const Math::Vector3f& aPosition, const Math::Vector3f& aNormal, const Math::Vector3f& aVertex);

		void CalculateAStarPath(std::vector<Simple::Node>& aNodes, Simple::Node* aStartNode, Simple::Node* aTargetNode);
		void CalculateFunnelPath(const Simple::Navmesh& aNavmeshData);
		void CalculateLeftOrRight(int& aLeftPoint, int& aRightPoint, const std::array<int, 3>& aStartIndices, const std::array<int, 3>& aNextIndices) const;
		void CreateLines();
	private:
		std::vector<Simple::Node*> myAStarNodes;
		std::vector<Drawer::Line> myLines;
		std::vector<Math::Vector3f> myPathAStar;
		std::vector<Math::Vector3f> myPathFunnel;

		Simple::Navmesh* myNavmesh;
		Math::Vector3f myStartPosition;
		Math::Vector3f myTargetPosition;
	};
}