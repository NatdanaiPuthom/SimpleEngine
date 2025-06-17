#pragma once
#include "../FlyDefines.hpp"
#include "../Function/FlyFunction.hpp"
#include "FlyNodeProxy.hpp"
#include "FlyNodeTypeProxy.hpp"
#include "FlyNodeGraphProxy.hpp"

namespace FLY_NAMESPACE
{
	
	class FunctionProxy final
	{
	public:

		FunctionProxy() = default;
		explicit FunctionProxy(FunctionID aFunctionID);

		const std::string& GetName() const;
		NodeProxy GetInputNode() const;
		NodeProxy GetOutputNode() const;
		NodeTypeProxy GetCallerNodeType() const;
		NodeTypeProxy GetInputNodeType() const;
		NodeTypeProxy GetOutputNodeType() const;
		NodeGraphProxy GetNodeGraph();

		FunctionID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void AddPin(GenericDataTypeProxy aDataType, eIODirection aIODirection, std::string_view aName, CommandTracker* aCommandTracker);
		void SetPinNameAtIndex(std::string_view aName, size_t aIndex, eIODirection aIODirection, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndex(GenericDataTypeProxy aDataType, size_t aIndex, eIODirection aIODirection, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const FunctionProxy& a, const FunctionProxy& b);

	private:

		Function& GetFunction();
		const Function& GetFunction() const;

	private:

		FunctionID mFunctionID = InvalidID<FunctionID>();
	};
}