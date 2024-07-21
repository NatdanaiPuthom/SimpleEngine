#pragma once

namespace Fly
{
	class DataTypeView;
}

namespace Editor
{

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::DataTypeView& aDataTypeView);
	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::DataTypeView& aDataTypeView);
}
