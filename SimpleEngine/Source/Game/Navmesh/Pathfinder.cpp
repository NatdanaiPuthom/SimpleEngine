#include "Game/Precomplied/GamePch.hpp"
#include "Game/Navmesh/Pathfinder.hpp"
#include <queue>
#include <cassert>

namespace Simple
{
	Pathfinder::Pathfinder()
		: myNavmesh(nullptr)
	{
	}

	void Pathfinder::CalculatePath(const Math::Vector3f& aStartPosition, const Math::Vector3f& aTargetPosition)
	{
		assert(myNavmesh && "Navmesh pointer is null");

		const int startNodeIndex = myNavmesh->GetNodeIndexFromPoint(aStartPosition);
		const int targetNodeIndex = myNavmesh->GetNodeIndexFromPoint(aTargetPosition);

		if (startNodeIndex < 0 || targetNodeIndex < 0)
		{
			myStartPosition = aStartPosition;
			myTargetPosition = aStartPosition;
			myPathAStar.clear();
			myPathFunnel.clear();
			myLines.clear();
			return;
		}

		std::vector<Simple::Node>& nodes = myNavmesh->GetNodes();
		Simple::Node* startNode = myNavmesh->GetNode(aStartPosition);
		Simple::Node* targetNode = myNavmesh->GetNode(aTargetPosition);

		myStartPosition = aStartPosition;
		myTargetPosition = aTargetPosition;
		const Math::Vector3 vertice0 = myNavmesh->GetNavmesh().myVertices[targetNode->myIndices[0]];
		myTargetPosition.y = CalculateHeight(aTargetPosition, targetNode->myPlane.GetNormal(), vertice0);

		if (startNode == targetNode)
		{
			myPathFunnel.clear();
			myPathFunnel.push_back(myTargetPosition);
			CreateLines();
			return;
		}

		CalculateAStarPath(nodes, startNode, targetNode);
		CalculateFunnelPath(*myNavmesh);
		CreateLines();
	}

	bool Pathfinder::CanIWalkhere(const Math::Vector3f& aPosition)
	{
		const int nodeIndex = myNavmesh->GetNodeIndexFromPoint(aPosition);

		return nodeIndex >= 0;
	}

	void Pathfinder::CalculateAStarPath(std::vector<Simple::Node>& aNodes, Simple::Node* aStartNode, Simple::Node* aTargetNode)
	{
		myPathAStar.clear();
		myAStarNodes.clear();

		for (auto& node : aNodes)
		{
			node.myData.ResetNode();
		}

		class
		{
		public:
			bool operator()(Simple::Node* n1, Simple::Node* n2)
			{
				return n1->myData.f > n2->myData.f;
			}
		} comparator;

		std::priority_queue<Simple::Node*, std::vector<Simple::Node*>, decltype(comparator)> openList;

		aStartNode->myData.h = CalculateEuclideanDistance(aStartNode->myCenter, aTargetNode->myCenter);
		aStartNode->myData.g = 0;
		aStartNode->myData.f = aStartNode->myData.h + aStartNode->myData.g;
		aStartNode->myData.myVisited = true;
		openList.push(aStartNode);

		while (!openList.empty())
		{
			Simple::Node* currentNode = openList.top();
			openList.pop();
			currentNode->myData.myVisited = true;

			if (currentNode->isEqual(*aTargetNode))
				break;

			for (unsigned short i = 0; i < currentNode->myConnections.size(); ++i)
			{
				if (currentNode->myConnections[i] == -1)
					continue;

				Simple::Node* neighbour = &aNodes[currentNode->myConnections[i]];

				//const float distance = CalculateEuclideanDistance(neighbour->myCenter, currentNode->myCenter);
				std::pair<int, int> edgeIndices = myNavmesh->GetEdgeBetweenNodes(neighbour, currentNode);
				Math::Vector3f pos = Math::Lerp(myNavmesh->GetNavmesh().myVertices[edgeIndices.first], myNavmesh->GetNavmesh().myVertices[edgeIndices.second], 0.5f);
				const float distance = CalculateEuclideanDistance(pos, currentNode->myCenter);

				if (currentNode->myData.g + distance < neighbour->myData.g)
				{
					neighbour->myData.myParent = currentNode;
					neighbour->myData.g = currentNode->myData.g + distance;
					neighbour->myData.h = CalculateEuclideanDistance(neighbour->myCenter, aTargetNode->myCenter);
					neighbour->myData.f = neighbour->myData.g + neighbour->myData.h;

					openList.push(neighbour);
				}
			}
		}

		for (Simple::Node* node = aTargetNode; node != nullptr; node = node->myData.myParent)
		{
			myAStarNodes.insert(myAStarNodes.begin(), node);
		}

		for (size_t i = 0; i < myAStarNodes.size(); ++i)
		{
			myPathAStar.push_back(myAStarNodes[i]->myCenter);
		}
	}

