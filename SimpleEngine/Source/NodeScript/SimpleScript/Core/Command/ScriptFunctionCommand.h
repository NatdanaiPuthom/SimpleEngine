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
		using FunctionType = std::function<void(Script&)>;
	public:

		FunctionCommand(const FunctionType& aDoFunction, const FunctionType& aUndoFunction, const std::string& aName = "FunctionCommand");
		~FunctionCommand();

	private:

		void Do(Script& aScript) override;
		void Undo(Script& aScript) override;

	private:

		const FunctionType myDoFunction;
		const FunctionType myUndoFunction;

	};

	inline FunctionCommand<void>::FunctionCommand(const FunctionType& aDoFunction, const FunctionType& aUndoFunction, const std::string& aName)
		: Command(aName)
		, myDoFunction(aDoFunction)
		, myUndoFunction(aUndoFunction)
	{
	}

	inline FunctionCommand<void>::~FunctionCommand()
	{
	}

	inline void FunctionCommand<void>::Do(Script& aScript)
	{
		myDoFunction(aScript);
	}

	inline void FunctionCommand<void>::Undo(Script& aScript)
	{
		myUndoFunction(aScript);
	}
	
	template<typename DataType>
	class FunctionCommand final : public Command
	{
		using FunctionType = FuncPtr<void, const DataType&, Script&>;
	public:

		FunctionCommand(const DataType& aData, FunctionType aDoFunction, FunctionType aUndoFunction, const std::string& aName = "FunctionCommand");
		~FunctionCommand();

	private:

		void Do(Script& aScript) override;
		void Undo(Script& aScript) override;

	private:

		const DataType myData;
		const FunctionType myDoFunction;
		const FunctionType myUndoFunction;

	};

	template<typename DataType>
	inline FunctionCommand<DataType>::FunctionCommand(const DataType& aData, FunctionType aDoFunction, FunctionType aUndoFunction, const std::string& aName)
		: Command(aName)
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
	inline void FunctionCommand<DataType>::Do(Script& aScript)
	{
		myDoFunction(myData, aScript);
	}

	template<typename Data>
	inline void FunctionCommand<Data>::Undo(Script& aScript)
	{
		myUndoFunction(myData, aScript);
	}
}