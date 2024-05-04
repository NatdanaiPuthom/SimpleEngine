#pragma once
#include "ScriptDefines.h"
#include <ImGui/imgui.h>
#include "DataType/DataTypeManager.h"


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

	template<SCR::HasOperator_Print T>
	std::ostream& operator<<(std::ostream& aOStream, const std::vector<T>& aData)
	{
		for (const T& value : aData)
		{
			std::cout << value << std::endl;
		}
		return aOStream;
	}

}

namespace SCR
{

	void RegisterSystemDataTypes();
}