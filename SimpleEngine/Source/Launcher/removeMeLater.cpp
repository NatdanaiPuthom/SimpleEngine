#include "Launcher/removeMeLater.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include <filesystem>
#include <iostream>
#include "MainSingleton/MainSingleton.hpp"

namespace SimpleTest
{
	TestTree::TestTree()
	{
	}

	void TestTree::LoopTree(PathNode* aParent)
	{
		for (const auto& entry : std::filesystem::directory_iterator(aParent->path))
		{
			if (entry.is_directory())
			{
				std::shared_ptr<PathNode> child = std::make_shared<PathNode>();
				child->parent = aParent;
				child->isFolder = true;
				child->path = entry.path().string();
				child->fileName = entry.path().filename().string();

				aParent->childrens.push_back(child);

				LoopTree(aParent->childrens.back().get());
			}
			else
			{
				aParent->files.push_back(entry.path().filename().string());
			}
		}
	}

	void TestTree::Print(PathNode* aParent, size_t aLevel)
	{
		std::string spaces(aLevel * 2, ' ');

		std::cout << spaces << aParent->fileName << std::endl;

		for (std::shared_ptr<PathNode> child : aParent->childrens)
		{
			Print(child.get(), aLevel + 1);
		}

		for (auto& file : aParent->files)
		{
			std::cout << spaces << "  " << file << std::endl;
		}
	}

	void TestTree::PrintActiveDirectory(PathNode* aActiveDirectory)
	{
		for (std::shared_ptr<PathNode> child : aActiveDirectory->childrens)
		{
			std::cout << child->fileName << std::endl;
		}

		for (const std::string& file : aActiveDirectory->files)
		{
			std::cout << file << std::endl;
		}
	}

	void TestTree::Init()
	{
		myRoot.parent = nullptr;
		myRoot.isFolder = true;
		myRoot.fileName = "Assets";
		myRoot.path = SimpleUtilities::GetAbsolutePath("\\Assets\\");

		myCurrent = &myRoot;

		LoopTree(&myRoot);
		Print(&myRoot, 0);
	}

	void TestTree::Update()
	{
		auto& input = MainSingleton::GetInputManager();

		static int selectedIndex = 0;

		if (input.IsKeyPressed('P'))
		{
			std::cout << "\n\n\n" << std::endl;
			PrintActiveDirectory(myCurrent);

			if (myCurrent->childrens.size() > 0)
			{
				std::cout << "Current selected: " << myCurrent->childrens[selectedIndex]->fileName << std::endl;
			}
		}
		else if (input.IsKeyPressed('O'))
		{
			if (myCurrent->parent != nullptr)
			{
				myCurrent = myCurrent->parent;
				std::cout << "\n\n\n" << std::endl;
				PrintActiveDirectory(myCurrent);
			}
			else
			{
				std::cout << "Is already at root" << std::endl;
			}
		}
		else if (input.IsKeyPressed(VK_RETURN))
		{
			if (myCurrent->childrens.size() <= 0)
			{
				std::cout << "\n\n\n" << std::endl;
				std::cout << "No folder to enter: " << myCurrent->fileName << std::endl;
				PrintActiveDirectory(myCurrent);
				return;
			}

			myCurrent = myCurrent->childrens[selectedIndex].get();
			selectedIndex = 0;
			std::cout << "\n\n\n" << std::endl;
			std::cout << "Entered: " << myCurrent->fileName << std::endl;
			PrintActiveDirectory(myCurrent);
		}
		else if (input.IsKeyPressed(VK_UP))
		{
			int temp = selectedIndex - 1;

			if (temp >= 0)
			{
				selectedIndex = selectedIndex - 1;
				std::cout << "\n\n\n" << std::endl;
				std::cout << "Current selected: " << myCurrent->childrens[selectedIndex]->fileName << std::endl;
			}
			else
			{
				std::cout << "Cannot go up" << std::endl;
			}
		}
		else if (input.IsKeyPressed(VK_DOWN))
		{
			int temp = selectedIndex + 1;

			if (temp < myCurrent->childrens.size())
			{
				selectedIndex = selectedIndex + 1;
				std::cout << "\n\n\n" << std::endl;
				std::cout << "Current selected: " << myCurrent->childrens[selectedIndex]->fileName << std::endl;
			}
			else
			{
				std::cout << "Cannot go down" << std::endl;
			}
		}
	}
}

