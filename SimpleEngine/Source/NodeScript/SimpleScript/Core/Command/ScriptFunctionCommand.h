//#pragma once
//#include "ScriptCommand.h"
//
//#include <functional>
//
//namespace SCR
//{
//	template<typename>
//	class FunctionCommand;
//
//	template<>
//	class FunctionCommand<void> final : public Command
//	{
//		using FunctionType = std::function<void()>;
//	public:
//
//		FunctionCommand(const FunctionType& aDoFunction, const FunctionType& aUndoFunction, const std::string& aName = "FunctionCommand");
//		~FunctionCommand();
//
//	private:
//
//		void Do() override;
//		void Undo() override;
//
//	private:
//
//		const FunctionType myDoFunction;
//		const FunctionType myUndoFunction;
//
//	};
//
//	inline FunctionCommand<void>::FunctionCommand(const FunctionType& aDoFunction, const FunctionType& aUndoFunction, const std::string& aName)
//		: Command(aName)
//		, myDoFunction(aDoFunction)
//		, myUndoFunction(aUndoFunction)
//	{
//	}
//
//	inline FunctionCommand<void>::~FunctionCommand()
//	{
//	}
//
//	inline void FunctionCommand<void>::Do()
//	{
//		myDoFunction();
//	}
//
//	inline void FunctionCommand<void>::Undo()
//	{
//		myUndoFunction();
//	}
//	
//	template<typename DataType>
//	class FunctionCommand final : public Command
//	{
//		using FunctionType = FuncPtr<void, const DataType&>;
//	public:
//
//		FunctionCommand(const DataType& aData, FunctionType aDoFunction, FunctionType aUndoFunction, const std::string& aName = "FunctionCommand");
//		~FunctionCommand();
//
//	private:
//
//		void Do() override;
//		void Undo() override;
//
//	private:
//
//		const DataType myData;
//		const FunctionType myDoFunction;
//		const FunctionType myUndoFunction;
//
//
//	};
//
//	template<typename DataType>
//	inline FunctionCommand<DataType>::FunctionCommand(const DataType& aData, FunctionType aDoFunction, FunctionType aUndoFunction, const std::string& aName)
//		: Command(aName)
//		, myData(aData)
//		, myDoFunction(aDoFunction)
//		, myUndoFunction(aUndoFunction)
//	{
//	}
//
//	template<typename DataType>
//	inline FunctionCommand<DataType>::~FunctionCommand()
//	{
//	}
//
//	template<typename DataType>
//	inline void FunctionCommand<DataType>::Do()
//	{
//		myDoFunction(myData);
//	}
//
//	template<typename Data>
//	inline void FunctionCommand<Data>::Undo()
//	{
//		myUndoFunction(myData);
//	}
//}