#pragma once
#include "FlyCommand.hpp"
#include "../Memory/FlyHeapObject.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class CompositeCommand final
	{
	public:
		enum class eEndCode
		{
			Ended,
			InProgress,
			Ended_Empty,
		};
	public:

		CompositeCommand(const std::string& aName = std::string());

		void AddCommand(CommandNew&& aCommand);

		void Do() const;
		void Undo() const;

		void Begin(const std::string& aName);
		eEndCode End();

		const std::string& GetName();

	private:

		HeapObject<CompositeCommand, false> mCurrentChild;
		std::vector<CommandNew> mCommands;
		std::string mName;
	};
}