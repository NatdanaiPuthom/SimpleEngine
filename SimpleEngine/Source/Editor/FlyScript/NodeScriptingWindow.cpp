#include "Editor/Precomplied/EditorPch.hpp"
#include "NodeScriptingWindow.hpp"
#include "NodeScript/SimpleScript/Core/Script.h"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.h"
#include "NodeScript/SimpleScript/Core/Serialization/ScriptLoader.h"
#include "NodeScript/SimpleScript/Core/Utilities/ScriptFilter.h"
#include "NodeScript/SimpleScript/Core/ScriptModifier.h"
#include "NodeScript/SimpleScript/Core/Command/ScriptCommandTracker.h"
#include "NodeScript/SimpleScript/Core/ScriptFoundation.h"
#include "NodeScript/SimpleScript/Core/Instance/ScriptInstance.h"
#include "NodeScript/SimpleScript/Core/NodeTypes/ExecutionNodes.h"
#include "NodeScript/SimpleScript/Core/Node/NodeExecutor.h"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.h"
#include "NodeScript/SimpleScript/Core/Utilities/ScriptLinker.h"

#include "Editor/Menu/MainMenuBar.hpp" //NOTE(v10.0.2): Remove this once we no longer use static bool of MainMenuBar class

#include <imnodes/imnodes_internal.h>

namespace Editor
{

	NodeScriptingWindow::NodeScriptingWindow()
		: myVariableWindow(*this)
		, myNodeCreatorWindow(*this)
		, myFunctionWindow(*this)
		, myFunctionSettingsWindow(*this)
	{
		myCommandTracker = std::make_unique<SCRIPT::CommandTracker>();
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



	void NodeScriptingWindow::SetNodeContext(SCRIPT::NodeGraph& aNodeGraph, SCRIPT::Script* aScript)
	{
		NodeContext nodeContext
		{
			aScript,
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
		return GetNodeContext().script ? eScriptMode::Class : eScriptMode::Global;
	}

	void NodeScriptingWindow::UpdateContext()
	{
		if (myNodeContextHistory.currentIndex == -1)
		{
			auto& scripts = SCRIPT::GetScripts();
			if (scripts.empty())
			{
				SCRIPT::CreateScript(0, "Default Script");
			}

			if (scripts.begin()->second.empty())
			{
				SCRIPT::CreateScript(SCRIPT::GetScripts().begin()->first, "Default Script");
			}

			SCRIPT::Script& script = *scripts.begin()->second.front();
			SetNodeContext(script.GetEventGraph(), &script);
		}


		ImNodes::SetCurrentContext(myImNodesContexts.at(GetNodeContext().nodeGraph));

		ImNodesStyle& style = ImNodes::GetStyle();

		style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(SCRIPT::Color{ 0.1f, 0.3f , 0.6f , 1.f });
		style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(SCRIPT::Color{ 0.1f, 0.3f , 0.7f , 1.f });
		style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(SCRIPT::Color{ 0.1f, 0.3f , 0.8f , 1.f });

		style.Colors[ImNodesCol_NodeBackground] = ToImGuiColor(SCRIPT::Color{ 0.02f, 0.02f, 0.02f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundHovered] = ToImGuiColor(SCRIPT::Color{ 0.015f, 0.015f, 0.015f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundSelected] = ToImGuiColor(SCRIPT::Color{ 0.01f, 0.01f, 0.01f, 1.f });

		style.Colors[ImNodesCol_NodeOutline] = ToImGuiColor(SCRIPT::Color{ 0.0f, 0.0f, 0.0f, 1.f });
		style.Colors[ImNodesCol_GridBackground] = ToImGuiColor(SCRIPT::Color{ 0.03f,0.03f, 0.03f, 1.f });
	}

	void NodeScriptingWindow::Draw()
	{
		SCRIPT::BeginFrame();


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
					std::vector<SCRIPT::NodeID> selectedNodes(numSelectedNodes);

					ImNodes::GetSelectedNodes(selectedNodes.data());

					SCRIPT::CreateCopyBuffer(selectedNodes, *GetNodeContext().nodeGraph);
				}
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
			{

				const SCRIPT::Vec2 mousePos = SCRIPT::Vec2{ GetMousePos().x, GetMousePos().y };

				SCRIPT::PasteCopyBuffer(mousePos, *GetNodeContext().nodeGraph, myCommandTracker.get());
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_X))
			{
				const int numSelectedNodes = ImNodes::NumSelectedNodes();

				if (numSelectedNodes > 0)
				{
					std::vector<SCRIPT::NodeID> selectedNodes(numSelectedNodes);

					ImNodes::GetSelectedNodes(selectedNodes.data());

					SCRIPT::CreateCopyBuffer(selectedNodes, *GetNodeContext().nodeGraph);
					SCRIPT::DestroySelection(selectedNodes, {}, *GetNodeContext().nodeGraph, nullptr);
				}
			}

			bool& isDebugging = SCRIPT::Global::IsDebugging();

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
				SCRIPT::Script& currentScript = *GetNodeContext().script;
				if (ImGui::Button("Create Member Function"))
				{
					SCRIPT::CreateMemberFunction("Function1", currentScript);
				}

				const std::vector<SCRIPT::FunctionID>& memberFunctionIDs = currentScript.GetMemberFunctionIDs();

				for (const SCRIPT::FunctionID memberFunctionID : memberFunctionIDs)
				{
					SCRIPT::FunctionView memberFunction(memberFunctionID);

					if (ImGui::Selectable(memberFunction.GetName().c_str(), &memberFunction.GetNodeGraph() == GetNodeContext().nodeGraph))
					{
						SetNodeContext(memberFunction.GetNodeGraph(), GetNodeContext().script);
						mySelectedFunctionID = memberFunctionID;
						break;
					}
				}
			}

			if (GetNodeContext().nodeGraph->GetType() == SCRIPT::eNodeGraphType::Function)
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
			strcpy_s(buffer, GetNodeContext().script->Name().c_str());

			if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
			{
				GetNodeContext().script->Name() = buffer;
			}
		}
	}

