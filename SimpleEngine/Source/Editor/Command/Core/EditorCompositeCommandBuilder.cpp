#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorCompositeCommandBuilder.hpp"

namespace Editor
{


	void CompositeCommandBuilder::AddCommand(Command&& aCommand)
	{
		myCurrentCompositeCommand->AddCommand(std::move(aCommand));
	}
	
	void CompositeCommandBuilder::Begin(std::string_view aName)
	{
		if (myCurrentCompositeCommand)
		{
			myCurrentCompositeCommand->Begin(aName);
		}
		else
		{
			myCurrentCompositeCommand = std::make_unique<CompositeCommandInternal>(std::string(aName));
		}
	}

	std::optional<CompositeCommand> CompositeCommandBuilder::End()
	{
		if (!myCurrentCompositeCommand)
		{
			return std::nullopt;
		}

		if (myCurrentCompositeCommand->myCommands.empty())
		{
			return std::nullopt;
		}

		eEndCode endCode = myCurrentCompositeCommand->End();

		if (endCode == eEndCode::Ended)
		{
			std::unique_ptr<CompositeCommandInternal> internalComposite = std::move(myCurrentCompositeCommand);
			myCurrentCompositeCommand.reset();
			return internalComposite->Build();
		}
		
		return std::nullopt;

	}

	CompositeCommand CompositeCommandBuilder::CompositeCommandInternal::Build() const
	{
		return CompositeCommand(myName, myCommands);
	}

	void CompositeCommandBuilder::CompositeCommandInternal::AddCommand(Command&& aCommand)
	{
		if (myCurrentChild)
		{
			myCurrentChild->AddCommand(std::move(aCommand));
		}
		else
		{
			myCommands.emplace_back(std::move(aCommand));
		}
	}


	void CompositeCommandBuilder::CompositeCommandInternal::Begin(std::string_view aName)
	{
		if (myCurrentChild)
		{
			myCurrentChild->Begin(aName);
		}
		else
		{
			myCurrentChild = std::make_unique<CompositeCommandInternal>(std::string(aName));
		}
	}

	CompositeCommandBuilder::eEndCode CompositeCommandBuilder::CompositeCommandInternal::End()
	{
		if (myCurrentChild)
		{
			const eEndCode endCode = myCurrentChild->End();
			if (endCode == eEndCode::Ended)
			{
				const std::string compositeName = myCurrentChild->myName;
				myCommands.push_back(Command(Build(), compositeName));
				myCurrentChild.reset();
			}
			else if (endCode == eEndCode::Ended_Empty) // If the child's commands are empty we don't want to add the child to our commands
			{
				myCurrentChild.reset();
			}
			return eEndCode::InProgress;
		}
		else if (myCommands.empty())
		{
			return eEndCode::Ended_Empty;
		}

		return eEndCode::Ended;
	}
}