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
					Fly::DataTypeFacade currentDataType;
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
								const Fly::DataTypeFacade variableDataTypeFacade(variableFacade.GetDataTypeID());
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
				
					/*for (const auto& variable : variables)
					{
						Fly::DataTypeFacade variableDataTypeFacade(variable.GetDataTypeID());
						const size_t size = variableDataTypeFacade.GetSize();
						const Fly::Color color = variableDataTypeFacade.GetColor();
						const size_t rows = static_cast<size_t>(std::ceilf(static_cast<float>(size) / alignment));

						for (int rowIndex = 0; rowIndex < rows; rowIndex++)
						{
							ImGui::TableNextRow();

							const size_t currentColumnCount = (rowIndex == rows - 1) && size % alignment != 0 ? size % alignment : alignment;

							for (int columnIndex = 0; columnIndex < currentColumnCount; ++columnIndex)
							{

								ImGui::TableSetColumnIndex(columnIndex);

								ImGui::Text("Hello");

								ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ToImGuiColor(color));
							}

						}

					}*/
				}


				ImGui::EndTable();
			}
			//ImGui::Columns(columns, "myColumns");

			//// Set up headers for the columns
			//ImGui::Text("Name");
			//ImGui::NextColumn();
			//ImVec4 testColor{ Fly::Colors::Purple.r, Fly::Colors::Purple.g , Fly::Colors::Purple.b , Fly::Colors::Purple.a };
			//ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, testColor);
			//ImGui::Text("Type");
			//ImGui::PopStyleColor();
			//ImGui::NextColumn();
			//ImGui::Separator();

			//// Populate the grid with data
			//for (const auto& variable : variables)
			//{
			//    ImGui::Text(variable.GetName().c_str());
			//    ImGui::NextColumn();
			//    ImGui::Text(Fly::DataTypeFacade(variable.GetDataTypeID()).GetName().c_str());
			//    ImGui::NextColumn();
			//}

			//// End the columns
			//ImGui::Columns(1);

			ImGui::End();
		}
	}
}