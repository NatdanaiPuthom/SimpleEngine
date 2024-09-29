#pragma once

namespace Fly
{
	class DataTypeFacade;
}

namespace Editor
{

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade);
	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade);
}
