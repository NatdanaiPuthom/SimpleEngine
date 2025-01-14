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
					std::vector<Fly::VariableFacade> variables = myCurrentDataType.GetMemberVariables();


					Fly::VariableFacade currentVariable;
					Fly::GenericDataTypeFacade currentDataType;
					int currentByteLeft = 0;

					for (size_t i = 0; i < myCurrentDataType.GetSize(); i++)
					{
						if (i % alignment == 0)
						{
							ImGui::TableNextRow();
						}

						if (currentByteLeft == 0)
						{
							currentVariable = Fly::VariableFacade();
						}

						if (!currentVariable)
						{
							for (const Fly::VariableFacade& variableFacade : variables)
							{
								const size_t byteOffset = variableFacade.GetByteOffset();
								const Fly::GenericDataTypeFacade variableDataTypeFacade(variableFacade.GetDataTypeID());
								if (i >= byteOffset && i < byteOffset + variableDataTypeFacade.GetSize())
								{
									currentVariable = variableFacade;
									currentDataType = variableDataTypeFacade;
									currentByteLeft = static_cast<int>(variableDataTypeFacade.GetSize());
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