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
		NodeView GetInputNode() const;
		NodeView GetOutputNode() const;
		NodeTypeView GetCallerNodeType() const;
		NodeTypeView GetInputNodeType() const;
		NodeTypeView GetOutputNodeType() const;
		NodeGraphView GetNodeGraph();

		FunctionID GetID() const;

		explicit operator bool() const;

		bool operator==(const FunctionView& aOther) const;

	private:

		Function& GetFunction();
		const Function& GetFunction() const;

	private:

		FunctionID mFunctionID;
	};
}