#pragma once
#include "FlyDefines.hpp"
#include <ImGui/imgui.h>
#include "DataType/FlyDataTypeRegistry.hpp"
#include "../SystemTypes/FlyFlow.hpp"
#include "../SystemTypes/FlyNone.hpp"

bool Edit(bool& aValue);
void Save(const bool& aValue, nlohmann::json& aJson);
void Load(bool& aValue, const nlohmann::json& aJson);

bool Edit(int& aValue);
void Save(const int& aValue, nlohmann::json& aJson);
void Load(int& aValue, const nlohmann::json& aJson);

bool Edit(float& aValue);
void Save(const float& aValue, nlohmann::json& aJson);
void Load(float& aValue, const nlohmann::json& aJson);

bool Edit(UINT& aValue);
void Save(const UINT& aValue, nlohmann::json& aJson);
void Load(UINT& aValue, const nlohmann::json& aJson);

bool Edit(UINT64& aValue);
void Save(const UINT64& aValue, nlohmann::json& aJson);
void Load(UINT64& aValue, const nlohmann::json& aJson);

bool Edit(char& aValue);
void Save(const char& aValue, nlohmann::json& aJson);
void Load(char& aValue, const nlohmann::json& aJson);


namespace std
{
	bool Edit(std::string& aValue);
	void Save(const std::string& aValue, nlohmann::json& aJson);
	void Load(std::string& aValue, const nlohmann::json& aJson);
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

	FLY_STRUCT(Wildcard, eNodeOperatorTrait::All, Colors::Gray, NonTargetable);
	FLY_STRUCT(Flow, eNodeOperatorTrait::None, Color(0.9f, 0.9f, 0.9f), NonTargetable);
	FLY_CLASS(None, eNodeOperatorTrait::None, Colors::Black);

	using String = std::string;

	FLY_STRUCT(bool, eNodeOperatorTrait::All, Color(1.f, 0.1f, 0.1f), NonTargetable);
	FLY_STRUCT(int, eNodeOperatorTrait::All, Color(0.f, 0.2f, 1.f), NonTargetable);
	FLY_STRUCT(float, eNodeOperatorTrait::All, Color(0.55f, 0.2f, 0.65f), NonTargetable);
	FLY_STRUCT(UINT, eNodeOperatorTrait::All, Color(0.f, 0.4f, 1.f), NonTargetable);
	FLY_STRUCT(UINT64, eNodeOperatorTrait::All, Color(0.1f, 0.5f, 1.f), NonTargetable);
	FLY_STRUCT(char, eNodeOperatorTrait::All, Color(0.2f, 0.7f, 0.4f), NonTargetable);
	FLY_STRUCT(String, eNodeOperatorTrait::All, Color(0.3f, 0.8f, 0.2f), NonTargetable);
}
