#pragma once
#include <vector>
#include <string>
#include <memory>

namespace SimpleTest
{
	struct PathNode
	{
		PathNode* parent = nullptr;

		std::string path = "empty path";
		std::string fileName = "empty name";

		std::vector<std::shared_ptr<PathNode>> childrens;
		std::vector<std::string> files;

		bool isFolder = false;
	};

	class TestTree
	{
	public:
		TestTree();

		void Init();
		void Update();
		void LoopTree(PathNode* aParent);
		void Print(PathNode* aParent, size_t aLevel);
		void PrintActiveDirectory(PathNode* aActiveDirectory);

		PathNode myRoot;
		PathNode* myCurrent = nullptr;
	};
}