	void NodeScriptingWindow::ScriptLoadingMenu()
	{
		SCRIPT::Script& currentScript = *GetNodeContext().script;

		const bool canSave = myCommandTracker->GetUndoSize() == 0;

		ImGui::BeginDisabled(canSave);

		if (ImGui::Button("Save"))
		{
			SCRIPT::ScriptLoader::SaveScript(currentScript, SCRIPT_FILE_PATH);
			myCommandTracker->Clear();
		}

		ImGui::EndDisabled();
		ImGui::SameLine();

		if (ImGui::Button("Reload All"))
		{
			SCRIPT::ScriptLoader::LoadAllScripts(SCRIPT_FILE_PATH);
		}

		ImGui::SameLine();

		if (ImGui::Button("Create New Script"))
		{
			ImGui::OpenPopup("Create New Script");
		}

		if (ImGui::BeginPopupModal("Create New Script", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("##", myScriptNameText, IM_ARRAYSIZE(myScriptNameText), ImGuiInputTextFlags_AutoSelectAll);
			ImGui::Separator();

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				SCRIPT::Script& script = SCRIPT::CreateScript(0, myScriptNameText);
				myScriptNameText[0] = (char)0;

				myImNodesContexts.emplace(&script.GetEventGraph(), ImNodes::CreateContext());


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
			ImGui::InputText("##", myScriptNameText, IM_ARRAYSIZE(myScriptNameText), ImGuiInputTextFlags_AutoSelectAll);
			ImGui::Separator();

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				myScriptNameText[0] = (char)0;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Save Custom Events"))
		{
			SCRIPT::ScriptLoader::SaveCustomEvents("Assets/VisualScripting");
		}

		if (GetCurrentMode() == eScriptMode::Class)
		{

			static int currentEventIndex = 0;
			const char* events[] = { "Begin Play", "Tick", "End Play", };

			ImGui::Combo("Event Type", &currentEventIndex, events, IM_ARRAYSIZE(events));
			ImGui::SameLine();


			if (ImGui::Button("Trigger Event"))
			{
				SCRIPT::ScriptInstance& scriptInstance = GetNodeContext().script->CreateScriptInstance();
				SCRIPT::ExecutionContextBase c;

				switch (currentEventIndex)
				{
				case 0:
					SCRIPT::Global::GetNodeExecutor().ExecuteEvent(SCRIPT::BeginPlay, scriptInstance, nullptr, c);
					break;
				case 1:
					SCRIPT::Global::GetNodeExecutor().ExecuteEvent(SCRIPT::Tick, scriptInstance, nullptr, c);
					break;
				case 2:
					SCRIPT::Global::GetNodeExecutor().ExecuteEvent(SCRIPT::EndPlay, scriptInstance, nullptr, c);
					break;
				default:
					break;
				}

				GetNodeContext().script->DestroyScriptInstance(scriptInstance);
			}
		}
	}

	void NodeScriptingWindow::VisualizeNodes()
	{
		NodeContext& currentNodeContext = GetNodeContext();
		const SCRIPT::Script& currentScript = *GetNodeContext().script;

		ImNodes::BeginNodeEditor();

		const std::vector<SCRIPT::NodeView> nodeViews = SCRIPT::GetNodes(*GetNodeContext().nodeGraph);

		for (const SCRIPT::NodeView& nodeView : nodeViews)
		{
			if (nodeView.IsDestroyed())
			{
				continue;
			}

			ImNodesStyle& style = ImNodes::GetStyle();

			if (nodeView.GetEventID() == SCRIPT::InvalidID<SCRIPT::EventID>())
			{
				style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(SCRIPT::Color{ 0.1f, 0.3f, 0.6f, 1.f });
				style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(SCRIPT::Color{ 0.1f, 0.3f, 0.7f, 1.f });
				style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(SCRIPT::Color{ 0.1f, 0.3f, 0.8f, 1.f });
			}
			else
			{
				style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(SCRIPT::Color{ 0.8f, 0.1f, 0.1f, 1.f });
				style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(SCRIPT::Color{ 0.9f, 0.1f, 0.1f, 1.f });
				style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(SCRIPT::Color{ 1.f, 0.1f, 0.1f, 1.f });
			}

			ImNodes::SetNodeGridSpacePos(nodeView.GetID(), ImVec2{ nodeView.GetPosition().x, nodeView.GetPosition().y });
			ImNodes::BeginNode(nodeView.GetID());

			float nodeNameWidth = 0;

			{ // Node title bar

				ImNodes::BeginNodeTitleBar();

				std::string nodeLabel;

				if (SCRIPT::HasFlag(nodeView.GetTraits(), SCRIPT::eNodeTrait::Accessor))
				{
					const SCRIPT::VariableView variable = SCRIPT::GetVariableByNodeID(nodeView.GetID(), *currentNodeContext.nodeGraph, currentScript);
					const bool isGetter = SCRIPT::HasFlag(nodeView.GetTraits(), SCRIPT::eNodeTrait::Getter);
					const char* const prefixLabel = isGetter ? "Get" : "Set";
					nodeLabel = prefixLabel + variable.GetName();
				}
				else
				{
					nodeLabel = nodeView.GetShortName();
				}

				if (SCRIPT::Global::IsDebugging())
				{
					nodeLabel += ", ID: " + std::to_string(nodeView.GetID());
				}
				ImGui::TextUnformatted(nodeLabel.c_str());

				nodeNameWidth = ImGui::CalcTextSize(nodeLabel.c_str()).x;

				ImNodes::EndNodeTitleBar();
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();

			const std::vector<SCRIPT::PinView> inputPins = nodeView.GetInputPins();
			float nodeWidthLeft = inputPins.empty() ? 0.f : 100.f;

			for (const SCRIPT::PinView& pinView : inputPins)
			{

				const std::string& pinLabel = pinView.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(pinLabel.c_str()).x;

				nodeWidthLeft = std::max(nodeWidthLeft, labelWidth);
			}

			float nodeWidthRight = 0.f;

			std::vector<SCRIPT::PinView> outputPins = nodeView.GetOutputPins();

			for (const SCRIPT::PinView& pinView : outputPins)
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
			for (const SCRIPT::PinView& inputPinView : inputPins)
			{

				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetColor(inputPinView.GetDataTypeID())));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetHoverColor(inputPinView.GetDataTypeID())));

				const bool shouldBeHighlighted = std::find(currentNodeContext.myPinIDsToHighlight.begin(), currentNodeContext.myPinIDsToHighlight.end(), inputPinView.GetID()) != currentNodeContext.myPinIDsToHighlight.end();

				const ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;
				ImNodes::BeginInputAttribute(inputPinView.GetID(), shape);

				std::string pinLabel = inputPinView.GetPinTypeName();
				if (!pinLabel.empty())
				{
					if (SCRIPT::Global::IsDebugging())
					{
						pinLabel += ", " + std::to_string(inputPinView.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				if (inputPinView.GetConnectedPinIDs().empty())
				{
					const float itemWidth = std::max(20.f, nodeWidthLeft);
					ImGui::PushItemWidth(itemWidth);

					SCRIPT::EditPin(inputPinView.GetID(), *GetNodeContext().nodeGraph, myCommandTracker.get());

					ImGui::PopItemWidth();
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}


			ImGui::SetCursorPos(cursorPos);

			// Render output pins
			for (const SCRIPT::PinView& outputPinView : outputPins)
			{

				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetColor(outputPinView.GetDataTypeID())));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetHoverColor(outputPinView.GetDataTypeID())));

				const bool shouldBeHighlighted = std::find(currentNodeContext.myPinIDsToHighlight.begin(), currentNodeContext.myPinIDsToHighlight.end(), outputPinView.GetID()) != currentNodeContext.myPinIDsToHighlight.end();

				const ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;

				ImNodes::BeginOutputAttribute(outputPinView.GetID(), shape);

				std::string pinLabel = outputPinView.GetPinTypeName();

				if (!pinLabel.empty())
				{
					const float plusWidth = std::max(nodeWidthLeft + extraWidth, nodeNameWidth - nodeWidthRight);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + plusWidth + extraWidth + nodeWidthRight - ImGui::CalcTextSize(pinLabel.c_str()).x);

					if (SCRIPT::Global::IsDebugging())
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

		std::vector<SCRIPT::LinkView> linkViews = SCRIPT::GetLinks(*currentNodeContext.nodeGraph);

		for (const SCRIPT::LinkView& linkView : linkViews)
		{
			if (linkView.IsDestroyed())
			{
				continue;
			}
			const SCRIPT::PinView pinView = linkView.GetInputPin();

			ImNodes::PushColorStyle(ImNodesCol_Link, ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetColor(pinView.GetDataTypeID())));
			ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetSelectionColor(pinView.GetDataTypeID())));
			ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ToImGuiColor(SCRIPT::Global::GetDataTypeManager().GetHoverColor(pinView.GetDataTypeID())));

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

		std::vector<SCRIPT::NodeView> nodeViews = SCRIPT::GetNodes(*currentNodeContext.nodeGraph);

		for (const SCRIPT::NodeView& nodeView : nodeViews)
		{
			if (nodeView.IsDestroyed())
			{
				continue;
			}

			const SCRIPT::Vec2 newPos = ToScriptVec2(ImNodes::GetNodeGridSpacePos(nodeView.GetID()));
			SCRIPT::Vec2 oldPos = nodeView.GetPosition();
			if (dragStarted && ImNodes::IsNodeSelected(nodeView.GetID()))
			{
				currentNodeContext.myNodeDragData.emplace(nodeView.GetID(), SCRIPT::NodeDragData{ .startPos = oldPos });
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
				SCRIPT::SetNodePosition(nodeView.GetID(), newPos, *GetNodeContext().nodeGraph);
			}
		}

		if (dragEnded)
		{
			SCRIPT::CommitNodeDrag(currentNodeContext.myNodeDragData, *GetNodeContext().nodeGraph, myCommandTracker.get());
			currentNodeContext.myNodeDragData.clear();
		}

		// See if links should be created

		SCRIPT::PinID createdLinkPinID1 = SCRIPT::InvalidID<SCRIPT::PinID>();
		SCRIPT::PinID createdLinkPinID2 = SCRIPT::InvalidID<SCRIPT::PinID>();

		if (ImNodes::IsLinkCreated(&createdLinkPinID1, &createdLinkPinID2))
		{
			SCRIPT::TryCreateLink(createdLinkPinID1, createdLinkPinID2, *GetNodeContext().nodeGraph, myCommandTracker.get());
			currentNodeContext.myPinIDsToHighlight.clear();
		}

		// See if links or nodes should be destroyed

		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			std::vector<SCRIPT::LinkID> selectedLinks(ImNodes::NumSelectedLinks());
			std::vector<SCRIPT::NodeID> selectedNodes(ImNodes::NumSelectedNodes());

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
				SCRIPT::DestroySelection(selectedNodes, selectedLinks, *GetNodeContext().nodeGraph, myCommandTracker.get());
			}

			ImNodes::ClearNodeSelection();
			ImNodes::ClearLinkSelection();
		}

		// Highlight pins
		SCRIPT::PinID startedPinID = SCRIPT::InvalidID<SCRIPT::PinID>();
		if (ImNodes::IsLinkStarted(&startedPinID))
		{
			currentNodeContext.myStartedLinkPinID = startedPinID;

			const SCRIPT::PinView pin(startedPinID, *currentNodeContext.nodeGraph);

			currentNodeContext.myPinIDsToHighlight = SCRIPT::ScriptFilter::GetNonConnectedPinsOfTypeAndHash(*GetNodeContext().nodeGraph, InvertFlowType(pin.GetFlowType()), pin.GetDataTypeID());

			for (SCRIPT::PinID i = 0; i < currentNodeContext.myPinIDsToHighlight.size(); i++)
			{
				if (SCRIPT::PinView(currentNodeContext.myPinIDsToHighlight[i], *currentNodeContext.nodeGraph).GetNodeID() == pin.GetNodeID())
				{
					currentNodeContext.myPinIDsToHighlight.erase(currentNodeContext.myPinIDsToHighlight.begin() + i);
					i--;
				}
			}
		}

		// Dropped link
		SCRIPT::PinID droppedPinID = SCRIPT::InvalidID<SCRIPT::PinID>();

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
			const SCRIPT::PinView pinView(currentNodeContext.myLinkCreationPinID, *GetNodeContext().nodeGraph);

			auto nodeTypePopulationFunc = [&](NodeTypeCategory& aMainCategory) -> void
				{

					const std::vector<SCRIPT::NodeTypeView> filteredNodeTypesByDataTypeAndFlowType = SCRIPT::GetNodeTypesFilteredByDataTypeAndFlowType(pinView.GetDataTypeID(), InvertFlowType(pinView.GetFlowType()));

					for (const SCRIPT::NodeTypeView& nodeType : filteredNodeTypesByDataTypeAndFlowType)
					{
						PopulateCategories(nodeType.GetName(), nodeType, aMainCategory);
					}
				};

			auto onClickCallback = [&](const SCRIPT::NodeTypeView& aNodeType) -> void
				{

					SCRIPT::CreateNodeAutoLink(*GetNodeContext().nodeGraph, aNodeType.GetID(), currentNodeContext.myLinkCreationPinID, SCRIPT::Vec2{myNodeCreationClickPos.x, myNodeCreationClickPos.y}, myCommandTracker.get());


					currentNodeContext.myPinIDsToHighlight.clear();
					myNodeTypeSearch[0] = '\0';
				};

			ShowNodeCreationMenu(nodeTypePopulationFunc, onClickCallback);

			ImGui::EndPopup();
		}

		if (SCRIPT::Global::IsDebugging())
		{

			if (ImGui::Begin("Debug Data"))
			{

				ImGui::Text("Links:");

				const std::vector<SCRIPT::LinkView> linkViews = SCRIPT::GetLinks(*GetNodeContext().nodeGraph);

				for (const SCRIPT::LinkView& linkView : linkViews)
				{
					if (linkView.IsDestroyed())
					{
						continue;
					}
					ImGui::Separator();
					ImGui::Text("In: %d, Out: %d", linkView.GetInputPin().GetID(), linkView.GetOutputPin().GetID());
				}

				ImGui::Separator();

				if (myHoveredPinID != SCRIPT::InvalidID<SCRIPT::PinID>())
				{
					ImGui::Text("Hovered PinID: %d", myHoveredPinID);
					const SCRIPT::PinView hoveredPin = SCRIPT::PinView(myHoveredPinID, *GetNodeContext().nodeGraph);
					ImGui::Text("Connections:");
					for (const SCRIPT::PinID connectionID : hoveredPin.GetConnectedPinIDs())
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
			myHoveredPinID = SCRIPT::InvalidID<SCRIPT::PinID>();
		}
	}

	void NodeScriptingWindow::PopulateCategories(const std::string& aName, const SCRIPT::NodeTypeView& aNodeType, NodeTypeCategory& aCategory)
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

	void NodeScriptingWindow::ShowNodeTypeCreationMenu(const std::vector<SCRIPT::NodeTypeView>& aNodeTypes, const std::function<void(const SCRIPT::NodeTypeView&)>& aOnClickFunc)
	{
		for (const SCRIPT::NodeTypeView& nodeType : aNodeTypes)
		{
			if (ImGui::MenuItem(nodeType.GetShortName().c_str()))
			{
				aOnClickFunc(nodeType);
			}
		}
	}

	void NodeScriptingWindow::ShowNodeCreationMenuByCategory(const NodeTypeCategory& aCategory, const std::function<void(const SCRIPT::NodeTypeView&)>& aOnClickFunc)
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

	void NodeScriptingWindow::ShowNodeCreationMenu(const std::function<void(NodeTypeCategory&)>& aCategoryFunction, const std::function<void(const SCRIPT::NodeTypeView&)>& aOnClickFunction)
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

						const std::vector<SCRIPT::NodeTypeView> filteredNodeTypes = SCRIPT::GetNodeTypesFilteredByTrait(SCRIPT::eNodeTrait::NonTrivial, SCRIPT::HasNotFlag);

						for (const SCRIPT::NodeTypeView& nodeType : filteredNodeTypes)
						{
							PopulateCategories(nodeType.GetName(), nodeType, aMainCategory);
						}
					}
					else
					{
						const std::vector<SCRIPT::NodeTypeView> nodeTypes = SCRIPT::GetNodeTypes();
						for (const SCRIPT::NodeTypeView& nodeType : nodeTypes)
						{

							if (SCRIPT::HasFlag(nodeType.GetTraits(), SCRIPT::eNodeTrait::NonTrivial))
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

			auto onClickCallback = [&](const SCRIPT::NodeTypeView& aNodeType) -> void
				{
					NodeContext& currentNodeContext = GetNodeContext();

					SCRIPT::CreateNode(*GetNodeContext().nodeGraph, aNodeType.GetID(), SCRIPT::Vec2{myNodeCreationClickPos.x, myNodeCreationClickPos.y}, myCommandTracker.get());


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

	SCRIPT::FunctionID NodeScriptingWindow::GetCurrentFunctionID() const
	{
		return mySelectedFunctionID;
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