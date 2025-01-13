#include "Editor/Precomplied/EditorPch.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/Fly.hpp"
#include "NodeScript/Command/FlyCommandTracker.hpp"
#include "NodeScript/NodeTypes/ExecutionNodes.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "EditorNodeRegistration.hpp"

#include <imnodes/imnodes_internal.h>

namespace Editor
{

	NodeScriptingWindow::NodeScriptingWindow()
		: myClassWindow(*this)
		, myCustomEventWindow(*this)
		, myFunctionSettingsWindow(*this)
		, myStructCreatorWindow(this)
	{
		myCommandTracker = std::make_unique<Fly::CommandTracker>();

		Fly::SetEditorTextFunction([](const std::string& aText) { ImGui::TextWrapped(aText.c_str()); });
	}

	NodeScriptingWindow::~NodeScriptingWindow()
	{
	}

	NodeGraphContext& NodeScriptingWindow::GetNodeContext()
	{
		return *myNodeContextHistory.history[myNodeContextHistory.currentIndex];
	}

	const NodeGraphContext& NodeScriptingWindow::GetNodeContext() const
	{
		return *myNodeContextHistory.history[myNodeContextHistory.currentIndex];
	}



	void NodeScriptingWindow::SetNodeContext(const Fly::NodeGraphFacade aNodeGraphFacade, Fly::ClassFacade aClassFacade)
	{
		assert(aNodeGraphFacade);
		assert(aClassFacade);

		std::unique_ptr<NodeGraphContext> nodeContext = std::make_unique<NodeGraphContext>();

		nodeContext->myClassFacade = aClassFacade;
		nodeContext->myNodeGraphFacade = aNodeGraphFacade;
		nodeContext->myClassInstanceFacade = aClassFacade.CreateClassInstance();

		ImNodes::SetCurrentContext(nodeContext->myImNodesContext);
		myNodeContextHistory.history.push_back(std::move(nodeContext));
		myNodeContextHistory.currentIndex++;

	}

	eGraphMode NodeScriptingWindow::GetCurrentMode() const
	{
		return GetNodeContext().myClassFacade ? eGraphMode::Class : eGraphMode::Global;
	}

	void NodeScriptingWindow::SetSelectedFunctionFacade(Fly::FunctionFacade aFunctionFacade)
	{
		mySelectedFunctionFacade = aFunctionFacade;
	}

	bool NodeScriptingWindow::OpenClassByName(std::string_view aName)
	{
		if (Fly::ClassFacade classFacade = Fly::FindClassByName(aName))
		{
			SetNodeContext(classFacade.GetEventGraphFacade(), classFacade);
			return true;
		}

		if (Fly::StructFacade structFacade = Fly::FindStructByName(aName))
		{
			myStructCreatorWindow.SetStructFacade(structFacade);
			return true;
		}

		return false;
	}

