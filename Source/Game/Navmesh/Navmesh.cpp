#include "Game/Precomplier/stdafx.h"
#include <fstream>
#include "Game/Navmesh/Navmesh.hpp"

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

		auto adjustedVertex = [&](int index) -> SimpleUtilities::Vector3f {
			const auto& vertex = myCurrentMesh.myVertices[index];
			return SimpleUtilities::Vector3f(vertex.x, vertex.y + 0.0001f, vertex.z);
			};

		Drawer::Line line;
		line.color = { 0.0f, 1.0f, 0.0f, 1.0f };

		for (size_t nodeIndex = 0; nodeIndex < myCurrentMesh.myIndices.size(); nodeIndex += 3)
		{
			const SimpleUtilities::Vector3f vertices[] = 
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
			const SimpleUtilities::Vector3f currentNodeCenter(currentNode.myCenter.x, currentNode.myCenter.y, currentNode.myCenter.z);

			for (int connectionIndex : currentNode.myConnections)
			{
				if (connectionIndex != -1)
				{
					const Node& connectedNode = myCurrentNodes[connectionIndex];
					const SimpleUtilities::Vector3f connectedNodeCenter(connectedNode.myCenter.x, connectedNode.myCenter.y + 0.0001f, connectedNode.myCenter.z);

					line.startPosition = currentNodeCenter;
					line.endPosition = connectedNodeCenter;
					myConnectionLines.push_back(line);
				}
			}
		}
	}

	void Navmesh::RenderNavmesh()
	{
		auto renderer = SimpleGlobal::GetRenderer();

		for (const auto& line : myNavmeshLines)
		{
			renderer->RenderLine(line);
		}
	}

	void Navmesh::RenderConnections()
	{
		auto renderer = SimpleGlobal::GetRenderer();

		for (const auto& line : myConnectionLines)
		{
			renderer->RenderLine(line);
		}
	}

	std::vector<Simple::Node>& Navmesh::GetNodes()
	{
		return myCurrentNodes;
	}

	Simple::Node* Navmesh::GetNode(const SimpleUtilities::Vector3f& aPoint)
	{
		const int index = GetNodeIndexFromPoint(aPoint);

		return GetNode(index);
	}

	Simple::Node* Navmesh::GetNode(const int aIndex)
	{
		if (aIndex < 0 || aIndex > static_cast<int>(myCurrentNodes.size() - 1))
			assert(true && "Invalid Node Index");

		return &myCurrentNodes[aIndex];;
	}

	const std::array<SimpleUtilities::Vector3f, 3> Navmesh::GetVertices(const Simple::Node* aNode, const Simple::NavmeshData& aMesh) const
	{
		std::array<SimpleUtilities::Vector3f, 3> vertices;

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
	int Navmesh::GetNodeIndexFromPoint(const SimpleUtilities::Vector3f& aPoint) const
	{
		for (int nodeIndex = 0; nodeIndex < myCurrentNodes.size(); ++nodeIndex)
		{
			const Node& node = myCurrentNodes[nodeIndex];

			const SimpleUtilities::Vector3f& pointA = myCurrentMesh.myVertices[node.myIndices[0]];
			const SimpleUtilities::Vector3f& pointB = myCurrentMesh.myVertices[node.myIndices[1]];
			const SimpleUtilities::Vector3f& pointC = myCurrentMesh.myVertices[node.myIndices[2]];

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

			//Check if Intersection Point is Inside This Triangle
			//Vector3 v1 = myCurrentMesh.myVertices[node.myIndices[0]];
			//Vector3 v2 = myCurrentMesh.myVertices[node.myIndices[1]];
			//Vector3 v3 = myCurrentMesh.myVertices[node.myIndices[2]];

			//v1 -= aPoint;
			//v2 -= aPoint;
			//v3 -= aPoint;

			//Vector3 u = v2.Cross(v3);
			//Vector3 v = v3.Cross(v1);
			//Vector3 w = v1.Cross(v2);

			//// See if normals are facing the same direction, return nodeindex if true
			//if (u.Dot(v) > 0.f)
			//{
			//	return nodeIndex;
			//}
			//else if (u.Dot(w) > 0.0f)
			//{
			//	return nodeIndex;
			//}

		}

		return -1;
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

	Simple::NavmeshData Simple::Navmesh::LoadNavmesh(const char* aObjFile)
	{
		const std::string absolutePath = SimpleUtilities::GetPath(SIMPLE_NAVMESH_DIR);
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
	}
}