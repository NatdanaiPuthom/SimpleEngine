#include "Editor/Precomplied/EditorPch.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/Fly.hpp"
#include "NodeScript/SimpleScript/Core/Command/FlyCommandTracker.hpp"
#include "NodeScript/SimpleScript/Core/NodeTypes/ExecutionNodes.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "EditorNodeRegistration.hpp"

#include <imnodes/imnodes_internal.h>

namespace Editor
{

	NodeScriptingWindow::NodeScriptingWindow()
		: myClassWindow(*this)
		, myCustomEventWindow(*this)
		, myFunctionSettingsWindow(*this)
	{
		myCommandTracker = std::make_unique<Fly::CommandTracker>();
	}

	NodeScriptingWindow::~NodeScriptingWindow()
	{
		for (auto& [nodeGraph, imNodesContext] : myImNodesContexts)
		{
			ImNodes::DestroyContext(imNodesContext);

		}
	}

	NodeContext& NodeScriptingWindow::GetNodeContext()
	{
		return myNodeContextHistory.history[myNodeContextHistory.currentIndex];
	}

	const NodeContext& NodeScriptingWindow::GetNodeContext() const
	{
		return myNodeContextHistory.history[myNodeContextHistory.currentIndex];
	}



	void NodeScriptingWindow::SetNodeContext(const Fly::NodeGraphFacade aNodeGraphFacade, const Fly::ClassFacade aClassFacade)
	{
		assert(aNodeGraphFacade);
		assert(aClassFacade);

		NodeContext nodeContext
		{
			.myClassFacade = aClassFacade,
			.myNodeGraphFacade = aNodeGraphFacade,
			.myClassInstanceFacade = Fly::CreateClassInstance(aClassFacade)
		};
		myNodeContextHistory.history.push_back(nodeContext);
		myNodeContextHistory.currentIndex++;

		if (!myImNodesContexts.contains(aNodeGraphFacade))
		{
			myImNodesContexts.emplace(aNodeGraphFacade, ImNodes::CreateContext());

		}

		ImNodes::SetCurrentContext(myImNodesContexts.at(aNodeGraphFacade));
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
		Fly::ClassFacade classFacade = Fly::FindClassByName(aName);

		if (!classFacade)
		{
			return false;
		}

		SetNodeContext(classFacade.GetEventGraphFacade(), classFacade);

		return true;
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
				Fly::CreateClass(Fly::GetClasses().begin()->first, "Default Class", ASSET_FILE_PATH);
			}

