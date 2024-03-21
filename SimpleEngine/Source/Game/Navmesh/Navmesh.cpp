#include "Game/Precomplied/GamePch.hpp"
#include "Game/Navmesh/Navmesh.hpp"
#include <fstream>

namespace Simple
{
	Navmesh::Navmesh()
	{
	}

	Navmesh::~Navmesh()
	{
	}

	void Navmesh::Init(const char* aObjFile)
	{
		myCurrentNodes.clear();
		myCurrentMesh = LoadNavmesh(aObjFile);

		CreateNodes();
		CalculateConnections();
		CalculateOffset();

		myNavmeshData.emplace(std::string(aObjFile), std::make_pair(myCurrentMesh, myCurrentNodes));

		if (myNavmeshData.size() == 1)
			SetNavmesh(aObjFile);
	}

	void Navmesh::SetNavmesh(const char* aObjFile)
	{
		const auto& data = myNavmeshData[std::string(aObjFile)];
		myCurrentMesh = data.first;
		myCurrentNodes = data.second;

		myNavmeshLines.clear();
		myConnectionLines.clear();
		myOffsetLines.clear();

		auto adjustedVertex = [&](int index) -> Math::Vector3f {
			const auto& vertex = myCurrentMesh.myVertices[index];
			return Math::Vector3f(vertex.x, vertex.y, vertex.z);
			};

		Drawer::Line line;
		line.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		for (size_t nodeIndex = 0; nodeIndex < myCurrentMesh.myIndices.size(); nodeIndex += 3)
		{
			const Math::Vector3f vertices[] =
			{
				adjustedVertex(myCurrentMesh.myIndices[nodeIndex + 0]),
				adjustedVertex(myCurrentMesh.myIndices[nodeIndex + 1]),
				adjustedVertex(myCurrentMesh.myIndices[nodeIndex + 2])
			};

			for (int i = 0; i < 3; ++i)
			{
				line.startPosition = vertices[i];
				line.endPosition = vertices[(i + 1) % 3];
				myNavmeshLines.push_back(line);
			}
		}

		line.color = { 1,1,1,1 };

		for (size_t nodeIndex = 0; nodeIndex < myCurrentNodes.size(); ++nodeIndex)
		{
			const Node& currentNode = myCurrentNodes[nodeIndex];
			const Math::Vector3f currentNodeCenter(currentNode.myCenter.x, currentNode.myCenter.y, currentNode.myCenter.z);

			for (int connectionIndex : currentNode.myConnections)
			{
				if (connectionIndex != -1)
				{
					const Node& connectedNode = myCurrentNodes[connectionIndex];
					const Math::Vector3f connectedNodeCenter(connectedNode.myCenter.x, connectedNode.myCenter.y + 0.0001f, connectedNode.myCenter.z);

					line.startPosition = currentNodeCenter;
					line.endPosition = connectedNodeCenter;
					myConnectionLines.push_back(line);
				}
			}
		}

		for (size_t nodeIndex = 0; nodeIndex < myCurrentNodes.size(); ++nodeIndex)
		{
			const Node& currentNode = myCurrentNodes[nodeIndex];
			const Math::Vector3f currentNodeCenter(currentNode.myCenter.x, currentNode.myCenter.y, currentNode.myCenter.z);

			for (int connectionIndex : currentNode.myConnections)
			{
				if (connectionIndex != -1)
				{
					const Node& connectedNode = myCurrentNodes[connectionIndex];
					const Math::Vector3f connectedNodeCenter(connectedNode.myCenter.x, connectedNode.myCenter.y + 0.0001f, connectedNode.myCenter.z);

					line.startPosition = currentNodeCenter;
					line.endPosition = connectedNodeCenter;
					myConnectionLines.push_back(line);
				}
			}
		}

		line.color = { 1.0f, 0.0f, 0.0f, 1.0f };

		for (const auto& wall : myCurrentMesh.offsetLines)
		{
			line.startPosition = wall.first;
			line.endPosition = wall.second;

			line.startPosition.y += 0.2f;
			line.endPosition.y += 0.2f;

			myOffsetLines.push_back(line);
		}
	}

	void Navmesh::RenderNavmesh() const
	{
		auto renderer = Global::GetRenderer();

		renderer->RenderLine(myNavmeshLines);
		renderer->RenderLine(myOffsetLines);
	}

