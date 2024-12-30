#pragma once
#include "NodeScript/FlyRegistration.hpp"
#include "NodeScriptCustomTypes.hpp"
#include <ImGui/imgui.h>
#include <nlohmann/json.hpp>

using uint32 = unsigned int;
using uint64 = unsigned long long;

Fly::ViewAndEditResult ViewAndEdit(bool& aValue);
void View(const bool& aValue);
void Save(const bool& aValue, nlohmann::json& aJson);
void Load(bool& aValue, const nlohmann::json& aJson);

Fly::ViewAndEditResult ViewAndEdit(int& aValue);
void View(const int& aValue);
void Save(const int& aValue, nlohmann::json& aJson);
void Load(int& aValue, const nlohmann::json& aJson);

Fly::ViewAndEditResult ViewAndEdit(float& aValue);
void View(const float& aValue);
void Save(const float& aValue, nlohmann::json& aJson);
void Load(float& aValue, const nlohmann::json& aJson);

Fly::ViewAndEditResult ViewAndEdit(uint32& aValue);
void View(const uint32& aValue);
void Save(const uint32& aValue, nlohmann::json& aJson);
void Load(uint32& aValue, const nlohmann::json& aJson);

Fly::ViewAndEditResult ViewAndEdit(uint64& aValue);
void View(const uint64& aValue);
void Save(const uint64& aValue, nlohmann::json& aJson);
void Load(uint64& aValue, const nlohmann::json& aJson);

Fly::ViewAndEditResult ViewAndEdit(char& aValue);
void View(const char& aValue);
void Save(const char& aValue, nlohmann::json& aJson);
void Load(char& aValue, const nlohmann::json& aJson);


namespace std
{
	Fly::ViewAndEditResult ViewAndEdit(std::string& aValue);
	void View(const std::string& aValue);
	void Save(const std::string& aValue, nlohmann::json& aJson);
	void Load(std::string& aValue, const nlohmann::json& aJson);
}

namespace FLY_NAMESPACE
{
	using String = std::string;

	FLY_VALUETYPE(bool, Color(1.f, 0.1f, 0.1f));
	FLY_VALUETYPE(int, Color(0.f, 0.2f, 1.f));
	FLY_VALUETYPE(float, Color(0.55f, 0.2f, 0.65f));
	FLY_VALUETYPE(uint32, Color(0.f, 0.4f, 1.f));
	FLY_VALUETYPE(uint64, Color(0.1f, 0.5f, 1.f));
	FLY_VALUETYPE(char, Color(0.2f, 0.7f, 0.4f));
	FLY_VALUETYPE(String, Color(0.3f, 0.8f, 0.2f));
}
