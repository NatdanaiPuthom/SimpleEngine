#pragma once
#include <vector>
#include <ImGui/imgui.h>

namespace FLY_NAMESPACE
{


	struct FlyCustomVectorType
	{
		template<typename T>
		using type = std::vector<T>;
	};
}

Fly::ViewAndEditResult ViewAndEditDataPtr(Fly::DataTypeID aDataTypeID, void* aDataPtr);
void SaveDataPtr(Fly::DataTypeID aDataTypeID, const void* aDataPtr, nlohmann::json& aJson);
void LoadDataPtr(Fly::DataTypeID aDataTypeID, void* aDataPtr, const nlohmann::json& aJson);


// Check if a type has `value_type` and `begin/end` member functions
template <typename T>
concept Container = requires(T a) 
{
	typename T::value_type;
	{ a.begin() };
	{ a.end() };
};

namespace std
{

	template<Container T>
	Fly::ViewAndEditResult ViewAndEdit(T& aContainer)
	{
		using ValueType = typename T::value_type;
		static size_t currentPopupIndex = 0;
		static constexpr const char* VectorElementPopupStrID = "Vector Element Popup";
		Fly::ViewAndEditResult result;

		for (auto it = begin(aContainer); it != end(aContainer); ++it)
		{
			ValueType& data = *it;
			ImGui::PushID(&data);

			result.mIsItemActive |= ViewAndEditDataPtr(Fly::GetDataTypeID<ValueType>(), &data).mIsItemActive;


			ImGui::SameLine();

			const bool vectorElementBtnWasPressed = ImGui::Button("...");
			ImGui::PopID();

			if (vectorElementBtnWasPressed)
			{
				ImGui::OpenPopup(VectorElementPopupStrID);
				currentPopupIndex = std::distance(begin(aContainer), it);
			}
		}

		if (ImGui::BeginPopup(VectorElementPopupStrID))
		{
			if (ImGui::MenuItem("Delete"))
			{
				aContainer.erase(begin(aContainer) + currentPopupIndex);
			}

			if (ImGui::MenuItem("Insert Before"))
			{
				aContainer.insert(begin(aContainer) + currentPopupIndex, ValueType());
			}

			if (ImGui::MenuItem("Reset Value"))
			{
				aContainer[currentPopupIndex] = ValueType();
			}

			result.mIsItemActive = true;

			ImGui::EndPopup();
		}

		if (ImGui::Button("Add"))
		{
			aContainer.emplace_back();

			result.mIsItemActive |= ImGui::IsItemActive();
		}

		return result;
	}

	/*template<typename T>
	Fly::ViewAndEditResult ViewAndEdit(std::vector<T>& aVector)
	{
		static size_t currentPopupIndex = 0;
		static constexpr const char* VectorElementPopupStrID = "Vector Element Popup";
		Fly::ViewAndEditResult result;
		for (size_t i = 0; i < aVector.size(); ++i)
		{
			ImGui::PushID((std::string(typeid(std::vector<T>).name()) + std::to_string(i)).c_str());

			T& data = aVector[i];
			result.mIsItemActive |= ViewAndEditDataPtr(Fly::GetDataTypeID<T>(), &data).mIsItemActive;

			ImGui::PopID();

			ImGui::SameLine();

			ImGui::PushID(&data);
			const bool vectorElementBtnWasPressed = ImGui::Button("...");
			ImGui::PopID();

			if (vectorElementBtnWasPressed)
			{
				ImGui::OpenPopup(VectorElementPopupStrID);
				currentPopupIndex = i;
			}

		}

		if (ImGui::BeginPopup(VectorElementPopupStrID, ImGuiWindowFlags_ChildWindow))
		{
			if (ImGui::MenuItem("Delete"))
			{
				aVector.erase(begin(aVector) + currentPopupIndex);
			}

			if (ImGui::MenuItem("Insert Before"))
			{
				aVector.insert(begin(aVector) + currentPopupIndex, T());
			}

			if (ImGui::MenuItem("Reset Value"))
			{
				aVector[currentPopupIndex] = T();
			}

			result.mIsItemActive = true;

			ImGui::EndPopup();
		}

		if (ImGui::Button("Add"))
		{
			aVector.emplace_back();

			result.mIsItemActive |= ImGui::IsItemActive();
		}

		return result;
	}*/

	template<typename T>
	void Save(const std::vector<T>& aVector, nlohmann::json& aJson)
	{
		nlohmann::json arrayJson = nlohmann::json::array();
		for (const T& data : aVector)
		{
			nlohmann::json element;

			SaveDataPtr(Fly::GetDataTypeID<T>(), &data, element);

			arrayJson.push_back(element);
		}
		aJson["Values"] = arrayJson;
	}

	template<typename T>
	void Load(std::vector<T>& aVector, const nlohmann::json& aJson)
	{
		const nlohmann::json& arrayJson = aJson["Values"];
		for (const nlohmann::json& element : arrayJson)
		{
			T& data = aVector.emplace_back();

			LoadDataPtr(Fly::GetDataTypeID<T>(), &data, element);
		}
	}

}