	void Navmesh::RenderConnections() const
	{
		auto renderer = Global::GetRenderer();
		renderer->RenderLine(myConnectionLines);
	}

	std::vector<Node>& Navmesh::GetNodes()
	{
		return myCurrentNodes;
	}

	Node* Navmesh::GetNode(const Math::Vector3f& aPoint)
	{
		const int index = GetNodeIndexFromPoint(aPoint);

		return GetNode(index);
	}

	Node* Navmesh::GetNode(const int aIndex)
	{
		if (aIndex < 0 || aIndex > static_cast<int>(myCurrentNodes.size() - 1))
			assert(true && "Invalid Node Index");

		return &myCurrentNodes[aIndex];;
	}

	const std::array<Math::Vector3f, 3> Navmesh::GetVertices(const Node* aNode, const Simple::NavmeshData& aMesh) const
	{
		std::array<Math::Vector3f, 3> vertices;

		for (int i = 0; i < 3; ++i)
		{
			vertices[i] = aMesh.myVertices[aNode->myIndices[i]];
		}

		return vertices;
	}

	const Simple::NavmeshData& Navmesh::GetNavmesh() const
	{
		return myCurrentMesh;
	}

	//https://www.youtube.com/watch?v=HYAgJN3x4GA
	int Navmesh::GetNodeIndexFromPoint(const Math::Vector3f& aPoint) const
	{
		for (int nodeIndex = 0; nodeIndex < myCurrentNodes.size(); ++nodeIndex)
		{
			const Node& node = myCurrentNodes[nodeIndex];

			const Math::Vector3f& pointA = myCurrentMesh.myVertices[node.myIndices[0]];
			const Math::Vector3f& pointB = myCurrentMesh.myVertices[node.myIndices[1]];
			const Math::Vector3f& pointC = myCurrentMesh.myVertices[node.myIndices[2]];

			float w1 = ((pointB.z - pointC.z) * (aPoint.x - pointC.x) + (pointC.x - pointB.x) * (aPoint.z - pointC.z)) /
				((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

			float w2 = ((pointC.z - pointA.z) * (aPoint.x - pointC.x) + (pointA.x - pointC.x) * (aPoint.z - pointC.z)) /
				((pointB.z - pointC.z) * (pointA.x - pointC.x) + (pointC.x - pointB.x) * (pointA.z - pointC.z));

			float w3 = 1.0f - w1 - w2;

			const bool inside = w1 >= 0 && w2 >= 0 && w3 >= 0;
			if (inside)
			{
				return nodeIndex;
			}
		}

		return -1;
	}

	std::pair<int, int> Navmesh::GetEdgeBetweenNodes(Node* aNode1, Node* aNode2) const
	{
		std::pair<int, int> edge(-1, -1);

		for (int i = 0; i < 3; ++i)
		{
			const int node1Index1 = aNode1->myIndices[i];
			const int node1Index2 = aNode1->myIndices[(i + 1) % 3];

			const int node2Index1 = aNode2->myIndices[i];
			const int node2Index2 = aNode2->myIndices[(i + 1) % 3];
			const int node2Index3 = aNode2->myIndices[(i + 2) % 3];

			if (node1Index1 == node2Index2 && node1Index2 == node2Index1)
			{
				edge.first = node1Index1;
				edge.second = node1Index2;
				break;
			}

			if (node1Index1 == node2Index3 && node1Index2 == node2Index2)
			{
				edge.first = node1Index1;
				edge.second = node1Index2;
				break;
			}

			if (node1Index1 == node2Index1 && node1Index2 == node2Index3)
			{
				edge.first = node1Index1;
				edge.second = node1Index2;
				break;
			}
		}

		return edge;
	}

	void Navmesh::CreateNodes()
	{
		for (int nodeIndex = 0; nodeIndex < myCurrentMesh.myIndices.size(); nodeIndex += 3)
		{
			Node newNode;
			newNode.myIndices[0] = myCurrentMesh.myIndices[nodeIndex + 0];
			newNode.myIndices[1] = myCurrentMesh.myIndices[nodeIndex + 1];
			newNode.myIndices[2] = myCurrentMesh.myIndices[nodeIndex + 2];
			newNode.myCenter = (myCurrentMesh.myVertices[newNode.myIndices[0]] + myCurrentMesh.myVertices[newNode.myIndices[1]] + myCurrentMesh.myVertices[newNode.myIndices[2]]) / 3.0f;

			const auto& vertex1 = myCurrentMesh.myVertices[newNode.myIndices[0]];
			const auto& vertex2 = myCurrentMesh.myVertices[newNode.myIndices[1]];
			const auto& vertex3 = myCurrentMesh.myVertices[newNode.myIndices[2]];
			newNode.myPlane.InitWith3Points(vertex1, vertex2, vertex3);

			myCurrentNodes.push_back(newNode);
		}
	}

	void Navmesh::CalculateConnections()
	{
		for (auto& node : myCurrentNodes)
		{
			node.myConnections.fill(-1);
		}

		for (int i = 0; i < myCurrentNodes.size(); ++i)
		{
			for (int j = i + 1; j < myCurrentNodes.size(); ++j)
			{
				int sharedVertices = 0;

				for (int vertice1 = 0; vertice1 < 3; ++vertice1)
				{
					for (int vertice2 = 0; vertice2 < 3; ++vertice2)
					{
						if (myCurrentNodes[i].myIndices[vertice1] == myCurrentNodes[j].myIndices[vertice2])
						{
							sharedVertices++;
						}
					}
				}

				if (sharedVertices == 2)
				{
					for (int connection = 0; connection < 3; ++connection)
					{
						if (myCurrentNodes[i].myConnections[connection] == -1)
						{
							myCurrentNodes[i].myConnections[connection] = j;
							break;
						}
					}

					for (int connection = 0; connection < 3; ++connection)
					{
						if (myCurrentNodes[j].myConnections[connection] == -1)
						{
							myCurrentNodes[j].myConnections[connection] = i;
							break;
						}
					}
				}
			}
		}

	}

	void Navmesh::CalculateOffset()
	{
		std::vector<std::pair<int, int>> walls;

		for (size_t nodeIndex = 0; nodeIndex < myCurrentNodes.size(); ++nodeIndex)
		{
			const int vertexIndex1 = myCurrentNodes[nodeIndex].myIndices[0];
			const int vertexIndex2 = myCurrentNodes[nodeIndex].myIndices[1];
			const int vertexIndex3 = myCurrentNodes[nodeIndex].myIndices[2];

			if (IsWall(nodeIndex, vertexIndex1, vertexIndex2))
			{
				walls.push_back({ vertexIndex1, vertexIndex2 });
			}

			if (IsWall(nodeIndex, vertexIndex2, vertexIndex3))
			{
				walls.push_back({ vertexIndex2, vertexIndex3 });
			}

			if (IsWall(nodeIndex, vertexIndex3, vertexIndex1))
			{
				walls.push_back({ vertexIndex3, vertexIndex1 });
			}
		}

		for (size_t i = 0; i < walls.size(); ++i)
		{
			bool connectionExists = false;

			for (size_t j = 0; j < walls.size(); ++j)
			{
				if (walls[i].first == walls[j].second)
				{
					connectionExists = true;
					break;
				}
			}
			if (!connectionExists)
			{
				assert(false);
			}
		}

		myCurrentMesh.myOffsetVertices.resize(myCurrentMesh.myVertices.size());

		for (size_t i = 0; i < myCurrentMesh.myOffsetVertices.size(); i++)
		{
			myCurrentMesh.myOffsetVertices[i] = myCurrentMesh.myVertices[i];
		}

		for (const std::pair<int, int>& wall1 : walls)
		{
			const int leftVertex = wall1.second;
			const int currentVertex = wall1.first;

			int rightVertex = -1;

			for (const std::pair<int, int>& wall2 : walls)
			{
				if (currentVertex == wall2.second)
				{
					rightVertex = wall2.first;
					break;
				}
			}
			const Math::Vector2f leftVector = (myCurrentMesh.myVertices[leftVertex] - myCurrentMesh.myVertices[currentVertex]).AsVector2XZ().GetNormalized();
			const Math::Vector2f rightVector = (myCurrentMesh.myVertices[rightVertex] - myCurrentMesh.myVertices[currentVertex]).AsVector2XZ().GetNormalized();

			const Math::Vector2f offsetDirection = (leftVector + rightVector).GetNormalized();
			const Math::Vector2f offsetPosition = myCurrentMesh.myVertices[currentVertex].AsVector2XZ() - offsetDirection * 0.1f;
			const Math::Vector3f offsetVertice = { offsetPosition.x, myCurrentMesh.myVertices[currentVertex].y, offsetPosition.y };

			myCurrentMesh.myOffsetVertices[currentVertex] = offsetVertice;
		}

		for (const auto& wall : walls)
		{
			const Math::Vector3f offsetStart = myCurrentMesh.myOffsetVertices[wall.first];
			const Math::Vector3f offsetEnd = myCurrentMesh.myOffsetVertices[wall.second];

			myCurrentMesh.offsetLines.push_back(std::make_pair(offsetStart, offsetEnd));
		}
	}

	bool Navmesh::IsWall(const size_t aNodeIndex, const int aVertexIndex1, const int aVertexIndex2) const
	{
		for (int i = 0; i < myCurrentNodes[aNodeIndex].myConnections.size(); ++i)
		{
			const int neighbourIndex = myCurrentNodes[aNodeIndex].myConnections[i];

			if (neighbourIndex == -1)
				continue;

			if ((aVertexIndex1 == myCurrentNodes[neighbourIndex].myIndices[0] && aVertexIndex2 == myCurrentNodes[neighbourIndex].myIndices[2]) ||
				(aVertexIndex1 == myCurrentNodes[neighbourIndex].myIndices[2] && aVertexIndex2 == myCurrentNodes[neighbourIndex].myIndices[1]) ||
				(aVertexIndex1 == myCurrentNodes[neighbourIndex].myIndices[1] && aVertexIndex2 == myCurrentNodes[neighbourIndex].myIndices[0]))
			{
				return false;
			}
		}

		return true;
	}

	NavmeshData Navmesh::LoadNavmesh(const char* aObjFile)
	{
		const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SIMPLE_NAVMESH_DIR);
		const std::string fileName = absolutePath + std::string(aObjFile);

		std::ifstream meshLoader;
		meshLoader.open(fileName, std::ios_base::in);

		if (!meshLoader.is_open())
			assert(false && "Failed to open navmesh obj file");

		NavmeshData mesh;

		while (meshLoader.eof() == false)
		{
			std::string reader;

			meshLoader >> reader; //read whole string and storage in "reader" variable until space, tab, newline

			if (reader == "v")
			{
				float x, y, z;

				meshLoader >> x >> y >> z;

				mesh.myVertices.push_back({ x, y, z });
			}
			else if (reader == "f")
			{
				std::string faceIndices;
				std::getline(meshLoader, faceIndices);

				std::istringstream iss(faceIndices);
				std::string indexString;

				while (iss >> indexString)
				{
					// Extract the first part before the '/'
					size_t pos = indexString.find('/');

					if (pos != std::string::npos)
					{
						std::string index = indexString.substr(0, pos);
						mesh.myIndices.push_back(std::stoi(index) - 1); // Adjusting to zero-based indexing
					}
					else
					{
						mesh.myIndices.push_back(std::stoi(indexString) - 1); // Adjusting to zero-based indexing
					}
				}
			}
		}

		return mesh;
	}

	/*Simple::NavmeshData Simple::Navmesh::LoadNavmesh(const char* aObjFile)  //NOTE(v9.21.0) : Older version, cannot read from Blender obj
	{
		const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SIMPLE_NAVMESH_DIR);
		const std::string fileName = absolutePath + std::string(aObjFile);

		std::ifstream meshLoader;
		meshLoader.open(fileName, std::ios_base::in);

		if (!meshLoader.is_open())
			assert(false && "Failed to open navmesh obj file");

		Simple::NavmeshData mesh;

		while (meshLoader.eof() == false)
		{
			std::string reader;
			meshLoader >> reader;

			if (reader == "v")
			{
				float x, y, z;
				meshLoader >> x;
				meshLoader >> y;
				meshLoader >> z;

				mesh.myVertices.push_back({ x, y,z });
			}
			else if (reader == "f")
			{
				int index1, index2, index3;

				meshLoader >> index1;
				meshLoader >> index2;
				meshLoader >> index3;

				mesh.myIndices.push_back(index1 - 1);
				mesh.myIndices.push_back(index2 - 1);
				mesh.myIndices.push_back(index3 - 1);
			}
		}

		return mesh;
	}*/
}