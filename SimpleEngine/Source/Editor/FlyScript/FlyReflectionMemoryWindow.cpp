#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyReflectionMemoryWindow.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include <algorithm>

namespace Editor
{
	void FlyReflectionMemoryWindow::Update()
	{

		if (ImGui::Begin("Memory Layout"))
		{
			DataTypeComboNoFilter("Data Type", myCurrentDataType);

			if (myCurrentDataType)
			{

				ImGui::Text("Size: %i", myCurrentDataType.GetSize());
				ImGui::SameLine();
				ImGui::Text("Alignment: %i", myCurrentDataType.GetAlignment());

				const size_t alignment = std::max(static_cast<size_t>(4), myCurrentDataType.GetAlignment());


				if (ImGui::BeginTable("MemoryTable", static_cast<int>(alignment)))
				{
					std::vector<Fly::VariableProxy> variables = myCurrentDataType.GetMemberVariables();


					Fly::VariableProxy currentVariable;
					Fly::GenericDataTypeProxy currentDataType;
					int currentByteLeft = 0;

					for (size_t i = 0; i < myCurrentDataType.GetSize(); i++)
					{
						if (i % alignment == 0)
						{
							ImGui::TableNextRow();
						}

						if (currentByteLeft == 0)
						{
							currentVariable = Fly::VariableProxy();
						}

						if (!currentVariable)
						{
							for (const Fly::VariableProxy& variableProxy : variables)
							{
								const size_t byteOffset = variableProxy.GetByteOffset();
								const Fly::GenericDataTypeProxy variableDataTypeProxy(variableProxy.GetDataTypeID());
								if (i >= byteOffset && i < byteOffset + variableDataTypeProxy.GetSize())
								{
									currentVariable = variableProxy;
									currentDataType = variableDataTypeProxy;
									currentByteLeft = static_cast<int>(variableDataTypeProxy.GetSize());
									break;
								}
							}

							if (!currentVariable)
							{
								currentDataType = myCurrentDataType;
							}
						}

						ImGui::TableSetColumnIndex(static_cast<int>(i % alignment));


						ImGui::Text("Hello");

						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ToImGuiColor(currentDataType.GetColor()));

						currentByteLeft--;

					}

				}


				ImGui::EndTable();
			}

		}
		ImGui::End();
	}
}