#include "Editor/Precomplied/EditorPch.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/FlyClass.hpp"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.hpp"
#include "NodeScript/SimpleScript/Core/Serialization/ScriptLoader.hpp"
#include "NodeScript/SimpleScript/Core/Utilities/ScriptFilter.hpp"
#include "NodeScript/SimpleScript/Core/ScriptModifier.hpp"
#include "NodeScript/SimpleScript/Core/Command/ScriptCommandTracker.hpp"
#include "NodeScript/SimpleScript/Core/Instance/FlyClassInstance.hpp"
#include "NodeScript/SimpleScript/Core/NodeTypes/ExecutionNodes.hpp"
#include "NodeScript/SimpleScript/Core/Node/NodeExecutor.hpp"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.hpp"

#include "Editor/Menu/MainMenuBar.hpp" //NOTE(v10.0.2): Remove this once we no longer use static bool of MainMenuBar class

#include <imnodes/imnodes_internal.h>

namespace Editor
{

	static void DataTypeComboSelection(const char* aComboLabel, Fly::DataTypeView& aDataTypeView)
	{
		if (ImGui::BeginCombo(aComboLabel, aDataTypeView.GetName().c_str()))
		{

			const std::vector<Fly::DataTypeView> dataTypes = Fly::GetDataTypesFiltered(
				[](const Fly::DataTypeView& aDataType) -> bool
				{
					return aDataType.IsTargetable();
				}
			);

			for (const Fly::DataTypeView& dataType : dataTypes)
			{
				if (ImGui::Selectable(dataType.GetName().c_str()))
				{
					aDataTypeView = dataType;
				}
			}

			ImGui::EndCombo();
		}
	}

	NodeScriptingWindow::NodeScriptingWindow()
		: myVariableWindow(*this)
		, myNodeCreatorWindow(*this)
		, myFunctionWindow(*this)
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



	void NodeScriptingWindow::SetNodeContext(Fly::NodeGraph& aNodeGraph, Fly::ClassView aClassView)
	{
		NodeContext nodeContext
		{
			aClassView,
			&aNodeGraph,
		};
		myNodeContextHistory.history.push_back(nodeContext);
		myNodeContextHistory.currentIndex++;

		if (!myImNodesContexts.contains(&aNodeGraph))
		{
			myImNodesContexts.emplace(&aNodeGraph, ImNodes::CreateContext());

		}

		ImNodes::SetCurrentContext(myImNodesContexts.at(&aNodeGraph));
	}

	eScriptMode NodeScriptingWindow::GetCurrentMode() const
	{
		return GetNodeContext().classView ? eScriptMode::Class : eScriptMode::Global;
	}

