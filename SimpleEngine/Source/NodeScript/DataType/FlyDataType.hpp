#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataTypeInterface.hpp"
#include "FlyDataTypeTrait.hpp"
#include "FlyVariableContainer.hpp"

namespace FLY_NAMESPACE
{

	class DataType final
	{
	public:

		DataType() = default;
		DataType(std::string aName, size_t aSize, size_t aAlignment, Color aColor, const std::type_info* aTypeInfo, DataTypeInterface aInterface, DataTypeID aToPointerDataTypeID, DataTypeID aToValueDataTypeID, eDataTypeTrait aTraits);

		[[nodiscard]] std::string& Name();
		[[nodiscard]] const std::string& Name() const;

		[[nodiscard]] size_t GetSize() const;
		[[nodiscard]] size_t GetAlignment() const;
		[[nodiscard]] Color GetColor() const;
		[[nodiscard]] const std::type_info* GetTypeInfo() const;
		[[nodiscard]] const DataTypeInterface& GetInterface() const;
		[[nodiscard]] VariableContainer& GetVariableContainer();
		[[nodiscard]] const VariableContainer& GetVariableContainer() const;
		[[nodiscard]] const std::vector<NodeTypeID>& GetMemberNodeTypeIDs() const;
		[[nodiscard]] NodeTypeID GetBreakerNodeTypeID() const;
		[[nodiscard]] NodeTypeID GetMakerNodeTypeID() const;
		[[nodiscard]] DataTypeID GetToPointerDataTypeID() const;
		[[nodiscard]] DataTypeID GetToValueDataTypeID() const;
		[[nodiscard]] eDataTypeTrait GetTypeTraits() const;

		void SetColor(Color aColor);
		void SetBreakerNodeTypeID(NodeTypeID aNodeTypeID);
		void SetMakerNodeTypeID(NodeTypeID aNodeTypeID);
		void AddMemberNodeTypeID(NodeTypeID aNodeTypeID);

	private:

		std::string mName;
		size_t mSize = 0;
		size_t mAlignment = 0;
		Color mColor;
		const std::type_info* mTypeInfo = nullptr;
		DataTypeInterface mInterface;
		VariableContainer mVariableContainer;
		std::vector<NodeTypeID> mMemberNodeTypeIDs;
		NodeTypeID mBreakerNodeTypeID;
		NodeTypeID mMakerNodeTypeID;
		DataTypeID mToPointerDataTypeID;
		DataTypeID mToValueDataTypeID;
		eDataTypeTrait mTypeTraits = eDataTypeTrait::None;
	};

	inline std::string& DataType::Name()
	{
		return mName;
	}

	inline const std::string& DataType::Name() const
	{
		return mName;
	}

	inline size_t DataType::GetSize() const
	{
		return mSize;
	}

	inline size_t DataType::GetAlignment() const
	{
		return mAlignment;
	}

	inline Color DataType::GetColor() const
	{
		return mColor;
	}

	inline const DataTypeInterface& DataType::GetInterface() const
	{
		return mInterface;
	}

	inline VariableContainer& DataType::GetVariableContainer()
	{
		return mVariableContainer;
	}

	inline const VariableContainer& DataType::GetVariableContainer() const
	{
		return mVariableContainer;
	}

	inline DataTypeID DataType::GetToPointerDataTypeID() const
	{
		return mToPointerDataTypeID;
	}

	inline DataTypeID DataType::GetToValueDataTypeID() const
	{
		return mToValueDataTypeID;
	}

	inline eDataTypeTrait DataType::GetTypeTraits() const
	{
		return mTypeTraits;
	}

	struct TemplateDataType
	{
		const std::string mName;
	};

	enum class eDataTypeRelation : uint8_t
	{
		None,
		Same,
		Pointer_Value,
		Value_Pointer
	};
}