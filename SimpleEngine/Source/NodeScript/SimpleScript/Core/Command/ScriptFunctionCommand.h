#pragma once
#include "ScriptCommand.h"

#include <functional>

namespace SCR
{
	template<typename>
	class FunctionCommand;

	template<>
	class FunctionCommand<void> final : public Command
	{
		using FunctionType = std::function<void(const CommandContext&)>;
	public:

		FunctionCommand(const CommandContext& aContext, const FunctionType& aDoFunction, const FunctionType& aUndoFunction, const std::string& aName = "FunctionCommand");
		~FunctionCommand();

	private:

		void Do() override;
		void Undo() override;

	private:

		const FunctionType myDoFunction;
		const FunctionType myUndoFunction;

	};

	inline FunctionCommand<void>::FunctionCommand(const CommandContext& aContext, const FunctionType& aDoFunction, const FunctionType& aUndoFunction, const std::string& aName)
		: Command(aContext, aName)
		, myDoFunction(aDoFunction)
		, myUndoFunction(aUndoFunction)
	{
	}

	inline FunctionCommand<void>::~FunctionCommand()
	{
	}

	inline void FunctionCommand<void>::Do()
	{
		myDoFunction(myContext);
	}

	inline void FunctionCommand<void>::Undo()
	{
		myUndoFunction(myContext);
	}
	
	template<typename DataType>
	class FunctionCommand final : public Command
	{
		using FunctionType = FuncPtr<void, const DataType&, const CommandContext&>;
	public:

		FunctionCommand(const CommandContext& aContext, const DataType& aData, FunctionType aDoFunction, FunctionType aUndoFunction, const std::string& aName = "FunctionCommand");
		~FunctionCommand();

	private:

		void Do() override;
		void Undo() override;

	private:

		const DataType myData;
		const FunctionType myDoFunction;
		const FunctionType myUndoFunction;


	};

	template<typename DataType>
	inline FunctionCommand<DataType>::FunctionCommand(const CommandContext& aContext, const DataType& aData, FunctionType aDoFunction, FunctionType aUndoFunction, const std::string& aName)
		: Command(aContext, aName)
		, myData(aData)
		, myDoFunction(aDoFunction)
		, myUndoFunction(aUndoFunction)
	{
	}

	template<typename DataType>
	inline FunctionCommand<DataType>::~FunctionCommand()
	{
	}

	template<typename DataType>
	inline void FunctionCommand<DataType>::Do()
	{
		myDoFunction(myData, myContext);
	}

	template<typename Data>
	inline void FunctionCommand<Data>::Undo()
	{
		myUndoFunction(myData, myContext);
	}
}