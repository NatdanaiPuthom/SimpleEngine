#pragma once

namespace Simple
{
	struct NavmeshData
	{
		std::vector<SimpleUtilities::Vector3f> myVertices;
		std::vector<int> myIndices;
	};

	struct Node
	{
		bool operator==(const Node& aNode)
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

			Node* myParent = nullptr;

			float f = FLT_MAX;
			float g = FLT_MAX;
			float h = FLT_MAX;

			bool myVisited = false;
		};

		NodeData myData;
		SimpleUtilities::Plane myPlane;
		SimpleUtilities::Vector3f myCenter;
		std::array<int, 3> myConnections;
		std::array<int, 3> myIndices;
	};

	class Navmesh
	{
	public:
		Navmesh();
		~Navmesh();

		void Init(const char* aObjFile);

		void RenderNavmesh();
		void RenderConnections();

		Simple::NavmeshData LoadNavmesh(const char* aObjFile);

		void SetNavmesh(const char* aObjFile);
	public:
		const std::array<SimpleUtilities::Vector3f, 3> GetVertices(const Simple::Node* aNode, const Simple::NavmeshData& aMesh) const;
		const Simple::NavmeshData& GetNavmesh() const;
		Simple::Node* GetNode(const SimpleUtilities::Vector3f& aPoint);
		Simple::Node* GetNode(const int aIndex);
		std::vector<Simple::Node>& GetNodes();
		int GetNodeIndexFromPoint(const SimpleUtilities::Vector3f& aPoint) const;
	private:
		void CreateNodes();
		void CalculateConnections();
	private:
		std::unordered_map<std::string, std::pair<Simple::NavmeshData, std::vector<Simple::Node>>> myNavmeshData;
		std::vector<Simple::Node> myCurrentNodes;
		std::vector<Drawer::Line> myNavmeshLines;
		std::vector<Drawer::Line> myConnectionLines;

		Simple::NavmeshData myCurrentMesh;
	};
}