#include "Game/Precomplier/stdafx.h"
//#include <queue>
//#include <cmath>
//#include <cassert>
//
//#include "Pathfinder.h"
//
//Bass::Pathfinder::Pathfinder()
//{
//}
//
//void Bass::Pathfinder::CalculatePath(Bass::Navmesh& aNavmeshData, const Vector3& aStartPosition, const Vector3& aTargetPosition)
//{
//	const int startNodeIndex = aNavmeshData.GetNodeIndexFromPoint(aStartPosition);
//	const int targetNodeIndex = aNavmeshData.GetNodeIndexFromPoint(aTargetPosition);
//
//	if (startNodeIndex < 0 || targetNodeIndex < 0)
//	{
//		myStartPosition = aStartPosition;
//		myTargetPosition = aStartPosition;
//		myPathAStar.clear();
//		myPathFunnel.clear();
//		myLines.clear();
//		return;
//	}
//
//	std::vector<Bass::Node>& nodes = aNavmeshData.GetNodes();
//	Bass::Node* startNode = aNavmeshData.GetNode(aStartPosition);
//	Bass::Node* targetNode = aNavmeshData.GetNode(aTargetPosition);
//
//	myStartPosition = aStartPosition;
//	myTargetPosition = aTargetPosition;
//	const Vector3 vertice0 = aNavmeshData.GetNavmesh().myVertices[targetNode->myIndices[0]];
//	myTargetPosition.y = CalculateHeight(aTargetPosition, targetNode->myPlane.GetNormal(), vertice0);
//
//	if (startNode == targetNode)
//	{
//		myPathFunnel.clear();
//		myPathFunnel.push_back(myTargetPosition);
//		CreateLines();
//		return;
//	}
//
//	CalculateAStarPath(nodes, startNode, targetNode);
//	CalculateFunnelPath(aNavmeshData);
//	CreateLines();
//}
//
//void Bass::Pathfinder::CalculatePath(const Vector3& aStartPosition, const Vector3& aTargetPosition)
//{
//	assert(myNavmesh && "Navmesh pointer is null");
//
//	const int startNodeIndex = myNavmesh->GetNodeIndexFromPoint(aStartPosition);
//	const int targetNodeIndex = myNavmesh->GetNodeIndexFromPoint(aTargetPosition);
//
//	if(startNodeIndex < 0 || targetNodeIndex < 0)
//	{
//		myStartPosition = aStartPosition;
//		myTargetPosition = aStartPosition;
//		myPathAStar.clear();
//		myPathFunnel.clear();
//		myLines.clear();
//		return;
//	}
//
//	std::vector<Bass::Node>& nodes = myNavmesh->GetNodes();
//	Bass::Node* startNode = myNavmesh->GetNode(aStartPosition);
//	Bass::Node* targetNode = myNavmesh->GetNode(aTargetPosition);
//
//	myStartPosition = aStartPosition;
//	myTargetPosition = aTargetPosition;
//	const Vector3 vertice0 = myNavmesh->GetNavmesh().myVertices[targetNode->myIndices[0]];
//	myTargetPosition.y = CalculateHeight(aTargetPosition, targetNode->myPlane.GetNormal(), vertice0);
//
//	if(startNode == targetNode)
//	{
//		myPathFunnel.clear();
//		myPathFunnel.push_back(myTargetPosition);
//		CreateLines();
//		return;
//	}
//
//	CalculateAStarPath(nodes, startNode, targetNode);
//	CalculateFunnelPath(*myNavmesh);
//	CreateLines();
//}
//
//bool Bass::Pathfinder::CanIWalkhere(const Vector3& aPosition)
//{
//	const int nodeIndex = myNavmesh->GetNodeIndexFromPoint(aPosition);
//	
//	return nodeIndex >= 0;
//}
//
//bool Bass::Pathfinder::CanIWalkHerePlusDistance(Vector3 pos, int index)
//{
//	index;
//	const int nodeIndex = myNavmesh->GetNodeIndexFromPoint(pos);
//	if (nodeIndex <= 0)
//	{
//		return false;
//	}
//	Bass::Node* node = myNavmesh->GetNode(nodeIndex);
//	
//	float distance = Vector3::DistanceSquared(pos, node->myCenter);
//	
//	if (distance > 0.1f)
//	{
//		return false;
//	}
//	return true;
//}
//
//void Bass::Pathfinder::CalculateAStarPath(std::vector<Bass::Node>& aNodes, Bass::Node* aStartNode, Bass::Node* aTargetNode)
//{
//	myPathAStar.clear();
//	myAStarNodes.clear();
//
//	for (auto& node : aNodes)
//	{
//		node.myData.ResetNode();
//	}
//
//	class
//	{
//	public:
//		bool operator()(Bass::Node* n1, Bass::Node* n2)
//		{
//			return n1->myData.f > n2->myData.f;
//		}
//	} comparator;
//
//	std::priority_queue<Bass::Node*, std::vector<Bass::Node*>, decltype(comparator)> openList;
//
//	aStartNode->myData.h = CalculateEuclideanDistance(aStartNode->myCenter, aTargetNode->myCenter);
//	aStartNode->myData.g = 0;
//	aStartNode->myData.f = aStartNode->myData.h + aStartNode->myData.g;
//	aStartNode->myData.myVisited = true;
//	openList.push(aStartNode);
//
//	while (!openList.empty())
//	{
//		Bass::Node* currentNode = openList.top();
//		openList.pop();
//		currentNode->myData.myVisited = true;
//
//		if (*currentNode == *aTargetNode)
//			break;
//
//		for (unsigned short i = 0; i < currentNode->myConnections.size(); ++i)
//		{
//			if (currentNode->myConnections[i] == -1)
//				continue;
//
//			Bass::Node* neighbour = &aNodes[currentNode->myConnections[i]];
//
//			const float distance = CalculateEuclideanDistance(neighbour->myCenter, currentNode->myCenter);
//
//			if (currentNode->myData.g + distance < neighbour->myData.g)
//			{
//				neighbour->myData.myParent = currentNode;
//				neighbour->myData.g = currentNode->myData.g + distance;
//				neighbour->myData.h = CalculateEuclideanDistance(neighbour->myCenter, aTargetNode->myCenter);
//				neighbour->myData.f = neighbour->myData.g + neighbour->myData.h;
//
//				openList.push(neighbour);
//			}
//		}
//	}
//
//	for (Bass::Node* node = aTargetNode; node != nullptr; node = node->myData.myParent)
//	{
//		myAStarNodes.insert(myAStarNodes.begin(), node);
//	}
//
//	for (size_t i = 0; i < myAStarNodes.size(); ++i)
//	{
//		myPathAStar.push_back(myAStarNodes[i]->myCenter);
//	}
//}
//
//void Bass::Pathfinder::Render()
//{
//	RenderFunnelPath();
//}
//
//void Bass::Pathfinder::RenderFunnelPath()
//{
//#if DRAW_DEBUG_LINES
//
//	if(myLines.size() <= 0)
//		return;
//
//	D3DAPI::DrawLineArray(myLines.data(), (int) myLines.size());
//
//#endif // DRAW_DEBUG_LINES
//}
//
//void Bass::Pathfinder::RenderStartEndPosition() const
//{
//	Vector3 startPositionEnd = myStartPosition;
//	Vector3 targetPositionEnd = myTargetPosition;
//
//	startPositionEnd.y += 10.0f;
//	targetPositionEnd.y += 10.0f;
//
//#if DRAW_DEBUG_LINES
//
//	D3DAPI::DrawLine(myStartPosition, startPositionEnd, {1, 1, 1, 1});
//	D3DAPI::DrawLine(myTargetPosition, targetPositionEnd, {1, 1, 1, 1});
//
//#endif // _DEBUG
//}
//
//void Bass::Pathfinder::SetNavmesh(Bass::Navmesh* aNavmeshData)
//{
//	myNavmesh = aNavmeshData;
//}
//
//void Bass::Pathfinder::ClearPaths()
//{
//	myPathAStar.clear();
//	myPathFunnel.clear();
//	myLines.clear();
//}
//
//void Bass::Pathfinder::RenderAStarPath() const
//{
//	const auto& paths = myPathAStar;
//	const float offset = 0.001f;
//
//	if (paths.size() <= 0)
//		return;
//
//	for (size_t i = 0; i < paths.size() - 1; ++i)
//	{
//		Vector3 start = paths[i];
//		Vector3 end = paths[i + 1];
//
//		start.y += offset;
//		end.y += offset;
//
//#if DRAW_DEBUG_LINES
//
//		D3DAPI::DrawLine(start, end, {0, 1, 1, 1});
//
//#endif // _DEBUG
//	}
//}
//
//const std::vector<Vector3>& Bass::Pathfinder::GetFunnelPath() const
//{
//	return myPathFunnel;
//}
//
//const std::vector<Vector3>& Bass::Pathfinder::GetAStarPath() const
//{
//	return myPathAStar;
//}
//
//void Bass::Pathfinder::CalculateFunnelPath(const Bass::Navmesh& aNavmeshData)
//{
//	myPathFunnel.clear();
//
//	if (myAStarNodes.size() == 2)
//	{
//		myPathFunnel.push_back(myTargetPosition);
//		return;
//	}
//	else if (myAStarNodes.size() < 2)
//		return;
//
//	const std::vector<Vector3> vertices = aNavmeshData.GetNavmesh().myVertices;
//	std::vector<Vector3> wayPoints;
//
//	Vector3 apex;
//	{
//		Vector3 startPosition = myStartPosition;
//		startPosition.y = CalculateHeight(myAStarNodes[0]->myCenter, myAStarNodes[0]->myPlane.GetNormal(), vertices[myAStarNodes[0]->myIndices[0]]);
//		apex = startPosition;
//	}
//
//	enum class eMover
//	{
//		Left,
//		Right,
//		None
//	};
//
//	struct FunnelPoint
//	{
//		int vertexIndex;
//		size_t portalIndex;
//
//	};
//
//	struct Portal
//	{
//		eMover mover = eMover::None;
//		FunnelPoint left;
//		FunnelPoint right;
//	};
//
//	std::vector<Portal> portals;
//
//	for (size_t i = 0; i < myAStarNodes.size() - 1; ++i)
//	{
//		Portal portal = {};
//		CalculateLeftOrRight(portal.left.vertexIndex, portal.right.vertexIndex, myAStarNodes[i]->myIndices, myAStarNodes[i + 1]->myIndices);
//		portal.left.portalIndex = i;
//		portal.right.portalIndex = i;
//		portals.push_back(portal);
//	}
//
//	for (size_t i = 1; i < portals.size(); ++i)
//	{
//		if (portals[i].left.vertexIndex == portals[i - 1].left.vertexIndex)
//		{
//			portals[i].mover = eMover::Right;
//		}
//		else
//		{
//			portals[i].mover = eMover::Left;
//		}
//	}
//
//	float currentAngle = CalculateAngle(vertices[portals[0].left.vertexIndex] - apex, vertices[portals[0].right.vertexIndex] - apex);
//
//	std::vector<FunnelPoint> left;
//	std::vector<FunnelPoint> right;
//
//	for (size_t i = 1; i <= portals.size(); ++i)
//	{
//		if (i == portals.size())
//		{
//			Vector3 endVector = myTargetPosition - apex;
//
//
//			if (!right.empty())
//			{
//				float angle = CalculateAngle(endVector, vertices[right.front().vertexIndex] - apex);
//
//				if (angle < 0)
//				{
//					apex = vertices[right.front().vertexIndex];
//					i = right.front().portalIndex;
//
//					wayPoints.push_back(apex);
//
//					currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);
//
//					right.clear();
//					left.clear();
//
//					continue;
//				}
//			}
//			else
//			{
//				float angle = CalculateAngle(endVector, vertices[portals.back().right.vertexIndex] - apex);
//
//				if (angle < 0)
//				{
//					apex = vertices[portals.back().right.vertexIndex];
//					i = portals.back().right.portalIndex;
//
//					wayPoints.push_back(apex);
//
//					currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);
//
//					right.clear();
//					left.clear();
//
//					continue;
//				}
//			}
//
//			if (!left.empty())
//			{
//				float angle = CalculateAngle(vertices[left.front().vertexIndex] - apex, endVector);
//
//				if (angle < 0)
//				{
//					apex = vertices[left.front().vertexIndex];
//					i = left.front().portalIndex;
//
//					wayPoints.push_back(apex);
//
//					currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);
//
//					right.clear();
//					left.clear();
//				}
//			}
//			else
//			{
//				float angle = CalculateAngle(vertices[portals.back().left.vertexIndex] - apex, endVector);
//
//				if (angle < 0)
//				{
//					apex = vertices[portals.back().left.vertexIndex];
//					i = portals.back().left.portalIndex;
//
//					wayPoints.push_back(apex);
//
//					currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);
//
//					right.clear();
//					left.clear();
//				}
//			}
//
//			if (i == portals.size())
//				break;
//
//			continue;
//		}
//
//		float nextAngle = -1;
//		FunnelPoint leftt = portals[i].left;
//		FunnelPoint rightt = portals[i].right;
//
//		if (left.size() > 0)
//			leftt = left.front();
//		if (right.size() > 0)
//			rightt = right.front();
//
//		switch (portals[i].mover)
//		{
//		case eMover::Right:
//			rightt = portals[i].right;
//			break;
//		case eMover::Left:
//			leftt = portals[i].left;
//			break;
//		}
//
//		nextAngle = CalculateAngle(vertices[leftt.vertexIndex] - apex, vertices[rightt.vertexIndex] - apex);
//
//		if (nextAngle < 0)
//		{
//			switch (portals[i].mover)
//			{
//			case eMover::Right:
//				apex = vertices[leftt.vertexIndex];
//				i = leftt.portalIndex;
//				break;
//			case eMover::Left:
//				apex = vertices[rightt.vertexIndex];
//				i = rightt.portalIndex;
//				break;
//			}
//
//			wayPoints.push_back(apex);
//
//			currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);
//
//
//			right.clear();
//			left.clear();
//		}
//		else if (nextAngle > currentAngle)
//		{
//			switch (portals[i].mover)
//			{
//			case eMover::Right:
//				right.push_back(portals[i - 1].right);
//				break;
//			case eMover::Left:
//				left.push_back(portals[i - 1].left);
//				break;
//			}
//		}
//		else if (nextAngle < currentAngle && nextAngle >= 0)
//		{
//			switch (portals[i].mover)
//			{
//			case eMover::Right:
//				right.clear();
//				break;
//			case eMover::Left:
//				left.clear();
//				break;
//			}
//
//			currentAngle = nextAngle;
//		}
//	}
//
//	{
//		Vector3 targetPosition = myTargetPosition;
//		targetPosition.y = myAStarNodes.back()->myCenter.y;
//		apex = targetPosition;
//		wayPoints.push_back(apex);
//	}
//
//	myPathFunnel = wayPoints;
//}
//
//float Bass::Pathfinder::CalculateEuclideanDistance(const Vector3& aStartPosition, const Vector3& aTargetPosition) const
//{
//	const float x = abs(aTargetPosition.x - aStartPosition.x);
//	const float y = abs(aTargetPosition.y - aStartPosition.y);
//	const float z = abs(aTargetPosition.z - aStartPosition.z);
//	const float distance = sqrtf((x * x) + (y * y) + (z * z));
//
//	return distance;
//}
//
//float Bass::Pathfinder::CalculateAngle(Vector3 aPointLeft, Vector3 aPointRight) const
//{
//	aPointLeft.Normalize();
//	aPointRight.Normalize();
//
//	const float dotProduct = aPointLeft.x * aPointRight.x + aPointLeft.z * aPointRight.z;
//	const float crossProduct = aPointRight.x * aPointLeft.z - aPointRight.z * aPointLeft.x;
//
//	const float angleRadians = acos(dotProduct);
//	float angleDegrees = angleRadians * (180.0f / 3.141592f);
//
//	if (crossProduct < 0)
//		angleDegrees = -angleDegrees;  // Flip the sign for clockwise rotations
//
//	return angleDegrees;
//}
//
//float Bass::Pathfinder::CalculateHeight(const Vector3& aPosition, const Vector3& aNormal, const Vector3& aVertex)
//{
//	const float distance = -aNormal.Dot(aVertex);
//	const float y = (-aNormal.x * aPosition.x - aNormal.z * aPosition.z - distance) / aNormal.y;
//	return y;
//}
//
//void Bass::Pathfinder::CalculateLeftOrRight(int& aLeftPoint, int& aRightPoint, const std::array<int, 3>& aStartIndices, const std::array<int, 3>& aNextIndices) const
//{
//	const std::array<int, 3> startIndices = aStartIndices;
//	const std::array<int, 3> nextIndices = aNextIndices;
//
//	unsigned int firstMatchedIndex = 0;
//	unsigned int secondMatchedIndex = 0;
//
//	for (unsigned int i = 0; i < startIndices.size(); ++i)
//	{
//		unsigned int& currentMatchedIndex = firstMatchedIndex == 0 ? firstMatchedIndex : secondMatchedIndex;
//
//		for (unsigned int j = 0; j < nextIndices.size(); ++j)
//		{
//			if (startIndices[i] == nextIndices[j])
//			{
//				currentMatchedIndex = i;
//				break;
//			}
//		}
//	}
//
//	if (firstMatchedIndex < 3 && secondMatchedIndex < 3)
//	{
//		if (((firstMatchedIndex + 1) % 3) == secondMatchedIndex) //Clockwise
//		{
//			aLeftPoint = startIndices[firstMatchedIndex];
//			aRightPoint = startIndices[secondMatchedIndex];
//		}
//		else
//		{
//			aLeftPoint = startIndices[secondMatchedIndex];
//			aRightPoint = startIndices[firstMatchedIndex];
//		}
//	}
//}
//
//void Bass::Pathfinder::CreateLines()
//{
//	myLines.clear();
//
//	const float offset = 0.001f;
//
//	if (myPathFunnel.empty())
//		return;
//
//	for (size_t i = 0; i < myPathFunnel.size() - 1; ++i)
//	{
//		Vector3 start = myPathFunnel[i];
//		Vector3 end = myPathFunnel[i + 1];
//
//		start.y += offset;
//		end.y += offset;
//
//#if DRAW_DEBUG_LINES
//
//		D3DAPI::DrawLine(start, end, {0, 1, 1, 1});
//
//#endif // _DEBUG
//
//
//
//		const Vector4 color(0, 1, 1, 1);
//		D3DAPI::Line line = { };
//		line.color = color;
//
//		line.start = start;
//		line.end = end;
//		myLines.push_back(line);
//	}
//}
