#pragma once
#include "Engine/SimpleUtilities/Plane.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include <array>
#include <vector>

namespace Simple
{
	struct NavmeshData
	{
		std::vector<Math::Vector3f> myVertices;
		std::vector<Math::Vector3f> myOffsetVertices;
		std::vector<std::pair<Math::Vector3f, Math::Vector3f>> offsetLines; //Debug purposes
		std::vector<int> myIndices;
	};

	struct Node
	{
		bool operator==(const Simple::Node& aNode)
		{
			return (memcmp(this, &aNode, sizeof(*this)) == 0) ? true : false;
		}

		bool isEqual(const Simple::Node& aNode) const
		{
			return (memcmp(this, &aNode, sizeof(*this)) == 0) ? true : false;
		}

		struct NodeData
		{
			void ResetNode()
			{
				myParent = nullptr;
				myVisited = false;

				f = FLT_MAX;
				g = FLT_MAX;
				h = FLT_MAX;
			}

			Simple::Node* myParent = nullptr;

			float f = FLT_MAX;
			float g = FLT_MAX;
			float h = FLT_MAX;

			bool myVisited = false;
		};

		NodeData myData;
		SimpleUtilities::Plane myPlane;
		Math::Vector3f myCenter;
		std::array<int, 3> myConnections;
		std::array<int, 3> myIndices;
	};

	class Navmesh
	{
	public:
		Navmesh();
		~Navmesh();

		void Init(const char* aObjFile);

		void RenderNavmesh() const;
		void RenderConnections() const;

		Simple::NavmeshData LoadNavmesh(const char* aObjFile);

		void SetNavmesh(const char* aObjFile);
	public:
		const std::array<Math::Vector3f, 3> GetVertices(const Simple::Node* aNode, const Simple::NavmeshData& aMesh) const;
		const Simple::NavmeshData& GetNavmesh() const;
		Simple::Node* GetNode(const Math::Vector3f& aPoint);
		Simple::Node* GetNode(const int aIndex);
		std::vector<Simple::Node>& GetNodes();
		int GetNodeIndexFromPoint(const Math::Vector3f& aPoint) const;
		std::pair<int, int> GetEdgeBetweenNodes(Simple::Node* aNode1, Simple::Node* aNode2) const;
	private:
		void CreateNodes();
		void CalculateConnections();
		void CalculateOffset();
		bool IsWall(const size_t aNodeIndex, const int aVertexIndex1, const int aVertexIndex2) const;
	private:
		std::unordered_map<std::string, std::pair<Simple::NavmeshData, std::vector<Simple::Node>>> myNavmeshData;
		std::vector<Simple::Node> myCurrentNodes;
		std::vector<Drawer::Line> myNavmeshLines;
		std::vector<Drawer::Line> myConnectionLines;
		std::vector<Drawer::Line> myOffsetLines;

		Simple::NavmeshData myCurrentMesh;
	};
}