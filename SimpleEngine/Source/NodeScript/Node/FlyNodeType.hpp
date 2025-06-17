#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeTrait.hpp"
#include "FlyNodeRef.hpp"
#include <vector>
#include <string>
#include <any>

namespace FLY_NAMESPACE
{

	class NodeType final
	{
	public:

		NodeType(std::string aName, CreateNodeFunction aCreateFunction, ExecuteNodeFunction aExecuteFunction, FastExecuteNodeFunction aFastExecuteFunction,
			eNodeTrait aTraits, EventID aEventID, DataTypeID aOwnerDataTypeID, std::vector<PinTypeID> aInputPinTypeIDs, std::vector<PinTypeID> aOutputPinTypeIDs,
			MemoryPoolID aFunctionMemoryID, DataTypeID aNodeStateDataTypeID, TraitID aTraitID, eNodeOperatorTrait aNodeOperatorTrait);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] CreateNodeFunction GetCreateFunction() const;
		[[nodiscard]] ExecuteNodeFunction GetExecuteFunction() const;
		[[nodiscard]] FastExecuteNodeFunction GetFastExecuteFunction() const;
		[[nodiscard]] eNodeTrait GetTraits() const;
		[[nodiscard]] EventID GetEventID() const;
		[[nodiscard]] DataTypeID GetOwnerDataTypeID() const;
		[[nodiscard]] std::vector<PinTypeID>& GetInputPinTypeIDs();
		[[nodiscard]] const std::vector<PinTypeID>& GetInputPinTypeIDs() const;
		[[nodiscard]] std::vector<PinTypeID>& GetOutputPinTypeIDs();
		[[nodiscard]] const std::vector<PinTypeID>& GetOutputPinTypeIDs() const;
		[[nodiscard]] MemoryPoolID GetFunctionMemoryID() const;
		[[nodiscard]] DataTypeID GetNodeStateDataTypeID() const;
		[[nodiscard]] TraitID GetTraitID() const;
		[[nodiscard]] std::vector<NodeRef>& GetNodeRefs();
		[[nodiscard]] const std::vector<NodeRef>& GetNodeRefs() const;
		[[nodiscard]] eNodeOperatorTrait GetOperatorTrait() const;

		void SetName(std::string aName);

	private:

		std::string mName;
		CreateNodeFunction mCreateFunction;
		ExecuteNodeFunction mExecuteFunction;
		FastExecuteNodeFunction mFastExecuteFunction;
		eNodeTrait mTraits = eNodeTrait::None;
		EventID mEventID = InvalidID<EventID>();
		DataTypeID mOwnerDataTypeID;
		std::vector<PinTypeID> mInputPinTypeIDs;
		std::vector<PinTypeID> mOutputPinTypeIDs;
		MemoryPoolID mFunctionMemoryID = InvalidID<MemoryPoolID>();
		DataTypeID mNodeStateDataTypeID;
		TraitID mTraitID;
		std::vector<NodeRef> mNodeRefs;
		eNodeOperatorTrait mOperatorTrait = eNodeOperatorTrait::None;
	};

	inline CreateNodeFunction NodeType::GetCreateFunction() const
	{
		return mCreateFunction;
	}

	inline ExecuteNodeFunction NodeType::GetExecuteFunction() const
	{
		return mExecuteFunction;
	}

	inline FastExecuteNodeFunction NodeType::GetFastExecuteFunction() const
	{
		return mFastExecuteFunction;
	}

	inline eNodeTrait NodeType::GetTraits() const
	{
		return mTraits;
	}

	inline EventID NodeType::GetEventID() const
	{
		return mEventID;
	}

	inline DataTypeID NodeType::GetOwnerDataTypeID() const
	{
		return mOwnerDataTypeID;
	}

	inline std::vector<PinTypeID>& NodeType::GetInputPinTypeIDs()
	{
		return mInputPinTypeIDs;
	}

	inline const std::vector<PinTypeID>& NodeType::GetInputPinTypeIDs() const
	{
		return mInputPinTypeIDs;
	}

	inline std::vector<PinTypeID>& NodeType::GetOutputPinTypeIDs()
	{
		return mOutputPinTypeIDs;
	}

	inline const std::vector<PinTypeID>& NodeType::GetOutputPinTypeIDs() const
	{
		return mOutputPinTypeIDs;
	}

	inline MemoryPoolID NodeType::GetFunctionMemoryID() const
	{
		return mFunctionMemoryID;
	}

	inline DataTypeID NodeType::GetNodeStateDataTypeID() const
	{
		return mNodeStateDataTypeID;
	}

	inline TraitID NodeType::GetTraitID() const
	{
		return mTraitID;
	}

	inline eNodeOperatorTrait NodeType::GetOperatorTrait() const
	{
		return mOperatorTrait;
	}

	struct NodeTypeDesc final
	{
		std::vector<std::string> mInputPinNames;
		std::vector<std::string> mOutputPinNames;
		std::vector<std::any> mDefaultValues;
		bool mShowDataTypePinNames = true;
	};
}