	void NodeScriptingWindow::UpdateContext()
	{
		if (myNodeContextHistory.currentIndex == -1)
		{
			auto classes = Fly::GetClasses();
			if (classes.empty())
			{
				Fly::CreateClass(Fly::GetDataTypeID<Fly::None>(), "Default Class");
			}

			classes = Fly::GetClasses();
			if (classes.begin()->second.empty())
			{
				Fly::CreateClass(Fly::GetClasses().begin()->first, "Default Class");
			}

			Fly::ClassView flyClass = classes.begin()->second.front();
			SetNodeContext(flyClass.GetEventGraph(), flyClass);
		}


		ImNodes::SetCurrentContext(myImNodesContexts.at(GetNodeContext().nodeGraph));

		ImNodesStyle& style = ImNodes::GetStyle();

		style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f , 0.6f , 1.f });
		style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f , 0.7f , 1.f });
		style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(Fly::Color{ 0.1f, 0.3f , 0.8f , 1.f });

		style.Colors[ImNodesCol_NodeBackground] = ToImGuiColor(Fly::Color{ 0.02f, 0.02f, 0.02f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundHovered] = ToImGuiColor(Fly::Color{ 0.015f, 0.015f, 0.015f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundSelected] = ToImGuiColor(Fly::Color{ 0.01f, 0.01f, 0.01f, 1.f });

		style.Colors[ImNodesCol_NodeOutline] = ToImGuiColor(Fly::Color{ 0.0f, 0.0f, 0.0f, 1.f });
		style.Colors[ImNodesCol_GridBackground] = ToImGuiColor(Fly::Color{ 0.03f,0.03f, 0.03f, 1.f });
	}

	void NodeScriptingWindow::Draw()
	{
		Fly::BeginFrame();


		UpdateContext();

		if (Editor::MainMenuBar::myStaticNodeScriptWindowActive == false) //TO-DO(v10.0.2): May move this somehow?
		{
			return;
		}

		if (ImGui::Begin("Node Scripting"))
		{
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

					Fly::CreateCopyBuffer(selectedNodes, *GetNodeContext().nodeGraph);
				}
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
			{

				const Fly::Vec2 mousePos = Fly::Vec2{ GetMousePos().x, GetMousePos().y };

				Fly::PasteCopyBuffer(mousePos, *GetNodeContext().nodeGraph, myCommandTracker.get());
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_X))
			{
				const int numSelectedNodes = ImNodes::NumSelectedNodes();

				if (numSelectedNodes > 0)
				{
					std::vector<Fly::NodeID> selectedNodes(numSelectedNodes);

					ImNodes::GetSelectedNodes(selectedNodes.data());

					Fly::CreateCopyBuffer(selectedNodes, *GetNodeContext().nodeGraph);
					Fly::DestroySelection(selectedNodes, {}, *GetNodeContext().nodeGraph, nullptr);
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

			ScriptSelectionMenu();
			ScriptLoadingMenu();
			UpdateContext();
			VisualizeNodes();
			UpdateNodes();
			NodeCreation();

			myVariableWindow.Update();
			myNodeCreatorWindow.Update();
			myFunctionWindow.Update();


			if (ImGui::Begin("Member Functions"))
			{
				Fly::ClassView currentScript = GetNodeContext().classView;
				if (ImGui::Button("Create Member Function"))
				{
					Fly::CreateMemberFunction("Function1", currentScript);
				}

				std::vector<Fly::FunctionView> memberFunctions = currentScript.GetFunctions();

				for (Fly::FunctionView& memberFunction : memberFunctions)
				{

					if (ImGui::Selectable(memberFunction.GetName().c_str(), &memberFunction.GetNodeGraph() == GetNodeContext().nodeGraph))
					{
						SetNodeContext(memberFunction.GetNodeGraph(), GetNodeContext().classView);
						mySelectedFunction = Fly::FunctionView(memberFunction.GetID());
						break;
					}
				}
			}

			if (GetNodeContext().nodeGraph->GetType() == Fly::eNodeGraphType::Function)
			{
				myFunctionSettingsWindow.Update();
			}

			ImGui::End();


		}

		ImGui::End();

	}

	void NodeScriptingWindow::ScriptSelectionMenu()
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

		if (GetCurrentMode() == eScriptMode::Class)
		{
			ImGui::SameLine();
			char buffer[35]{};
			strcpy_s(buffer, GetNodeContext().classView.GetName().c_str());

			if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
			{
				Fly::SetClassName(GetNodeContext().classView, buffer);
			}
		}
	}

	void NodeScriptingWindow::ScriptLoadingMenu()
	{
		Fly::ClassView currentClass = GetNodeContext().classView;

		/*if (!currentClass)
		{
			return;
		}*/

		std::string currentClassName = currentClass ? currentClass.GetName() : "None";

		if (ImGui::BeginCombo("Select Fly Class", currentClassName.c_str()))
		{
			const auto classes = Fly::GetClasses();

			for (auto& [dataTypeID, classesByDataTypeID] : classes)
			{
				if (ImGui::BeginMenu(Fly::DataTypeView(dataTypeID).GetName().c_str()))
				{
					for (auto& flyClass : classesByDataTypeID)
					{
						if (ImGui::MenuItem(flyClass.GetName().c_str()))
						{
							SetNodeContext(flyClass.GetEventGraph(), flyClass);
						}
					}

					ImGui::EndMenu();
				}
			}
			//ImGui::MenuItem()
			ImGui::EndCombo();
		}

		const bool canSave = myCommandTracker->GetUndoSize() == 0;

		ImGui::BeginDisabled(canSave);

		if (ImGui::Button("Save"))
		{
			Fly::SaveClass(currentClass, SCRIPT_FILE_PATH);
			myCommandTracker->Clear();
		}

		ImGui::EndDisabled();
		ImGui::SameLine();

		if (ImGui::Button("Reload All"))
		{
			Fly::ScriptLoader::LoadAllClasses(SCRIPT_FILE_PATH);
		}

		ImGui::SameLine();

		if (ImGui::Button("Create New Script"))
		{
			ImGui::OpenPopup("Create New Script");
		}

		if (ImGui::BeginPopupModal("Create New Script", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("##", myNewClassNameText, IM_ARRAYSIZE(myNewClassNameText), ImGuiInputTextFlags_AutoSelectAll);

			DataTypeComboSelection("Select Target##CreateNewScriptTarget", mySelectedTargetDataType);

			ImGui::Separator();

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				Fly::ClassView classView = Fly::CreateClass(Fly::GetDataTypeID<Fly::None>(), myNewClassNameText);
				myNewClassNameText[0] = (char)0;

				myImNodesContexts.emplace(&classView.GetEventGraph(), ImNodes::CreateContext());


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
			ImGui::OpenPopup("Create Copy Script");
		}

		if (ImGui::BeginPopupModal("Create Copy Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
			Fly::ScriptLoader::SaveCustomEvents("Assets/VisualScripting");
		}

		if (GetCurrentMode() == eScriptMode::Class)
		{

			static int currentEventIndex = 0;
			const char* events[] = { "Begin Play", "Tick", "End Play", };

			ImGui::Combo("Event Type", &currentEventIndex, events, IM_ARRAYSIZE(events));
			ImGui::SameLine();


			if (ImGui::Button("Trigger Event"))
			{
				Fly::ClassInstance& classInstance = GetNodeContext().classView.GetClass().CreateClassInstance();
				Fly::ExecutionContextBase c;

				switch (currentEventIndex)
				{
				case 0:
					Fly::Global::GetNodeExecutor().ExecuteEvent(Fly::BeginPlay, classInstance, nullptr, c);
					break;
				case 1:
					Fly::Global::GetNodeExecutor().ExecuteEvent(Fly::Tick, classInstance, nullptr, c);
					break;
				case 2:
					Fly::Global::GetNodeExecutor().ExecuteEvent(Fly::EndPlay, classInstance, nullptr, c);
					break;
				default:
					break;
				}

				GetNodeContext().classView.GetClass().DestroyClassInstance(classInstance);
			}
		}
	}

	void NodeScriptingWindow::VisualizeNodes()
	{
		NodeContext& currentNodeContext = GetNodeContext();
		const Fly::ClassView currentClass = GetNodeContext().classView;

		ImNodes::BeginNodeEditor();

		const std::vector<Fly::NodeView> nodeViews = Fly::GetNodes(*GetNodeContext().nodeGraph);

		for (const Fly::NodeView& nodeView : nodeViews)
		{
			if (nodeView.IsDestroyed())
			{
				continue;
			}

			ImNodesStyle& style = ImNodes::GetStyle();

			if (nodeView.GetEventID() == Fly::InvalidID<Fly::EventID>())
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

			ImNodes::SetNodeGridSpacePos(nodeView.GetID(), ImVec2{ nodeView.GetPosition().x, nodeView.GetPosition().y });
			ImNodes::BeginNode(nodeView.GetID());

			float nodeNameWidth = 0;

			{ // Node title bar

				ImNodes::BeginNodeTitleBar();

				std::string nodeLabel;

				if (Fly::HasFlag(nodeView.GetTraits(), Fly::eNodeTrait::Accessor))
				{
					const Fly::VariableView variable = Fly::GetVariableByNodeID(nodeView.GetID(), *currentNodeContext.nodeGraph, currentClass);
					const bool isGetter = Fly::HasFlag(nodeView.GetTraits(), Fly::eNodeTrait::Getter);
					const char* const prefixLabel = isGetter ? "Get" : "Set";
					nodeLabel = prefixLabel + variable.GetName();
				}
				else
				{
					nodeLabel = nodeView.GetShortName();
				}

				if (Fly::Global::IsDebugging())
				{
					nodeLabel += ", ID: " + std::to_string(nodeView.GetID());
				}
				ImGui::TextUnformatted(nodeLabel.c_str());

				nodeNameWidth = ImGui::CalcTextSize(nodeLabel.c_str()).x;

				ImNodes::EndNodeTitleBar();
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();

			const std::vector<Fly::PinView> inputPins = nodeView.GetInputPins();
			float nodeWidthLeft = inputPins.empty() ? 0.f : 100.f;

			for (const Fly::PinView& pinView : inputPins)
			{

				const std::string& pinLabel = pinView.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(pinLabel.c_str()).x;

				nodeWidthLeft = std::max(nodeWidthLeft, labelWidth);
			}

			float nodeWidthRight = 0.f;

			std::vector<Fly::PinView> outputPins = nodeView.GetOutputPins();

			for (const Fly::PinView& pinView : outputPins)
			{
				const std::string& pinLabel = pinView.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(pinLabel.c_str()).x;

				nodeWidthRight = std::max(nodeWidthRight, labelWidth);
			}

			float extraWidth = 0.f;

			if (nodeWidthLeft > 0.f && nodeWidthRight > 0.f)
			{
				extraWidth += 30.f;
			}

			// Render input pins
			for (const Fly::PinView& inputPinView : inputPins)
			{
				const Fly::DataTypeView pinDataType(inputPinView.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor() - myHoverTint));

				const bool shouldBeHighlighted = std::find(currentNodeContext.myPinIDsToHighlight.begin(), currentNodeContext.myPinIDsToHighlight.end(), inputPinView.GetID()) != currentNodeContext.myPinIDsToHighlight.end();

				const ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;
				ImNodes::BeginInputAttribute(inputPinView.GetID(), shape);

				std::string pinLabel = inputPinView.GetPinTypeName();
				if (!pinLabel.empty())
				{
					if (Fly::Global::IsDebugging())
					{
						pinLabel += ", " + std::to_string(inputPinView.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				if (inputPinView.GetConnectedPinIDs().empty())
				{
					const float itemWidth = std::max(20.f, nodeWidthLeft);
					ImGui::PushItemWidth(itemWidth);

					Fly::EditPin(inputPinView.GetID(), *GetNodeContext().nodeGraph, myCommandTracker.get());

					ImGui::PopItemWidth();
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}


			ImGui::SetCursorPos(cursorPos);

			// Render output pins
			for (const Fly::PinView& outputPinView : outputPins)
			{
				const Fly::DataTypeView pinDataType(outputPinView.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor() - myHoverTint));

				const bool shouldBeHighlighted = std::find(currentNodeContext.myPinIDsToHighlight.begin(), currentNodeContext.myPinIDsToHighlight.end(), outputPinView.GetID()) != currentNodeContext.myPinIDsToHighlight.end();

				const ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;

				ImNodes::BeginOutputAttribute(outputPinView.GetID(), shape);

				std::string pinLabel = outputPinView.GetPinTypeName();

				if (!pinLabel.empty())
				{
					const float plusWidth = std::max(nodeWidthLeft + extraWidth, nodeNameWidth - nodeWidthRight);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + plusWidth + extraWidth + nodeWidthRight - ImGui::CalcTextSize(pinLabel.c_str()).x);

					if (Fly::Global::IsDebugging())
					{
						pinLabel += ", " + std::to_string(outputPinView.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}

			ImNodes::EndNode();
		}

		std::vector<Fly::LinkView> linkViews = Fly::GetLinks(*currentNodeContext.nodeGraph);

		for (const Fly::LinkView& linkView : linkViews)
		{
			if (linkView.IsDestroyed())
			{
				continue;
			}
			const Fly::PinView pinView = linkView.GetInputPin();
			const Fly::DataTypeView pinDataType(pinView.GetDataTypeID());

			ImNodes::PushColorStyle(ImNodesCol_Link, ToImGuiColor(pinDataType.GetColor()));
			ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ToImGuiColor(pinDataType.GetColor() - mySelectionTint));
			ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ToImGuiColor(pinDataType.GetColor() - myHoverTint));

			ImNodes::Link(linkView.GetID(), linkView.GetInputPin().GetID(), linkView.GetOutputPin().GetID());

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

		std::vector<Fly::NodeView> nodeViews = Fly::GetNodes(*currentNodeContext.nodeGraph);

		for (const Fly::NodeView& nodeView : nodeViews)
		{
			if (nodeView.IsDestroyed())
			{
				continue;
			}

			const Fly::Vec2 newPos = ToScriptVec2(ImNodes::GetNodeGridSpacePos(nodeView.GetID()));
			Fly::Vec2 oldPos = nodeView.GetPosition();
			if (dragStarted && ImNodes::IsNodeSelected(nodeView.GetID()))
			{
				currentNodeContext.myNodeDragData.emplace(nodeView.GetID(), Fly::NodeDragData{ .startPos = oldPos });
			}
			else if (dragEnded)
			{
				auto it = currentNodeContext.myNodeDragData.find(nodeView.GetID());

				if (it != currentNodeContext.myNodeDragData.end())
				{
					it->second.endPos = newPos;
				}
			}
			if (newPos != oldPos)
			{
				Fly::SetNodePosition(nodeView.GetID(), newPos, *GetNodeContext().nodeGraph);
			}
		}

		if (dragEnded)
		{
			Fly::CommitNodeDrag(currentNodeContext.myNodeDragData, *GetNodeContext().nodeGraph, myCommandTracker.get());
			currentNodeContext.myNodeDragData.clear();
		}

		// See if links should be created

		Fly::PinID createdLinkPinID1 = Fly::InvalidID<Fly::PinID>();
		Fly::PinID createdLinkPinID2 = Fly::InvalidID<Fly::PinID>();

		if (ImNodes::IsLinkCreated(&createdLinkPinID1, &createdLinkPinID2))
		{
			Fly::TryCreateLink(createdLinkPinID1, createdLinkPinID2, *GetNodeContext().nodeGraph, myCommandTracker.get());
			currentNodeContext.myPinIDsToHighlight.clear();
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
				Fly::DestroySelection(selectedNodes, selectedLinks, *GetNodeContext().nodeGraph, myCommandTracker.get());
			}

			ImNodes::ClearNodeSelection();
			ImNodes::ClearLinkSelection();
		}

		// Highlight pins
		Fly::PinID startedPinID = Fly::InvalidID<Fly::PinID>();
		if (ImNodes::IsLinkStarted(&startedPinID))
		{
			currentNodeContext.myStartedLinkPinID = startedPinID;

			const Fly::PinView pin(startedPinID, *currentNodeContext.nodeGraph);

			currentNodeContext.myPinIDsToHighlight = Fly::ScriptFilter::GetNonConnectedPinsOfTypeAndHash(*GetNodeContext().nodeGraph, InvertFlowType(pin.GetFlowType()), pin.GetDataTypeID());

			for (Fly::PinID i = 0; i < currentNodeContext.myPinIDsToHighlight.size(); i++)
			{
				if (Fly::PinView(currentNodeContext.myPinIDsToHighlight[i], *currentNodeContext.nodeGraph).GetNodeID() == pin.GetNodeID())
				{
					currentNodeContext.myPinIDsToHighlight.erase(currentNodeContext.myPinIDsToHighlight.begin() + i);
					i--;
				}
			}
		}

		// Dropped link
		Fly::PinID droppedPinID = Fly::InvalidID<Fly::PinID>();

		if (ImNodes::IsLinkDropped(&droppedPinID))
		{
			ImGui::OpenPopup("Node Create Popup");
			currentNodeContext.myLinkCreationPinID = droppedPinID;
			currentNodeContext.myPinIDsToHighlight.clear();

			UpdateClickPos();
		}

		// Drop link create popup
		if (ImGui::BeginPopup("Node Create Popup"))
		{
			const Fly::PinView pinView(currentNodeContext.myLinkCreationPinID, *GetNodeContext().nodeGraph);

			auto nodeTypePopulationFunc = [&](NodeTypeCategory& aMainCategory) -> void
				{

					const std::vector<Fly::NodeTypeView> filteredNodeTypesByDataTypeAndFlowType = Fly::GetNodeTypesFilteredByDataTypeAndFlowType(pinView.GetDataTypeID(), InvertFlowType(pinView.GetFlowType()));

					for (const Fly::NodeTypeView& nodeType : filteredNodeTypesByDataTypeAndFlowType)
					{
						PopulateCategories(nodeType.GetName(), nodeType, aMainCategory);
					}
				};

			auto onClickCallback = [&](const Fly::NodeTypeView& aNodeType) -> void
				{

					Fly::CreateNodeAutoLink(*GetNodeContext().nodeGraph, aNodeType.GetID(), currentNodeContext.myLinkCreationPinID, Fly::Vec2{ myNodeCreationClickPos.x, myNodeCreationClickPos.y }, myCommandTracker.get());


					currentNodeContext.myPinIDsToHighlight.clear();
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

				const std::vector<Fly::LinkView> linkViews = Fly::GetLinks(*GetNodeContext().nodeGraph);

				for (const Fly::LinkView& linkView : linkViews)
				{
					if (linkView.IsDestroyed())
					{
						continue;
					}
					ImGui::Separator();
					ImGui::Text("In: %d, Out: %d", linkView.GetInputPin().GetID(), linkView.GetOutputPin().GetID());
				}

				ImGui::Separator();

				if (myHoveredPinID != Fly::InvalidID<Fly::PinID>())
				{
					ImGui::Text("Hovered PinID: %d", myHoveredPinID);
					const Fly::PinView hoveredPin = Fly::PinView(myHoveredPinID, *GetNodeContext().nodeGraph);
					ImGui::Text("Connections:");
					for (const Fly::PinID connectionID : hoveredPin.GetConnectedPinIDs())
					{
						ImGui::Text("%d", connectionID);
					}
				}

			}
			ImGui::End();
		}

		ImNodes::GetStyle().PinCircleRadius = 5.f;

		if (!ImNodes::IsPinHovered(&myHoveredPinID))
		{
			myHoveredPinID = Fly::InvalidID<Fly::PinID>();
		}
	}

	void NodeScriptingWindow::PopulateCategories(const std::string& aName, const Fly::NodeTypeView& aNodeType, NodeTypeCategory& aCategory)
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

	void NodeScriptingWindow::ShowNodeTypeCreationMenu(const std::vector<Fly::NodeTypeView>& aNodeTypes, const std::function<void(const Fly::NodeTypeView&)>& aOnClickFunc)
	{
		for (const Fly::NodeTypeView& nodeType : aNodeTypes)
		{
			if (ImGui::MenuItem(nodeType.GetShortName().c_str()))
			{
				aOnClickFunc(nodeType);
			}
		}
	}

	void NodeScriptingWindow::ShowNodeCreationMenuByCategory(const NodeTypeCategory& aCategory, const std::function<void(const Fly::NodeTypeView&)>& aOnClickFunc)
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

	void NodeScriptingWindow::ShowNodeCreationMenu(const std::function<void(NodeTypeCategory&)>& aCategoryFunction, const std::function<void(const Fly::NodeTypeView&)>& aOnClickFunction)
	{
		NodeTypeCategory mainCategory{ "Create Node" };
		aCategoryFunction(mainCategory);

		//ImGui::SetKeyboardFocusHere();
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

	bool StringCompare(std::string_view aStr1, std::string_view aStr2)
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
		if (ImGui::IsKeyPressed(ImGuiKey_MouseRight))
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

						const std::vector<Fly::NodeTypeView> filteredNodeTypes = Fly::GetNodeTypesFilteredByTrait(Fly::eNodeTrait::NonTrivial, Fly::HasNotFlag);

						for (const Fly::NodeTypeView& nodeType : filteredNodeTypes)
						{
							PopulateCategories(nodeType.GetName(), nodeType, aMainCategory);
						}
					}
					else
					{
						const std::vector<Fly::NodeTypeView> nodeTypes = Fly::GetNodeTypes();
						for (const Fly::NodeTypeView& nodeType : nodeTypes)
						{

							if (Fly::HasFlag(nodeType.GetTraits(), Fly::eNodeTrait::NonTrivial))
							{
								continue;
							}

							const bool isSearched = StringCompare(nodeType.GetName(), myNodeTypeSearch);
							//const bool isSearched = std::strstr(nodeType.GetName().c_str(), myNodeTypeSearch);
							if (isSearched)
							{
								aMainCategory.nodeTypes.push_back(nodeType);
							}
						}
					}
				};

			auto onClickCallback = [&](const Fly::NodeTypeView& aNodeType) -> void
				{
					NodeContext& currentNodeContext = GetNodeContext();

					Fly::CreateNode(*GetNodeContext().nodeGraph, aNodeType.GetID(), Fly::Vec2{ myNodeCreationClickPos.x, myNodeCreationClickPos.y }, myCommandTracker.get());


					currentNodeContext.myPinIDsToHighlight.clear();
					myNodeTypeSearch[0] = '\0';
				};

			ShowNodeCreationMenu(categoryFunc, onClickCallback);
			ImGui::EndPopup();
		}
	}

	ImVec2 NodeScriptingWindow::GetMiddlePos() const
	{
		const NodeContext& currentContext = GetNodeContext();
		ImNodesContext* currentImNodesContext = myImNodesContexts.at(currentContext.nodeGraph);
		return currentImNodesContext->CanvasOriginScreenSpace + ImNodes::EditorContextGetPanning() / 2.f;
	}

	Fly::FunctionView NodeScriptingWindow::GetCurrentFunction() const
	{
		return mySelectedFunction;
	}


	void NodeScriptingWindow::UpdateClickPos()
	{
		myNodeCreationClickPos = GetMousePos();
	}

	ImVec2 NodeScriptingWindow::GetMousePos() const
	{
		NodeContext currentContext = GetNodeContext();
		ImNodesContext* currentImNodesContext = myImNodesContexts.at(currentContext.nodeGraph);
		return ImGui::GetMousePosOnOpeningCurrentPopup() - currentImNodesContext->CanvasOriginScreenSpace - ImNodes::EditorContextGetPanning();
	}
}