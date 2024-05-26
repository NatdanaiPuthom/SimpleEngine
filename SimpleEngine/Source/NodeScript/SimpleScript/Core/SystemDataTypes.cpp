#include "SystemDataTypes.h"
#include "DataType/DataTypeRegistry.h"
#include "SystemTypes/Undefined.h"
#include "SystemTypes/Flow.h"
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

// Double
static bool Edit(double& aValue)
{
	float floatValue = static_cast<float>(aValue);

	if (ImGui::DragFloat("##", &floatValue, 0.01f, -FLT_MAX, FLT_MAX))
	{
		aValue = static_cast<double>(floatValue);
		return true;
	}
	return false;
}

static void Save(nlohmann::json& aJson, const double& aValue)
{
	aJson["Value"] = aValue;
}


static void Load(const nlohmann::json& aJson, double& aValue)
{
	aValue = aJson["Value"];
}

// Short
static bool Edit(short& aValue)
{
	int intValue = static_cast<int>(aValue);
	if (ImGui::DragInt("##", &intValue, 1.f, 0, static_cast<int>(std::numeric_limits<short>::max())))
	{
		aValue = static_cast<short>(intValue);
		return true;
	}
	return false;
}

static void Save(nlohmann::json& aJson, const short& aValue)
{
	aJson["Value"] = aValue;
}

static void Load(const nlohmann::json& aJson, short& aValue)
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
		char buffer[35]{};
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

	/*struct EmilComponent
	{
		int a;
		bool b;
		float c;
	};

	static EmilComponent EmilNode(EmilComponent a)
	{
		return a;
	}*/


	void RegisterSystemDataTypes()
	{
		DataTypeRegistry::RegisterTemplateType<std::vector>("Array");

		DataTypeRegistry::RegisterNonSerializableType<Undefined>("Undefined", ScriptColor(0.f, 0.f, 0.f));
		DataTypeRegistry::RegisterNonSerializableType<Flow>("Flow", ScriptColor(0.9f, 0.9f, 0.9f));

		DataTypeRegistry::Register<bool>("Bool", ScriptColor(1.f, 0.1f, 0.1f));
		DataTypeRegistry::Register<int>("Int", ScriptColor(0.f, 0.2f, 1.f));
		DataTypeRegistry::Register<float, eNodeOperatorTrait::All, std::vector>("Float", ScriptColor(0.55f, 0.2f, 0.65f));
		DataTypeRegistry::Register<double, eNodeOperatorTrait::All, std::vector>("Double", ScriptColor(0.57f, 0.4f, 0.9f));
		DataTypeRegistry::Register<short, eNodeOperatorTrait::All, std::vector>("Short", ScriptColor(0.f, 0.1f, 1.f));
		DataTypeRegistry::Register<unsigned int, eNodeOperatorTrait::All, std::vector>("Unsigned Int", ScriptColor(0.f, 0.4f, 1.f));
		DataTypeRegistry::Register<unsigned long long, eNodeOperatorTrait::All, std::vector>("Unsigned Long Long", ScriptColor(0.1f, 0.5f, 1.f));
		DataTypeRegistry::Register<char, eNodeOperatorTrait::All, std::vector>("Char", ScriptColor(0.2f, 0.7f, 0.4f));
		DataTypeRegistry::Register<std::string>("String", ScriptColor(0.3f, 0.8f, 0.2f));

		/*NodeTypeRegistry::RegisterNodeType(EmilNode, "Test/Emil");

		DataTypeRegistry::RegisterNonSerializableType<EmilComponent>("Emil");

		DataTypeRegistry::RegisterProperty(&EmilComponent::a, "A");
		DataTypeRegistry::RegisterProperty(&EmilComponent::b, "B");
		DataTypeRegistry::RegisterProperty(&EmilComponent::c, "C");*/
	}
}


