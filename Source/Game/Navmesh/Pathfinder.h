//#pragma once
//#include <SimpleMath.h>
//#include "Navmesh.h"
//#include <vector>
//#include "d3d11_api.h" //Draw lines
//
//namespace Bass
//{
//	class Pathfinder
//	{
//	public:
//		Pathfinder();
//
//		void CalculatePath(Bass::Navmesh& aNavmeshData, const Vector3& aStartPosition, const Vector3& aTargetPosition);
//		void CalculatePath(const Vector3& aStartPosition, const Vector3& aTargetPosition);
//		bool CanIWalkhere(const Vector3& aPosition);
//		bool CanIWalkHerePlusDistance(Vector3 pos, int index);
//
//		void Render();
//		void RenderAStarPath() const;
//		void RenderFunnelPath();
//		void RenderStartEndPosition() const;
//
//		void SetNavmesh(Bass::Navmesh* aNavmeshData);
//		void ClearPaths();
//	public:
//		const std::vector<Vector3>& GetFunnelPath() const;
//		const std::vector<Vector3>& GetAStarPath() const;
//	private:
//		float CalculateEuclideanDistance(const Vector3& aStartPosition, const Vector3& aTargetPosition) const;
//		float CalculateAngle(Vector3 aPointA, Vector3 aPointB) const;
//		float CalculateHeight(const Vector3& aPosition, const Vector3& aNormal, const Vector3& aVertex);
//		void CalculateAStarPath(std::vector<Bass::Node>& aNodes, Bass::Node* aStartNode, Bass::Node* aTargetNode);
//		void CalculateFunnelPath(const Bass::Navmesh& aNavmeshData);
//		void CalculateLeftOrRight(int& aLeftPoint, int& aRightPoint, const std::array<int, 3>& aStartIndices, const std::array<int, 3>& aNextIndices) const;
//		void CreateLines();
//	private:
//		Bass::Navmesh* myNavmesh;
//
//		std::vector<Bass::Node*> myAStarNodes;
//
//		std::vector<D3DAPI::Line> myLines;
//
//		std::vector<Vector3> myPathAStar;
//		std::vector<Vector3> myPathFunnel;
//
//		Vector3 myStartPosition;
//		Vector3 myTargetPosition;
//	};
//}