	void Pathfinder::Render() const
	{
		RenderFunnelPath();
	}

	void Pathfinder::RenderFunnelPath() const
	{
		auto renderer = Global::GetRenderer();
		renderer->RenderLine(myLines);
	}

	void Pathfinder::RenderStartEndPosition() const
	{
		Math::Vector3f startPositionEnd = myStartPosition;
		Math::Vector3 targetPositionEnd = myTargetPosition;

		startPositionEnd.y += 100.0f;
		targetPositionEnd.y += 100.0f;

		Drawer::Line startLine;
		startLine.startPosition = myStartPosition;
		startLine.endPosition = startPositionEnd;
		startLine.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		Drawer::Line endLine;
		endLine.startPosition = myTargetPosition;
		endLine.endPosition = targetPositionEnd;
		endLine.color = { 0.0f, 1.0f, 1.0f, 1.0f };

		Drawer::Sphere startSphere;
		startSphere.position = myStartPosition;
		startSphere.color = { 0.0f, 1.0f, 0.0f, 1.0f };
		startSphere.radius = 0.1f;

		Drawer::Sphere endSphere;
		endSphere.position = myTargetPosition;
		endSphere.color = { 0.0f, 1.0f, 1.0f, 1.0f };
		endSphere.radius = 0.1f;

		auto renderer = Global::GetRenderer();

		renderer->RenderLine(startLine);
		renderer->RenderLine(endLine);

		renderer->RenderSphere(startSphere);
		renderer->RenderSphere(endSphere);
	}

	void Pathfinder::RenderAStarPath() const
	{
		const auto& paths = myPathAStar;
		const float offset = 0.001f;

		if (paths.size() <= 0)
			return;

		std::vector<Drawer::Line> lines;
		lines.reserve(paths.size());

		for (size_t i = 0; i < paths.size() - 1; ++i)
		{
			Math::Vector3f start = paths[i];
			Math::Vector3f end = paths[i + 1];

			start.y += offset;
			end.y += offset;

			Drawer::Line line;
			line.startPosition = start;
			line.endPosition = end;
			line.color = { 0.0f, 1.0f, 1.0f, 1.0f };

			lines.push_back(line);
		}

		auto renderer = Global::GetRenderer();
		renderer->RenderLine(lines);
	}

	void Pathfinder::SetNavmesh(Simple::Navmesh* aNavmeshData)
	{
		myNavmesh = aNavmeshData;
	}

	void Pathfinder::SetStartPosition(const Math::Vector3f& aStartPosition)
	{
		myStartPosition = aStartPosition;
	}

	void Pathfinder::SetTargetPosition(const Math::Vector3f& aTargetPosition)
	{
		myTargetPosition = aTargetPosition;
	}

	void Pathfinder::ClearPaths()
	{
		myPathAStar.clear();
		myPathFunnel.clear();
		myLines.clear();
	}

	const std::vector<Math::Vector3f>& Pathfinder::GetFunnelPath() const
	{
		return myPathFunnel;
	}

	const std::vector<Math::Vector3f>& Pathfinder::GetAStarPath() const
	{
		return myPathAStar;
	}

