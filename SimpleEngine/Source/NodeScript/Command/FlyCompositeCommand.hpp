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

		template<typename... Args>
		void AddCommand(Args&&... aArgs);

		void Do() const;
		void Undo() const;

		void Begin(const std::string& aName);
		eEndCode End();

		const std::string& GetName();

	private:

		HeapObject<CompositeCommand, false> mCurrentChild;
		std::vector<Command> mCommands;
		std::string mName;
	};

	template<typename ...Args>
	inline void CompositeCommand::AddCommand(Args && ...aArgs)
	{
		if (mCurrentChild)
		{
			mCurrentChild->AddCommand(std::forward<Args>(aArgs)...);
		}
		else
		{
			mCommands.emplace_back(std::forward<Args>(aArgs)...);
		}
	}
}