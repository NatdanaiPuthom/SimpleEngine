#pragma once
#include "../FlyDefines.hpp"
#include "../Function/FlyFunction.hpp"
#include "FlyNodeView.hpp"
#include "FlyNodeTypeView.hpp"
#include "FlyNodeGraphView.hpp"

namespace FLY_NAMESPACE
{
	
	class FunctionView final
	{
	public:

		FunctionView();
		explicit FunctionView(FunctionID aFunctionID);

		const std::string& GetName() const;
		NodeView GetInputNodeView() const;
		NodeView GetOutputNodeView() const;
		NodeTypeView GetCallerNodeTypeView() const;
		NodeTypeView GetInputNodeTypeView() const;
		NodeTypeView GetOutputNodeTypeView() const;
		NodeGraphView GetNodeGraphView();

		FunctionID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void AddPin(DataTypeView aDataTypeView, eFlowType aFlowType, std::string_view aName, CommandTracker* aCommandTracker);
		void SetPinNameAtIndex(std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndex(DataTypeView aDataTypeView, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const FunctionView& a, const FunctionView& b);

	private:

		Function& GetFunction();
		const Function& GetFunction() const;

	private:

		FunctionID mFunctionID;
	};
}