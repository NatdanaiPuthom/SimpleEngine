#include "SystemDataTypes.h"
#include "DataType/DataTypeRegistry.h"
#include "SystemTypes/ScriptWildcard.h"
#include "SystemTypes/ScriptFlow.h"
#include <ImGui/imgui.h>
#include <string>


// Bool
static bool Edit(bool& aValue)
{
	bool prev = aValue;
	ImGui::Checkbox("##", &aValue);

	return prev != aValue;
}


static void Save(nlohmann::json& aJson, const bool& aValue)
{
	aJson["Value"] = aValue;
}


static void Load(const nlohmann::json& aJson, bool& aValue)
{
	aValue = aJson["Value"];
}

// Int
static bool Edit(int& aValue)
{
	return ImGui::DragInt("##", &aValue);
}

static void Save(nlohmann::json& aJson, const int& aValue)
{
	aJson["Value"] = aValue;
}

static void Load(const nlohmann::json& aJson, int& aValue)
{
	aValue = aJson["Value"];
}

// Float
static bool Edit(float& aValue)
{
	return ImGui::DragFloat("##", &aValue);
}

static void Save(nlohmann::json& aJson, const float& aValue)
{
	aJson["Value"] = aValue;
}


static void Load(const nlohmann::json& aJson, float& aValue)
{
	aValue = aJson["Value"];
}

// Unsigned Int
static bool Edit(unsigned int& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<unsigned int>(intValue);
		return true;
	}
	return false;
}

static void Save(nlohmann::json& aJson, const unsigned int& aValue)
{
	aJson["Value"] = aValue;
}

static void Load(const nlohmann::json& aJson, unsigned int& aValue)
{
	aValue = aJson["Value"];
}

// Unsigned Long Long
static bool Edit(unsigned long long& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, INT_MAX))
	{
		aValue = static_cast<unsigned long long>(intValue);
		return true;
	}
	return false;
}

static void Save(nlohmann::json& aJson, const unsigned long long& aValue)
{
	aJson["Value"] = aValue;
}

static void Load(const nlohmann::json& aJson, unsigned long long& aValue)
{
	aValue = aJson["Value"];
}

// Char
static bool Edit(char& aValue)
{
	char c[2] = { aValue, '\0' };

	if (ImGui::InputText("##", c, 2))
	{
		aValue = c[0];
		return true;
	}
	return false;
}

static void Save(nlohmann::json& aJson, const char& aValue)
{
	int i = aValue;
	aJson["char"] = i;
}

static void Load(const nlohmann::json& aJson, char& aValue)
{
	int i = aJson["char"];
	aValue = (char)i;
}

namespace std
{

	static bool Edit(std::string& aValue)
	{
		char buffer[32]{};
		strcpy_s(buffer, aValue.c_str());

		if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
		{
			aValue = buffer;
			return true;
		}
		return false;

	}

	static void Save(nlohmann::json& aJson, const std::string& aValue)
	{
		aJson["Value"] = aValue;
	}

	static void Load(const nlohmann::json& aJson, std::string& aValue)
	{
		aValue = aJson["Value"];
	}
}


namespace SCR
{

	/*constexpr bool s = SCRIPT::Fundamental<UINT64>;
	constexpr bool a = SCRIPT::Editable<bool>;*/

	using String = std::string;

	FLY_DATATYPE(Wildcard, eNodeOperatorTrait::All, Color(0.f, 0.f, 0.f));
	FLY_DATATYPE(Flow, eNodeOperatorTrait::None, Color(0.9f, 0.9f, 0.9f));

	FLY_DATATYPE(bool, eNodeOperatorTrait::All, Color(1.f, 0.1f, 0.1f));
	FLY_DATATYPE(int, eNodeOperatorTrait::All, Color(0.f, 0.2f, 1.f));
	FLY_DATATYPE(float, eNodeOperatorTrait::All, Color(0.55f, 0.2f, 0.65f));
	FLY_DATATYPE(UINT, eNodeOperatorTrait::All, Color(0.f, 0.4f, 1.f));
	FLY_DATATYPE(UINT64, eNodeOperatorTrait::All, Color(0.1f, 0.5f, 1.f));
	FLY_DATATYPE(char, eNodeOperatorTrait::All, Color(0.2f, 0.7f, 0.4f));
	FLY_DATATYPE(String, eNodeOperatorTrait::All, Color(0.3f, 0.8f, 0.2f));

	//constexpr bool a = SCRIPT::Scriptable<String, nlohmann::json>;

	void Do()
	{

	}
	/*	void RegisterSystemDataTypes()
	{
		DataTypeRegistry::RegisterTemplateType<std::vector>("Array");

		DataTypeRegistry::RegisterNonSerializableType<Wildcard>("Wildcard", Color(0.f, 0.f, 0.f));
		DataTypeRegistry::RegisterNonSerializableType<Flow>("Flow", Color(0.9f, 0.9f, 0.9f));

		DataTypeRegistry::Register<bool>("Bool", Color(1.f, 0.1f, 0.1f));
		DataTypeRegistry::Register<int>("Int", Color(0.f, 0.2f, 1.f));
		DataTypeRegistry::Register<float, eNodeOperatorTrait::All, std::vector>("Float", Color(0.55f, 0.2f, 0.65f));
		DataTypeRegistry::Register<double, eNodeOperatorTrait::All, std::vector>("Double", Color(0.57f, 0.4f, 0.9f));
		DataTypeRegistry::Register<unsigned int, eNodeOperatorTrait::All, std::vector>("Unsigned Int", Color(0.f, 0.4f, 1.f));
		DataTypeRegistry::Register<unsigned long long, eNodeOperatorTrait::All, std::vector>("Unsigned Long Long", Color(0.1f, 0.5f, 1.f));
		DataTypeRegistry::Register<char, eNodeOperatorTrait::All, std::vector>("Char", Color(0.2f, 0.7f, 0.4f));
		DataTypeRegistry::Register<std::string>("String", Color(0.3f, 0.8f, 0.2f));
	}*/
}