	void NodeScriptingWindow::UpdateContext()
	{
		if (myNodeContextHistory.currentIndex == -1)
		{
			auto classes = Fly::GetClasses();
			if (classes.empty())
			{
				Fly::CreateClassWithoutTarget("Default Class", ASSET_FILE_PATH);
			}

			classes = Fly::GetClasses();
			if (classes.begin()->second.empty())
			{
				Fly::CreateClass(Fly::GenericDataTypeFacade(Fly::GetClasses().begin()->first), "Default Class", ASSET_FILE_PATH);
			}

			Fly::ClassFacade flyClass = classes.begin()->second.front();
			SetNodeContext(flyClass.GetEventGraphFacade(), flyClass);
		}


		ImNodes::SetCurrentContext((*myNodeContextHistory.history.back()).myImNodesContext);

		ImNodesStyle& style = ImNodes::GetStyle();

		style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f , 0.6f , 1.f });
		style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f , 0.7f , 1.f });
		style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f , 0.8f , 1.f });

		style.Colors[ImNodesCol_NodeBackground] = ToImGuiColor(Fly::Color{ 0.02f, 0.02f, 0.02f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundHovered] = ToImGuiColor(Fly::Color{ 0.015f, 0.015f, 0.015f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundSelected] = ToImGuiColor(Fly::Color{ 0.01f, 0.01f, 0.01f, 1.f });

		style.Colors[ImNodesCol_NodeOutline] = ToImGuiColor(Fly::Color{ 0.0f, 0.0f, 0.0f, 1.f });
		style.Colors[ImNodesCol_GridBackground] = ToImGuiColor(Fly::Color{ 0.03f,0.03f, 0.03f, 1.f });

		style.Flags = ImNodesStyleFlags_GridSnapping | ImNodesStyleFlags_GridLines | ImNodesStyleFlags_NodeOutline;
		style.PinCircleRadius = 5.f;

		style.LinkThickness = 3.f;
	}

	void NodeScriptingWindow::Draw()
	{
		PROFILER_FUNCTION(profiler::colors::Amber600);
		if (ImGui::Begin("Node Scripting"))
		{

			UpdateContext();

			if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Z))
			{
				myCommandTracker->UndoCommand();
			}
			else if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Y))
			{
				myCommandTracker->RedoCommand();
			}
			Fly::BeginFrame(myCommandTracker.get());

			if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))
			{
				const int numSelectedNodes = ImNodes::NumSelectedNodes();

				if (numSelectedNodes > 0)
				{
					std::vector<Fly::NodeID> selectedNodes(numSelectedNodes);


					ImNodes::GetSelectedNodes(reinterpret_cast<int*>(selectedNodes.data()));

					Fly::CreateCopyBuffer(selectedNodes, GetNodeContext().myNodeGraphFacade);
				}
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
			{

				const Fly::Vec2 mousePos = Fly::Vec2{ GetMousePos().x, GetMousePos().y };

				Fly::PasteCopyBuffer(mousePos, GetNodeContext().myNodeGraphFacade, myCommandTracker.get());
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_X))
			{
				const int numSelectedNodes = ImNodes::NumSelectedNodes();

				if (numSelectedNodes > 0)
				{
					std::vector<Fly::NodeID> selectedNodes(numSelectedNodes);

					ImNodes::GetSelectedNodes(reinterpret_cast<int*>(selectedNodes.data()));

					Fly::CreateCopyBuffer(selectedNodes, GetNodeContext().myNodeGraphFacade);
					GetNodeContext().myNodeGraphFacade.DestroySelection(selectedNodes, {}, nullptr);
				}
			}

			if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			{
				GetNodeContext().mySearchNodeData.myCurrentIndex = std::max(0, GetNodeContext().mySearchNodeData.myCurrentIndex - 1);
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			{
				GetNodeContext().mySearchNodeData.myCurrentIndex++;
			}

			bool& isDebugging = Fly::IsDebugging();

			ImGui::Checkbox("Debug", &isDebugging);

			if (isDebugging)
			{

				ImGui::Text((std::string("Undo Stack size: ") + std::to_string(myCommandTracker->GetUndoSize())).c_str());
				ImGui::SameLine();
				ImGui::Text((std::string("Redo Stack size: ") + std::to_string(myCommandTracker->GetRedoSize())).c_str());

			}

			ShowSelectionMenu();
			ShowLoadingMenu();
			UpdateContext();


			ShowNodeGraph(*myNodeContextHistory.history[myNodeContextHistory.currentIndex]);

			myClassWindow.Draw();
			myCustomEventWindow.Update();
			myStructCreatorWindow.Update();
			myReflectionMemoryWindow.Update();
			myTraitWindow.Update();


			if (GetNodeContext().myNodeGraphFacade.GetType() == Fly::eNodeGraphType::Function)
			{
				myFunctionSettingsWindow.Update();
			}


		}

		ImGui::End();

	}

	void NodeScriptingWindow::ShowSelectionMenu()
	{

		ImGui::BeginDisabled(myNodeContextHistory.currentIndex == 0);
		if (ImGui::ArrowButton("NodeContextHistoryLeft", ImGuiDir_Left))
		{
			myNodeContextHistory.currentIndex--;
			UpdateContext();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled(myNodeContextHistory.currentIndex + 1 == myNodeContextHistory.history.size());
		if (ImGui::ArrowButton("NodeContextHistoryRight", ImGuiDir_Right))
		{
			myNodeContextHistory.currentIndex++;
			UpdateContext();
		}
		ImGui::EndDisabled();

		if (GetCurrentMode() == eGraphMode::Class)
		{
			ImGui::SameLine();

			std::string className = std::string(GetNodeContext().myClassFacade.GetName());
			if (ImGui::InputString<32>("##", className))
			{
				GetNodeContext().myClassFacade.SetName(className, nullptr);
			}
		}
	}

	void NodeScriptingWindow::ShowLoadingMenu()
	{
		Fly::ClassFacade currentClass = GetNodeContext().myClassFacade;

		std::string currentClassName = currentClass ? std::string(currentClass.GetName()) : "None";

		if (ImGui::BeginCombo("Select Fly Class", currentClassName.c_str()))
		{
			const auto classes = Fly::GetClasses();

			for (auto& [dataTypeFacade, classesByDataTypeID] : classes)
			{
				if (ImGui::BeginMenu(dataTypeFacade.GetName().c_str()))
				{
					for (auto& flyClass : classesByDataTypeID)
					{
						if (ImGui::MenuItem(std::string(flyClass.GetName()).c_str()))
						{
							SetNodeContext(flyClass.GetEventGraphFacade(), flyClass);
						}
					}

					ImGui::EndMenu();
				}
			}
			ImGui::EndCombo();
		}

		const bool canSave = myCommandTracker->GetUndoSize() == 0;

		ImGui::BeginDisabled(canSave);

		if (ImGui::Button("Save"))
		{
			currentClass.Save(ASSET_FILE_PATH);
			myCommandTracker->Clear();
		}

		ImGui::EndDisabled();
		ImGui::SameLine();

		if (ImGui::Button("Reload All"))
		{
			Fly::LoadAllFlyFiles(ASSET_FILE_PATH);
		}

		ImGui::SameLine();

		if (ImGui::Button("Create New Class"))
		{
			ImGui::OpenPopup("Create New Class");
		}

		if (ImGui::BeginPopupModal("Create New Class", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("##", myNewClassNameText, IM_ARRAYSIZE(myNewClassNameText), ImGuiInputTextFlags_AutoSelectAll);

			DataTypeComboTargetableFilter("Select Target##CreateNewClassTarget", mySelectedTargetDataType);

			ImGui::Separator();

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				Fly::ClassFacade createdClassFacade = Fly::CreateClass(mySelectedTargetDataType, myNewClassNameText, ASSET_FILE_PATH);
				myNewClassNameText[0] = (char)0;


				ImGui::CloseCurrentPopup();
			}


			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Copy"))
		{
			ImGui::OpenPopup("Create Copy Class");
		}

		if (ImGui::BeginPopupModal("Create Copy Class", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("##", myCreateCopyNameText, IM_ARRAYSIZE(myCreateCopyNameText), ImGuiInputTextFlags_AutoSelectAll);
			ImGui::Separator();

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				myCreateCopyNameText[0] = (char)0;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Save Custom Events"))
		{
			Fly::SaveCustomEvents(ASSET_FILE_PATH);
		}

		if (GetCurrentMode() == eGraphMode::Class)
		{

			constexpr const char* events[] = { "Begin Play", "Tick", "End Play", "Editor Update" };
			static int currentEventIndex = 0;

			if (ImGui::BeginCombo("Event Type", events[currentEventIndex]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(events); ++i)
				{
					if (ImGui::Selectable(events[i], i == currentEventIndex))
					{
						currentEventIndex = i;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::GetFocusID();

			ImGui::SameLine();

			if (ImGui::Button("Execute Event"))
			{
				Fly::ClassInstanceFacade classInstanceFacade = GetNodeContext().myClassFacade.CreateClassInstance();
				Fly::ExecutionContextBase c
				{
					.mDeltaTime = Global::GetDeltaTime()
				};

				Fly::None none;
				switch (currentEventIndex)
				{
				case 0:
					classInstanceFacade.ExecuteEvent(Fly::BeginPlay, &none, c);
					break;
				case 1:
					classInstanceFacade.ExecuteEvent(Fly::Tick, &none, c);
					break;
				case 2:
					classInstanceFacade.ExecuteEvent(Fly::EndPlay, &none, c);
					break;
				case 3:
					break;
				default:
					break;
				}

				classInstanceFacade.Destroy();
			}
		}

		if (GetNodeContext().myClassFacade.GetTargetDataType().GetID() == Fly::GetDataTypeID<Fly::None*>())
		{
			GetNodeContext().myClassInstanceFacade.GetClassInstance().mEventGraphInstance.Mirror();
			Fly::ExecutionContextBase c
			{
				.mDeltaTime = Global::GetDeltaTime()
			};
			Fly::None none;
			PROFILER_BEGIN("Execute Event");
			GetNodeContext().myClassInstanceFacade.ExecuteEvent(EditorUpdate, &none, c);
			PROFILER_END();

			GetNodeContext().myTraversedLinks = Fly::GetTraversedLinks();
		}
	}

	ImVec2 NodeScriptingWindow::GetMiddlePos() const
	{
		ImNodesContext* currentImNodesContext = GetNodeContext().myImNodesContext;
		return currentImNodesContext->CanvasOriginScreenSpace + ImNodes::EditorContextGetPanning() / 2.f;
	}

	Fly::FunctionFacade NodeScriptingWindow::GetCurrentFunctionFacade()
	{
		return mySelectedFunctionFacade;
	}


	void NodeScriptingWindow::UpdateClickPos()
	{
		GetNodeContext().myNodeCreationClickPos = GetMousePos();
	}

	ImVec2 NodeScriptingWindow::GetMousePos() const
	{
		return ImGui::GetMousePosOnOpeningCurrentPopup() - GetNodeContext().myImNodesContext->CanvasOriginScreenSpace - ImNodes::EditorContextGetPanning();
	}
}