			Fly::ClassFacade flyClass = classes.begin()->second.front();
			SetNodeContext(flyClass.GetEventGraphFacade(), flyClass);
		}


		ImNodes::SetCurrentContext(myImNodesContexts.at(GetNodeContext().myNodeGraphFacade));

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

		if (ImGui::Begin("Node Scripting"))
		{
			Fly::BeginFrame();


			UpdateContext();

			if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Z))
			{
				myCommandTracker->UndoCommand();
			}
			else if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Y))
			{
				myCommandTracker->RedoCommand();
			}

			if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))
			{
				const int numSelectedNodes = ImNodes::NumSelectedNodes();

				if (numSelectedNodes > 0)
				{
					std::vector<Fly::NodeID> selectedNodes(numSelectedNodes);

					ImNodes::GetSelectedNodes(selectedNodes.data());

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

					ImNodes::GetSelectedNodes(selectedNodes.data());

					Fly::CreateCopyBuffer(selectedNodes, GetNodeContext().myNodeGraphFacade);
					Fly::DestroySelection(selectedNodes, {}, GetNodeContext().myNodeGraphFacade, nullptr);
				}
			}

			bool& isDebugging = Fly::Global::IsDebugging();

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
			VisualizeNodes();
			UpdateNodes();
			NodeCreation();

			myClassWindow.Draw();
			myCustomEventWindow.Update();


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
			char buffer[35]{};
			strcpy_s(buffer, GetNodeContext().myClassFacade.GetName().c_str());

			if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
			{
				Fly::SetClassName(GetNodeContext().myClassFacade, buffer);
			}
		}
	}

	void NodeScriptingWindow::ShowLoadingMenu()
	{
		Fly::ClassFacade currentClass = GetNodeContext().myClassFacade;

		std::string currentClassName = currentClass ? currentClass.GetName() : "None";

		if (ImGui::BeginCombo("Select Fly Class", currentClassName.c_str()))
		{
			const auto classes = Fly::GetClasses();

			for (auto& [dataTypeFacade, classesByDataTypeID] : classes)
			{
				if (ImGui::BeginMenu(dataTypeFacade.GetName().c_str()))
				{
					for (auto& flyClass : classesByDataTypeID)
					{
						if (ImGui::MenuItem(flyClass.GetName().c_str()))
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
			Fly::SaveClass(currentClass, ASSET_FILE_PATH);
			myCommandTracker->Clear();
		}

		ImGui::EndDisabled();
		ImGui::SameLine();

		if (ImGui::Button("Reload All"))
		{
			Fly::LoadAllClasses(ASSET_FILE_PATH);
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

				myImNodesContexts.emplace(createdClassFacade.GetEventGraphFacade(), ImNodes::CreateContext());


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

			ImGui::SameLine();

			if (ImGui::Button("Execute Event"))
			{
				Fly::ClassInstanceFacade classInstanceFacade = Fly::CreateClassInstance(GetNodeContext().myClassFacade);
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

				Fly::DestroyClassInstance(classInstanceFacade);
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
			GetNodeContext().myClassInstanceFacade.ExecuteEvent(EditorUpdate, &none, c);
		}
	}

	void NodeScriptingWindow::VisualizeNodes()
	{
		NodeContext& currentNodeContext = GetNodeContext();

		ImNodes::BeginNodeEditor();

		const std::vector<Fly::NodeFacade> nodeFacades = currentNodeContext.myNodeGraphFacade.GetNodeFacades();

		for (const Fly::NodeFacade& nodeFacade : nodeFacades)
		{
			if (nodeFacade.IsDestroyed())
			{
				continue;
			}

			ImNodesStyle& style = ImNodes::GetStyle();

			if (nodeFacade.GetEventID() == Fly::InvalidID<Fly::EventID>())
			{
				style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f, 0.6f, 1.f });
				style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f, 0.7f, 1.f });
				style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f, 0.8f, 1.f });
			}
			else
			{
				style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(Fly::Color{ 0.8f, 0.1f, 0.1f, 1.f });
				style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(Fly::Color{ 0.9f, 0.1f, 0.1f, 1.f });
				style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(Fly::Color{ 1.f, 0.1f, 0.1f, 1.f });
			}

			style.Colors[ImNodesCol_NodeOutline] = ImNodes::IsNodeSelected(nodeFacade.GetID()) ? ToImGuiColor(Fly::Color(0.8f, 0.8f, 0.8f, 1.f)) : ToImGuiColor(Fly::Color(0.f, 0.f, 0.f, 1.f));

			ImNodes::SetNodeGridSpacePos(nodeFacade.GetID(), ImVec2{ nodeFacade.GetPosition().x, nodeFacade.GetPosition().y });
			ImNodes::BeginNode(nodeFacade.GetID());

			float nodeNameWidth = 0;

			{ // Node title bar

				ImNodes::BeginNodeTitleBar();

				std::string nodeLabel;

				if (Fly::HasFlag(nodeFacade.GetTraits(), Fly::eNodeTrait::Accessor))
				{
					const Fly::VariableFacade variable = Fly::GetVariableByNode(nodeFacade, currentNodeContext.myNodeGraphFacade);
					const bool isGetter = Fly::HasFlag(nodeFacade.GetTraits(), Fly::eNodeTrait::Getter);
					const char* const prefixLabel = isGetter ? "Get " : "Set ";
					nodeLabel = prefixLabel + variable.GetName();
				}
				else
				{
					nodeLabel = nodeFacade.GetShortName();
				}

				if (Fly::Global::IsDebugging())
				{
					nodeLabel += ", ID: " + std::to_string(nodeFacade.GetID());
				}
				ImGui::TextUnformatted(nodeLabel.c_str());

				nodeNameWidth = ImGui::CalcTextSize(nodeLabel.c_str()).x;

				ImNodes::EndNodeTitleBar();
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();

			const std::vector<Fly::PinFacade> inputPinFacades = nodeFacade.GetInputPinFacades();
			float nodeWidthLeft = inputPinFacades.empty() ? 0.f : 100.f;

			for (const Fly::PinFacade& inputPinFacade : inputPinFacades)
			{

				const std::string& pinLabel = inputPinFacade.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(pinLabel.c_str()).x;

				nodeWidthLeft = std::max(nodeWidthLeft, labelWidth);
			}

			float nodeWidthRight = 0.f;

			std::vector<Fly::PinFacade> outputPinFacades = nodeFacade.GetOutputPinFacades();

			for (const Fly::PinFacade& outputPinFacade : outputPinFacades)
			{
				const std::string& pinLabel = outputPinFacade.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(pinLabel.c_str()).x;

				nodeWidthRight = std::max(nodeWidthRight, labelWidth);
			}

			float extraWidth = 0.f;

			if (nodeWidthLeft > 0.f && nodeWidthRight > 0.f)
			{
				extraWidth += 30.f;
			}

			// Render input pins
			for (Fly::PinFacade inputPinFacade : inputPinFacades)
			{
				const Fly::DataTypeFacade pinDataType(inputPinFacade.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor() - myHoverTint));

				const bool shouldBeHighlighted = std::find(currentNodeContext.myPinFacadesToHighlight.begin(), currentNodeContext.myPinFacadesToHighlight.end(), inputPinFacade) != currentNodeContext.myPinFacadesToHighlight.end();

				const ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;
				ImNodes::BeginInputAttribute(inputPinFacade.GetID(), shape);

				std::string pinLabel = inputPinFacade.GetPinTypeName();
				if (!pinLabel.empty())
				{
					if (Fly::Global::IsDebugging())
					{
						pinLabel += ", " + std::to_string(inputPinFacade.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				if (inputPinFacade.GetConnectedPinIDs().empty())
				{
					const float itemWidth = std::max(20.f, nodeWidthLeft);
					ImGui::PushItemWidth(itemWidth);

					inputPinFacade.Edit(myCommandTracker.get());

					ImGui::PopItemWidth();
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}


			ImGui::SetCursorPos(cursorPos);

			// Render output pins
			for (const Fly::PinFacade& outputPinFacade : outputPinFacades)
			{
				const Fly::DataTypeFacade pinDataType(outputPinFacade.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor() - myHoverTint));

				const bool shouldBeHighlighted = std::find(currentNodeContext.myPinFacadesToHighlight.begin(), currentNodeContext.myPinFacadesToHighlight.end(), outputPinFacade) != currentNodeContext.myPinFacadesToHighlight.end();

				const ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;

				ImNodes::BeginOutputAttribute(outputPinFacade.GetID(), shape);

				std::string pinLabel = outputPinFacade.GetPinTypeName();

				if (!pinLabel.empty())
				{
					const float plusWidth = std::max(nodeWidthLeft + extraWidth, nodeNameWidth - nodeWidthRight);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + plusWidth + extraWidth + nodeWidthRight - ImGui::CalcTextSize(pinLabel.c_str()).x);

					if (Fly::Global::IsDebugging())
					{
						pinLabel += ", " + std::to_string(outputPinFacade.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}

			ImNodes::EndNode();
		}

		std::vector<Fly::LinkFacade> linkFacades = Fly::GetLinks(currentNodeContext.myNodeGraphFacade);

		for (const Fly::LinkFacade& linkFacade : linkFacades)
		{
			if (linkFacade.IsDestroyed())
			{
				continue;
			}
			const Fly::PinFacade pinFacade = linkFacade.GetInputPin();
			const Fly::DataTypeFacade pinDataType(pinFacade.GetDataTypeID());

			ImNodes::PushColorStyle(ImNodesCol_Link, ToImGuiColor(pinDataType.GetColor()));
			ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ToImGuiColor(pinDataType.GetColor() - mySelectionTint));
			ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ToImGuiColor(pinDataType.GetColor() - myHoverTint));

			ImNodes::Link(linkFacade.GetID(), linkFacade.GetInputPin().GetID(), linkFacade.GetOutputPin().GetID());

			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
		}

		ImNodes::MiniMap(.2f, ImNodesMiniMapLocation_BottomRight);
		ImNodes::EndNodeEditor();
	}

	void NodeScriptingWindow::UpdateNodes()
	{
		NodeContext& currentNodeContext = GetNodeContext();

		bool dragStarted = ImGui::IsKeyDown(ImGuiKey_MouseLeft) && !myIsDraggingNode;
		if (dragStarted)
		{
			if (ImNodes::NumSelectedNodes() > 0) // Drag node started
			{
				myIsDraggingNode = true;
			}
		}

		bool dragEnded = ImGui::IsKeyReleased(ImGuiKey_MouseLeft) && myIsDraggingNode;
		if (dragEnded)
		{
			myIsDraggingNode = false;
		}

		const std::vector<Fly::NodeFacade> nodeFacades = currentNodeContext.myNodeGraphFacade.GetNodeFacades();

		for (Fly::NodeFacade nodeFacade : nodeFacades)
		{
			if (nodeFacade.IsDestroyed())
			{
				continue;
			}

			const Fly::Vec2 newPos = ToFlyVec2(ImNodes::GetNodeGridSpacePos(nodeFacade.GetID()));
			Fly::Vec2 oldPos = nodeFacade.GetPosition();
			if (dragStarted && ImNodes::IsNodeSelected(nodeFacade.GetID()))
			{
				currentNodeContext.myNodeDragData.emplace(nodeFacade.GetID(), Fly::NodeDragData{ .mStartPos = oldPos });
			}
			else if (dragEnded)
			{
				auto it = currentNodeContext.myNodeDragData.find(nodeFacade.GetID());

				if (it != currentNodeContext.myNodeDragData.end())
				{
					it->second.mEndPos = newPos;
				}
			}
			if (newPos != oldPos)
			{
				nodeFacade.SetPosition(newPos);
			}
		}

		if (dragEnded)
		{
			currentNodeContext.myNodeGraphFacade.CommitNodeDrag(currentNodeContext.myNodeDragData, myCommandTracker.get());
			currentNodeContext.myNodeDragData.clear();
		}

		Fly::NodeID hoveredNodeID = Fly::InvalidID<Fly::NodeID>();
		if (ImNodes::IsNodeHovered(&hoveredNodeID) && ImGui::IsKeyPressed(ImGuiKey_MouseRight))
		{
			myClickedNodeFacade = Fly::NodeFacade(hoveredNodeID, currentNodeContext.myNodeGraphFacade);

			ImGui::OpenPopup("Node Popup");
		}

		if (ImGui::BeginPopup("Node Popup"))
		{
			ImGui::Text("Node Options");

			ImGui::Separator();
			if (ImGui::Selectable("Destroy Node"))
			{
				myClickedNodeFacade.Destroy(myCommandTracker.get());
				ImGui::CloseCurrentPopup();
			}

			ImGui::BeginDisabled(!myClickedNodeFacade.HasAnyConnectedLinks());
			if (ImGui::Selectable("Destroy Links"))
			{
				myClickedNodeFacade.DestroyConnectedLinks(myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::Separator();

			if (myClickedNodeFacade.IsReplacable() && ImGui::BeginMenu("Data Type"))
			{
				const std::vector<Fly::DataTypeFacade> replacableDataTypeFacades = Fly::GetReplacableDataTypes(myClickedNodeFacade, currentNodeContext.myNodeGraphFacade);
				for (const Fly::DataTypeFacade& replacableDataTypeFacade : replacableDataTypeFacades)
				{
					ImGui::PushID(static_cast<int>(replacableDataTypeFacade.GetID()));
					if (ImGui::Selectable(replacableDataTypeFacade.GetName().c_str()))
					{
						currentNodeContext.myNodeGraphFacade.ReplaceTemplateNode(myClickedNodeFacade, replacableDataTypeFacade, myCommandTracker.get());
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopID();
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
		else
		{
			myClickedNodeFacade = {};
		}

		Fly::PinID hoveredPinID;
		if (ImNodes::IsPinHovered(&hoveredPinID) && ImGui::IsKeyPressed(ImGuiKey_MouseRight))
		{
			myClickedPinFacade = Fly::PinFacade(hoveredPinID, currentNodeContext.myNodeGraphFacade);

			ImGui::OpenPopup("Pin Popup");

		}

		if (ImGui::BeginPopup("Pin Popup"))
		{

			ImGui::Text("Pin Options");

			ImGui::Separator();

			ImGui::BeginDisabled(!myClickedPinFacade.HasAnyConnectedLinks());
			if (ImGui::Selectable("Destroy Links"))
			{
				myClickedPinFacade.DestroyConnectedLinks(myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::Separator();

			if (myClickedPinFacade.IsParentNodeReplacable() && ImGui::BeginMenu("Choose Type"))
			{
				const std::vector<Fly::DataTypeFacade> replacableDataTypeFacades = Fly::GetReplacableDataTypes(myClickedPinFacade, currentNodeContext.myNodeGraphFacade);
				for (const Fly::DataTypeFacade& replacableDataTypeFacade : replacableDataTypeFacades)
				{
					ImGui::PushID(static_cast<int>(replacableDataTypeFacade.GetID()));
					if (ImGui::Selectable(replacableDataTypeFacade.GetName().c_str(), replacableDataTypeFacade.GetID() == myClickedPinFacade.GetDataTypeID()))
					{
						currentNodeContext.myNodeGraphFacade.ReplaceTemplateNode(myClickedPinFacade, replacableDataTypeFacade, myCommandTracker.get());
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopID();
				}

				ImGui::EndMenu();
			}


			ImGui::EndPopup();
		}
		else
		{
			myClickedPinFacade = Fly::PinFacade();
		}


		// See if links should be created

		Fly::PinID createdLinkPinID1 = Fly::InvalidID<Fly::PinID>();
		Fly::PinID createdLinkPinID2 = Fly::InvalidID<Fly::PinID>();

		if (ImNodes::IsLinkCreated(&createdLinkPinID1, &createdLinkPinID2))
		{
			currentNodeContext.myNodeGraphFacade.TryCreateLink(Fly::PinFacade(createdLinkPinID1, GetNodeContext().myNodeGraphFacade), Fly::PinFacade(createdLinkPinID2, GetNodeContext().myNodeGraphFacade), myCommandTracker.get());
			currentNodeContext.myPinFacadesToHighlight.clear();
		}

		// See if links or nodes should be destroyed

		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			std::vector<Fly::LinkID> selectedLinks(ImNodes::NumSelectedLinks());
			std::vector<Fly::NodeID> selectedNodes(ImNodes::NumSelectedNodes());

			if (!selectedLinks.empty())
			{
				ImNodes::GetSelectedLinks(selectedLinks.data());
			}

			if (!selectedNodes.empty())
			{
				ImNodes::GetSelectedNodes(selectedNodes.data());
			}

			if (!selectedLinks.empty() || !selectedNodes.empty())
			{
				currentNodeContext.myNodeGraphFacade.DestroySelection(selectedNodes, selectedLinks, myCommandTracker.get());
			}

			ImNodes::ClearNodeSelection();
			ImNodes::ClearLinkSelection();
		}

		// Highlight pins
		Fly::PinID startedPinID = Fly::InvalidID<Fly::PinID>();
		if (ImNodes::IsLinkStarted(&startedPinID))
		{
			currentNodeContext.myStartedLinkPinID = startedPinID;

			const Fly::PinFacade startedPin(startedPinID, currentNodeContext.myNodeGraphFacade);

			currentNodeContext.myPinFacadesToHighlight = GetNodeContext().myNodeGraphFacade.GetNonConnectedPinFacadesByFlowTypeAndDataType(InvertFlowType(startedPin.GetFlowType()), Fly::DataTypeFacade(startedPin.GetDataTypeID()));

			std::erase_if(currentNodeContext.myPinFacadesToHighlight,
				[&](const Fly::PinFacade& aPinFacade)-> bool
				{
					return aPinFacade.GetID() == startedPin.GetID();
				}
			);
		}

		// Dropped link
		Fly::PinID droppedPinID = Fly::InvalidID<Fly::PinID>();

		if (ImNodes::IsLinkDropped(&droppedPinID))
		{
			ImGui::OpenPopup("Node Create Popup");
			currentNodeContext.myLinkCreationPinID = droppedPinID;
			currentNodeContext.myPinFacadesToHighlight.clear();

			UpdateClickPos();
		}

		// Drop link create popup
		if (ImGui::BeginPopup("Node Create Popup"))
		{
			const Fly::PinFacade pinFacade(currentNodeContext.myLinkCreationPinID, GetNodeContext().myNodeGraphFacade);

			auto nodeTypePopulationFunc = [&](NodeTypeCategory& aMainCategory) -> void
				{

					const std::vector<Fly::NodeTypeFacade> filteredNodeTypesByDataTypeAndFlowType = Fly::GetNodeTypesFilteredByDataTypeAndFlowType(pinFacade.GetDataTypeID(), InvertFlowType(pinFacade.GetFlowType()));

					for (const Fly::NodeTypeFacade& nodeType : filteredNodeTypesByDataTypeAndFlowType)
					{
						PopulateCategories(nodeType.GetName(), nodeType, aMainCategory);
					}
				};

			auto onClickCallback = [&](const Fly::NodeTypeFacade& aNodeTypeFacade) -> void
				{

					Fly::CreateNodeAutoLink(GetNodeContext().myNodeGraphFacade, aNodeTypeFacade, currentNodeContext.myLinkCreationPinID, Fly::Vec2{ myNodeCreationClickPos.x, myNodeCreationClickPos.y }, myCommandTracker.get());


					currentNodeContext.myPinFacadesToHighlight.clear();
					myNodeTypeSearch[0] = '\0';
				};

			ShowNodeCreationMenu(nodeTypePopulationFunc, onClickCallback);

			ImGui::EndPopup();
		}

		if (Fly::Global::IsDebugging())
		{

			if (ImGui::Begin("Debug Data"))
			{

				ImGui::Text("Links:");

				const std::vector<Fly::LinkFacade> linkFacades = Fly::GetLinks(GetNodeContext().myNodeGraphFacade);

				for (const Fly::LinkFacade& linkFacade : linkFacades)
				{
					if (linkFacade.IsDestroyed())
					{
						continue;
					}
					ImGui::Separator();
					ImGui::Text("In: %d, Out: %d", linkFacade.GetInputPin().GetID(), linkFacade.GetOutputPin().GetID());
				}

				ImGui::Separator();

				if (myHoveredPinFacade)
				{
					ImGui::Text("Hovered PinID: %d", myHoveredPinFacade.GetID());
					ImGui::Text("Connections:");
					for (const Fly::PinID connectionID : myHoveredPinFacade.GetConnectedPinIDs())
					{
						ImGui::Text("%d", connectionID);
					}
				}

			}
			ImGui::End();
		}

	}

	void NodeScriptingWindow::PopulateCategories(const std::string& aName, const Fly::NodeTypeFacade& aNodeType, NodeTypeCategory& aCategory)
	{
		std::string categoryName = aName.substr(0, aName.find_first_of('/'));
		std::string rest = aName.substr(categoryName.length(), aName.length() - 1);

		if (rest.empty())
		{
			aCategory.nodeTypes.push_back(aNodeType);
			return;
		}

		rest = rest.substr(1, rest.length());

		bool hasFoundCategory = false;
		for (NodeTypeCategory& childCategory : aCategory.childCategories)
		{
			if (childCategory.name == categoryName)
			{
				PopulateCategories(rest, aNodeType, childCategory);
				hasFoundCategory = true;
				break;
			}
		}

		if (!hasFoundCategory)
		{
			aCategory.childCategories.emplace_back(categoryName);
			PopulateCategories(rest, aNodeType, aCategory.childCategories.back());
		}
	}

	void NodeScriptingWindow::ShowNodeTypeCreationMenu(const std::vector<Fly::NodeTypeFacade>& aNodeTypes, const std::function<void(const Fly::NodeTypeFacade&)>& aOnClickFunc)
	{
		for (const Fly::NodeTypeFacade& nodeType : aNodeTypes)
		{
			if (ImGui::MenuItem(nodeType.GetShortName().c_str()))
			{
				aOnClickFunc(nodeType);
			}
		}
	}

	void NodeScriptingWindow::ShowNodeCreationMenuByCategory(const NodeTypeCategory& aCategory, const std::function<void(const Fly::NodeTypeFacade&)>& aOnClickFunc)
	{
		if (ImGui::BeginMenu(aCategory.name.c_str()))
		{

			for (const NodeTypeCategory& childCategory : aCategory.childCategories)
			{
				ShowNodeCreationMenuByCategory(childCategory, aOnClickFunc);
			}

			ShowNodeTypeCreationMenu(aCategory.nodeTypes, aOnClickFunc);

			ImGui::EndMenu();
		}
	}

	void NodeScriptingWindow::ShowNodeCreationMenu(const std::function<void(NodeTypeCategory&)>& aCategoryFunction, const std::function<void(const Fly::NodeTypeFacade&)>& aOnClickFunction)
	{
		NodeTypeCategory mainCategory{ "Create Node" };
		aCategoryFunction(mainCategory);

		ImGui::SetKeyboardFocusHere();
		ImGui::InputTextWithHint("##", "Node Type", myNodeTypeSearch, IM_ARRAYSIZE(myNodeTypeSearch));

		//ImGui::Keyboard

		if (myNodeTypeSearch[0] == '\0')
		{

			for (const NodeTypeCategory& category : mainCategory.childCategories)
			{
				ShowNodeCreationMenuByCategory(category, aOnClickFunction);
			}
		}
		else
		{
			ShowNodeTypeCreationMenu(mainCategory.nodeTypes, aOnClickFunction);
		}
	}

	static bool StringCompare(std::string_view aStr1, std::string_view aStr2)
	{
		auto it = std::search(
			aStr1.begin(), aStr1.end(),
			aStr2.begin(), aStr2.end(),
			[](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
		);
		return it != aStr1.end();
	}

	void NodeScriptingWindow::NodeCreation()
	{
		if (ImGui::IsKeyPressed(ImGuiKey_MouseRight) && !myClickedPinFacade && !myClickedNodeFacade)
		{
			ImGui::OpenPopup("Node Type Selection");
			UpdateClickPos();
		}

		if (ImGui::BeginPopup("Node Type Selection"))
		{
			auto categoryFunc = [&](NodeTypeCategory& aMainCategory) -> void
				{

					if (myNodeTypeSearch[0] == '\0')
					{

						const std::vector<Fly::NodeTypeFacade> filteredNodeTypes = Fly::GetNodeTypesFilteredByTrait(Fly::eNodeTrait::NonTrivial, Fly::HasNotFlag);

						for (const Fly::NodeTypeFacade& nodeType : filteredNodeTypes)
						{
							PopulateCategories(nodeType.GetName(), nodeType, aMainCategory);
						}
					}
					else
					{
						const std::vector<Fly::NodeTypeFacade> filteredNodeTypes = Fly::GetNodeTypesFilteredByTrait(Fly::eNodeTrait::NonTrivial, Fly::HasNotFlag);

						for (const Fly::NodeTypeFacade& nodeType : filteredNodeTypes)
						{
							const bool isSearched = StringCompare(nodeType.GetName(), myNodeTypeSearch);
							if (isSearched)
							{
								aMainCategory.nodeTypes.push_back(nodeType);
							}
						}
					}
				};

			auto onClickCallback = [&](const Fly::NodeTypeFacade& aNodeTypeFacade) -> void
				{
					NodeContext& currentNodeContext = GetNodeContext();

					currentNodeContext.myNodeGraphFacade.CreateNode(aNodeTypeFacade, Fly::Vec2{ myNodeCreationClickPos.x, myNodeCreationClickPos.y }, myCommandTracker.get());


					currentNodeContext.myPinFacadesToHighlight.clear();
					myNodeTypeSearch[0] = '\0';
				};

			ShowNodeCreationMenu(categoryFunc, onClickCallback);
			ImGui::EndPopup();
		}
	}

	ImVec2 NodeScriptingWindow::GetMiddlePos() const
	{
		const NodeContext& currentContext = GetNodeContext();
		ImNodesContext* currentImNodesContext = myImNodesContexts.at(currentContext.myNodeGraphFacade);
		return currentImNodesContext->CanvasOriginScreenSpace + ImNodes::EditorContextGetPanning() / 2.f;
	}

	Fly::FunctionFacade NodeScriptingWindow::GetCurrentFunctionFacade()
	{
		return mySelectedFunctionFacade;
	}


	void NodeScriptingWindow::UpdateClickPos()
	{
		myNodeCreationClickPos = GetMousePos();
	}

	ImVec2 NodeScriptingWindow::GetMousePos() const
	{
		NodeContext currentContext = GetNodeContext();
		ImNodesContext* currentImNodesContext = myImNodesContexts.at(currentContext.myNodeGraphFacade);
		return ImGui::GetMousePosOnOpeningCurrentPopup() - currentImNodesContext->CanvasOriginScreenSpace - ImNodes::EditorContextGetPanning();
	}
}