	void Pathfinder::CalculateFunnelPath(const Simple::Navmesh& aNavmeshData)
	{
		myPathFunnel.clear();

		if (myAStarNodes.size() == 2)
		{
			myPathFunnel.push_back(myTargetPosition);
			return;
		}
		else if (myAStarNodes.size() < 2)
			return;

		const std::vector<Math::Vector3f>& vertices = aNavmeshData.GetNavmesh().myVertices;
		std::vector<Math::Vector3f> wayPoints;

		Math::Vector3f apex;
		{
			Math::Vector3f startPosition = myStartPosition;
			startPosition.y = CalculateHeight(myAStarNodes[0]->myCenter, myAStarNodes[0]->myPlane.GetNormal(), vertices[myAStarNodes[0]->myIndices[0]]);
			apex = startPosition;
		}

		enum class eMover
		{
			Left,
			Right,
			None
		};

		struct FunnelPoint
		{
			int vertexIndex;
			size_t portalIndex;

		};

		struct Portal
		{
			eMover mover = eMover::None;
			FunnelPoint left;
			FunnelPoint right;
		};

		std::vector<Portal> portals;

		for (size_t i = 0; i < myAStarNodes.size() - 1; ++i)
		{
			Portal portal = {};
			CalculateLeftOrRight(portal.left.vertexIndex, portal.right.vertexIndex, myAStarNodes[i]->myIndices, myAStarNodes[i + 1]->myIndices);
			portal.left.portalIndex = i;
			portal.right.portalIndex = i;
			portals.push_back(portal);
		}

		for (size_t i = 1; i < portals.size(); ++i)
		{
			if (portals[i].left.vertexIndex == portals[i - 1].left.vertexIndex)
			{
				portals[i].mover = eMover::Right;
			}
			else
			{
				portals[i].mover = eMover::Left;
			}
		}

		float currentAngle = CalculateAngle(vertices[portals[0].left.vertexIndex] - apex, vertices[portals[0].right.vertexIndex] - apex);

		std::vector<FunnelPoint> leftSaved;
		std::vector<FunnelPoint> rightSaved;

		const auto& navmeshOffset = aNavmeshData.GetNavmesh().myOffsetVertices;

		for (size_t i = 1; i <= portals.size(); ++i)
		{
			if (i == portals.size())
			{
				Math::Vector3f endVector = myTargetPosition - apex;

				if (!rightSaved.empty())
				{
					float angle = CalculateAngle(endVector, vertices[rightSaved.front().vertexIndex] - apex);

					if (angle < 0)
					{
						apex = vertices[rightSaved.front().vertexIndex];
						i = rightSaved.front().portalIndex + 1ull;

						wayPoints.push_back(navmeshOffset[rightSaved.front().vertexIndex]);
						//wayPoints.push_back(apex);

						currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);

						rightSaved.clear();
						leftSaved.clear();

						continue;
					}
				}
				else
				{
					float angle = CalculateAngle(endVector, vertices[portals.back().right.vertexIndex] - apex);

					if (angle < 0)
					{
						apex = vertices[portals.back().right.vertexIndex];
						i = portals.back().right.portalIndex;

						wayPoints.push_back(navmeshOffset[portals.back().right.vertexIndex]);
						//wayPoints.push_back(apex);

						currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);

						rightSaved.clear();
						leftSaved.clear();

						continue;
					}
				}

				if (!leftSaved.empty())
				{
					float angle = CalculateAngle(vertices[leftSaved.front().vertexIndex] - apex, endVector);

					if (angle < 0)
					{
						apex = vertices[leftSaved.front().vertexIndex];
						i = leftSaved.front().portalIndex + 1ull;

						wayPoints.push_back(navmeshOffset[leftSaved.front().vertexIndex]);
						//wayPoints.push_back(apex);

						currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);


						rightSaved.clear();
						leftSaved.clear();
					}
				}
				else
				{
					float angle = CalculateAngle(vertices[portals.back().left.vertexIndex] - apex, endVector);

					if (angle < 0)
					{
						apex = vertices[portals.back().left.vertexIndex];
						i = portals.back().left.portalIndex;

						wayPoints.push_back(navmeshOffset[portals.back().left.vertexIndex]);
						//wayPoints.push_back(apex);

						currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);

						rightSaved.clear();
						leftSaved.clear();
					}
				}

				if (i == portals.size())
					break;

				continue;
			}

			FunnelPoint leftt = portals[i].left;
			FunnelPoint rightt = portals[i].right;

			if (leftSaved.size() > 0)
				leftt = leftSaved.front();
			if (rightSaved.size() > 0)
				rightt = rightSaved.front();

			switch (portals[i].mover)
			{
			case eMover::Right:
				rightt = portals[i].right;
				break;
			case eMover::Left:
				leftt = portals[i].left;
				break;
			}

			const float nextAngle = CalculateAngle(vertices[leftt.vertexIndex] - apex, vertices[rightt.vertexIndex] - apex);

			if (nextAngle < 0)
			{
				switch (portals[i].mover)
				{
				case eMover::Right:
				{
					apex = vertices[leftt.vertexIndex];
					size_t nextPortal = leftt.portalIndex;
					for (size_t j = leftt.portalIndex + 1ull; j < portals.size(); ++j)
					{
						if (portals[j].left.vertexIndex != leftt.vertexIndex)
						{
							nextPortal = j;
							break;
						}
					}
					i = nextPortal;
					wayPoints.push_back(navmeshOffset[leftt.vertexIndex]);
					//wayPoints.push_back(apex);

					break;
				}
				case eMover::Left:
				{
					apex = vertices[rightt.vertexIndex];
					size_t nextPortal = leftt.portalIndex;
					for (size_t j = rightt.portalIndex + 1ull; j < portals.size(); ++j)
					{
						if (portals[j].right.vertexIndex != rightt.vertexIndex)
						{
							nextPortal = j;
							break;
						}
					}
					i = nextPortal;
					wayPoints.push_back(navmeshOffset[rightt.vertexIndex]);
					//wayPoints.push_back(apex);
					break;
				}
				}


				currentAngle = CalculateAngle(vertices[portals[i].left.vertexIndex] - apex, vertices[portals[i].right.vertexIndex] - apex);


				rightSaved.clear();
				leftSaved.clear();
			}
			else if (nextAngle > currentAngle)
			{
				switch (portals[i].mover)
				{
				case eMover::Right:
					rightSaved.push_back(portals[i - 1].right);
					break;
				case eMover::Left:
					leftSaved.push_back(portals[i - 1].left);
					break;
				}
			}
			else if (nextAngle < currentAngle && nextAngle >= 0)
			{
				switch (portals[i].mover)
				{
				case eMover::Right:
					rightSaved.clear();
					break;
				case eMover::Left:
					leftSaved.clear();
					break;
				}

				currentAngle = nextAngle;
			}
		}

		{
			Math::Vector3f targetPosition = myTargetPosition;
			targetPosition.y = myAStarNodes.back()->myCenter.y;
			apex = targetPosition;
			wayPoints.push_back(apex);
		}

		myPathFunnel = wayPoints;
	}

	float Pathfinder::CalculateEuclideanDistance(const Math::Vector3f& aStartPosition, const Math::Vector3f& aTargetPosition) const
	{
		const float x = abs(aTargetPosition.x - aStartPosition.x);
		const float y = abs(aTargetPosition.y - aStartPosition.y);
		const float z = abs(aTargetPosition.z - aStartPosition.z);
		const float distance = sqrtf((x * x) + (y * y) + (z * z));

		return distance;
	}

	float Pathfinder::CalculateAngle(Math::Vector3f aPointLeft, Math::Vector3f aPointRight) const
	{
		aPointLeft.Normalize();
		aPointRight.Normalize();

		const float dotProduct = aPointLeft.x * aPointRight.x + aPointLeft.z * aPointRight.z;
		const float crossProduct = aPointRight.x * aPointLeft.z - aPointRight.z * aPointLeft.x;

		const float angleRadians = acos(dotProduct);
		float angleDegrees = angleRadians * (180.0f / 3.141592f);

		if (crossProduct < 0)
			angleDegrees = -angleDegrees;  // Flip the sign for clockwise rotations

		return angleDegrees;
	}

	float Pathfinder::CalculateHeight(const Math::Vector3f& aPosition, const Math::Vector3f& aNormal, const Math::Vector3f& aVertex)
	{
		const float distance = -Math::Dot(aNormal, aVertex);
		const float y = (-aNormal.x * aPosition.x - aNormal.z * aPosition.z - distance) / aNormal.y;
		return y;
	}

	void Pathfinder::CalculateLeftOrRight(int& aLeftPoint, int& aRightPoint, const std::array<int, 3>& aStartIndices, const std::array<int, 3>& aNextIndices) const
	{
		const std::array<int, 3> startIndices = aStartIndices;
		const std::array<int, 3> nextIndices = aNextIndices;

		unsigned int firstMatchedIndex = 0;
		unsigned int secondMatchedIndex = 0;

		for (unsigned int i = 0; i < startIndices.size(); ++i)
		{
			unsigned int& currentMatchedIndex = firstMatchedIndex == 0 ? firstMatchedIndex : secondMatchedIndex;

			for (unsigned int j = 0; j < nextIndices.size(); ++j)
			{
				if (startIndices[i] == nextIndices[j])
				{
					currentMatchedIndex = i;
					break;
				}
			}
		}

		if (firstMatchedIndex < 3 && secondMatchedIndex < 3)
		{
			if (((firstMatchedIndex + 1) % 3) == secondMatchedIndex) //Clockwise
			{
				aLeftPoint = startIndices[firstMatchedIndex];
				aRightPoint = startIndices[secondMatchedIndex];
			}
			else
			{
				aLeftPoint = startIndices[secondMatchedIndex];
				aRightPoint = startIndices[firstMatchedIndex];
			}
		}
	}

	void Pathfinder::CreateLines()
	{
		myLines.clear();

		const float offset = 0.1f;

		if (myPathFunnel.empty())
			return;

		Drawer::Line line;
		line.startPosition = myStartPosition;
		line.startPosition.y += offset;
		line.endPosition = myPathFunnel[0];
		line.endPosition.y += offset;
		line.color = { 1.0f, 1.0f, 0.0f, 1.0f };

		myLines.push_back(line);

		for (size_t i = 0; i < myPathFunnel.size() - 1; ++i)
		{
			Math::Vector3f start = myPathFunnel[i];
			Math::Vector3f end = myPathFunnel[i + 1];

			start.y += offset;
			end.y += offset;

			line.startPosition = start;
			line.endPosition = end;

			myLines.push_back(line);
		}
	}
}