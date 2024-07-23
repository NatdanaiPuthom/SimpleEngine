#pragma once
#include "ScriptDefines.hpp"
#include <ImGui/imgui.h>
#include "DataType/DataTypeRegistry.hpp"
#include "../SystemTypes/FlyFlow.hpp"
#include "../SystemTypes/FlyNone.hpp"

bool Edit(bool& aValue);
void Save(nlohmann::json& aJson, const bool& aValue);
void Load(const nlohmann::json& aJson, bool& aValue);

bool Edit(int& aValue);
void Save(nlohmann::json& aJson, const int& aValue);
void Load(const nlohmann::json& aJson, int& aValue);

bool Edit(float& aValue);
void Save(nlohmann::json& aJson, const float& aValue);
void Load(const nlohmann::json& aJson, float& aValue);

bool Edit(UINT& aValue);
void Save(nlohmann::json& aJson, const UINT& aValue);
void Load(const nlohmann::json& aJson, UINT& aValue);

bool Edit(UINT64& aValue);
void Save(nlohmann::json& aJson, const UINT64& aValue);
void Load(const nlohmann::json& aJson, UINT64& aValue);

bool Edit(char& aValue);
void Save(nlohmann::json& aJson, const char& aValue);
void Load(const nlohmann::json& aJson, char& aValue);


namespace std
{
	bool Edit(std::string& aValue);
	void Save(nlohmann::json& aJson, const std::string& aValue);
	void Load(const nlohmann::json& aJson, std::string& aValue);
}

template<typename T>
static bool EditVectorImpl(std::vector<T>& aData, bool(*anEditFunc)(T&))
{
	bool changed = false;
	for (size_t i = 0; i < aData.size(); ++i)
	{
		ImGui::PushID((std::string(typeid(std::vector<T>).name()) + std::to_string(i)).c_str());

		if (anEditFunc(aData[i]))
		{
			changed = true;
		}
		ImGui::PopID();
	}

	if (ImGui::Button("Add"))
	{
		aData.emplace_back();
		changed = true;
	}
	ImGui::SameLine();
	ImGui::BeginDisabled(aData.empty());
	if (ImGui::Button("Delete"))
	{
		if (!aData.empty())
		{
			aData.erase(aData.end() - 1);
			changed = true;
		}
	}
	ImGui::EndDisabled();
	return changed;
}

template<typename T>
static void SaveVectorImpl(nlohmann::json& aJson, const std::vector<T>& aData, void(*aSaveFunc)(nlohmann::json&, const T&))
{
	nlohmann::json arrayJson = nlohmann::json::array();
	for (const T& data : aData)
	{
		nlohmann::json element;
		aSaveFunc(element, data);
		arrayJson.push_back(element);
	}
	aJson["Array"] = arrayJson;
}

template<typename T>
static void LoadVectorImpl(const nlohmann::json& aJson, std::vector<T>& aData, void(*aLoadFunc)(const nlohmann::json&, T&))
{
	const nlohmann::json& arrayJson = aJson["Array"];
	for (const nlohmann::json& element : arrayJson)
	{
		T& data = aData.emplace_back();
		aLoadFunc(element, data);
	}
}

namespace std
{

	template<typename T>
	static bool EditTemplate(std::vector<T>& aData)
	{
		bool changed = false;
		for (size_t i = 0; i < aData.size(); ++i)
		{
			ImGui::PushID((std::string(typeid(std::vector<T>).name()) + std::to_string(i)).c_str());

			bool(*editFunc)(T&) = ::Edit;
			if (editFunc(aData[i]))
			{
				changed = true;
			}
			ImGui::PopID();
		}

		if (ImGui::Button("Add"))
		{
			aData.emplace_back();
			changed = true;
		}
		ImGui::SameLine();
		ImGui::BeginDisabled(aData.empty());
		if (ImGui::Button("Delete"))
		{
			if (!aData.empty())
			{
				aData.erase(aData.end() - 1);
				changed = true;
			}
		}
		ImGui::EndDisabled();
		return changed;
	}

	template<typename T>
	static void SaveTemplate(nlohmann::json& aJson, const std::vector<T>& aData)
	{
		nlohmann::json arrayJson = nlohmann::json::array();
		for (const T& data : aData)
		{
			nlohmann::json element;
			::Save(element, data);
			arrayJson.push_back(element);
		}
		aJson["Array"] = arrayJson;
	}

	template<typename T>
	static void LoadTemplate(const nlohmann::json& aJson, std::vector<T>& aData)
	{
		const nlohmann::json& arrayJson = aJson["Array"];
		for (const nlohmann::json& element : arrayJson)
		{
			T& data = aData.emplace_back();
			::Load(element, data);
		}
	}

	template<FLY_NAMESPACE::HasOperator_Print T>
	std::ostream& operator<<(std::ostream& aOStream, const std::vector<T>& aData)
	{
		for (const T& value : aData)
		{
			std::cout << value << std::endl;
		}
		return aOStream;
	}

}

namespace FLY_NAMESPACE
{

	FLY_DATATYPE(Wildcard, eNodeOperatorTrait::All, Color(0.75f, 0.75f, 0.75f), NonTargetable);
	FLY_DATATYPE(Flow, eNodeOperatorTrait::None, Color(0.9f, 0.9f, 0.9f), NonTargetable);
	FLY_DATATYPE(None, eNodeOperatorTrait::None, Color());

	using String = std::string;


	FLY_DATATYPE(bool, eNodeOperatorTrait::All, Color(1.f, 0.1f, 0.1f), NonTargetable);
	FLY_DATATYPE(int, eNodeOperatorTrait::All, Color(0.f, 0.2f, 1.f), NonTargetable);
	FLY_DATATYPE(float, eNodeOperatorTrait::All, Color(0.55f, 0.2f, 0.65f), NonTargetable);
	FLY_DATATYPE(UINT, eNodeOperatorTrait::All, Color(0.f, 0.4f, 1.f), NonTargetable);
	FLY_DATATYPE(UINT64, eNodeOperatorTrait::All, Color(0.1f, 0.5f, 1.f), NonTargetable);
	FLY_DATATYPE(char, eNodeOperatorTrait::All, Color(0.2f, 0.7f, 0.4f), NonTargetable);
	FLY_DATATYPE(String, eNodeOperatorTrait::All, Color(0.3f, 0.8f, 0.2f), NonTargetable);

	void Test123();
}
