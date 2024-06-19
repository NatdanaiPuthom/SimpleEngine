#include "SimpleScriptEditor/VisualScriptingWindow.h"
#include "SimpleScript/Core/ScriptManager.h"
#include "SimpleScript/Core/Script.h"
#include "SimpleScript/Core/Node/NodeTypeRegistry.h"
#include "SimpleScript/Core/Node/NodeTypeManager.h"
#include "SimpleScript/Core/DataType/DataTypeManager.h"
#include "SimpleScript/Core/Node/NodeManager.h"
#include "SimpleScript/Core/Serialization/ScriptLoader.h"
#include "SimpleScript/Core/Utilities/ScriptFilter.h"
#include "SimpleScript/Core/ScriptModifier.h"
#include "SimpleScript/Core/CustomEvent/CustomEvent.h"
#include "SimpleScript/Core/Command/ScriptCommandTracker.h"
#include "SimpleScript/Core/ScriptFoundation.h"
#include "SimpleScript/Core/Instance/ScriptInstance.h"

#include "Editor/Menu/MainMenuBar.hpp" //NOTE(v10.0.2): Remove this once we no longer use static bool of MainMenuBar class

#include <imnodes/imnodes_internal.h>

namespace Editor
{
	using namespace SCRIPT;

	VisualScriptingWindow::VisualScriptingWindow()
		: myVariableWindow(*this)
		, myNodeCreatorWindow(*this)
		, myFunctionWindow(*this)
	{
		myCommandTracker = std::make_unique<CommandTracker>();
		myCurrentNodeGraph = nullptr;

	}

	VisualScriptingWindow::~VisualScriptingWindow()
	{
		for (auto& [nodeGraph, imNodesContext] : myImNodesContexts)
		{
			ImNodes::DestroyContext(imNodesContext);

		}

		//ImNodes::CreateContext(); //NOTE(v11.2.0): Is it supposed to call CreateContext in destructor?

		ScriptLoader::Clear();
	}

	NodeContext VisualScriptingWindow::GetCurrentContext() const
	{
		return { myCurrentScript, myCurrentNodeGraph, myImNodesContexts.at(myCurrentNodeGraph) };
	}

	void VisualScriptingWindow::UpdateContext()
	{
		if (!myCurrentNodeGraph)
		{
			if (ScriptFoundation::GetInstance().GetScripts().empty())
			{
				ScriptFoundation::GetInstance().CreateScript(0);
			}

			if (ScriptFoundation::GetInstance().GetScripts().begin()->second.empty())
			{
				ScriptFoundation::GetInstance().CreateScript(ScriptFoundation::GetInstance().GetScripts().begin()->first);
			}

			Script& script = *ScriptFoundation::GetInstance().GetScripts().begin()->second.front();
			myCurrentNodeGraph = &script.GetEventGraph();
			myCurrentScript = &script;
			myImNodesContexts.emplace(myCurrentNodeGraph, ImNodes::CreateContext());
		}


		ImNodes::SetCurrentContext(myImNodesContexts.at(myCurrentNodeGraph));

		ImNodesStyle& style = ImNodes::GetStyle();

		style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(Color{ 0.1f, 0.3f , 0.6f , 1.f });
		style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(Color{ 0.1f, 0.3f , 0.7f , 1.f });
		style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(Color{ 0.1f, 0.3f , 0.8f , 1.f });

		style.Colors[ImNodesCol_NodeBackground] = ToImGuiColor(Color{ 0.02f, 0.02f, 0.02f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundHovered] = ToImGuiColor(Color{ 0.015f, 0.015f, 0.015f, 1.f });
		style.Colors[ImNodesCol_NodeBackgroundSelected] = ToImGuiColor(Color{ 0.01f, 0.01f, 0.01f, 1.f });

		style.Colors[ImNodesCol_NodeOutline] = ToImGuiColor(Color{ 0.0f, 0.0f, 0.0f, 1.f });
		style.Colors[ImNodesCol_GridBackground] = ToImGuiColor(Color{ 0.03f,0.03f, 0.03f, 1.f });
	}

	void VisualScriptingWindow::Update(const std::string& LevelName)
	{
		ScriptLoader::SavePath = "../Source/Script/data/SimpleScripts/" + LevelName;
		UpdateContext();

		if (Editor::MainMenuBar::myStaticNodeScriptWindowActive == false) //TO-DO(v10.0.2): May move this somehow?
		{
			return;
		}

		if (ImGui::Begin("Node Scripting"))
		{
			myCommandTracker->Update(
				[]() -> bool
				{
					return ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z);
				},
				[]() -> bool
				{
					return  ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_Y);
				}
			);

			if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))
			{
				int numSelectedNodes = ImNodes::NumSelectedNodes();

				if (numSelectedNodes > 0)
				{
					std::vector<NodeID> copyNodes(numSelectedNodes);

					ImNodes::GetSelectedNodes(copyNodes.data());

					Modify::CreateCopyBuffer(copyNodes, *GetCurrentContext().nodeGraph);
					//GetCurrentContext().script->GetModifier().CreateCopyBuffer(copyNodes);
				}
			}
			else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
			{

				ScriptVec2 mousePos = ScriptVec2{ GetMousePos().x, GetMousePos().y };

				Modify::PasteCopyBuffer(mousePos, *GetCurrentContext().nodeGraph, myCommandTracker.get());
				//GetCurrentContext().script->GetModifier().PasteCopyBuffer(mousePos);
			}

			bool& isDebug = myCommandTracker->IsDebugPrinting();

			ImGui::Checkbox("Debug Commands", &isDebug);

			if (isDebug)
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
		}

		ImGui::End();
	}

	void VisualScriptingWindow::ScriptSelectionMenu()
	{
		//Script& currentScript = *GetCurrentContext().script;

		/*ImGui::SetNextItemWidth(200);

		if (ImGui::BeginCombo("##ScriptSelectionCombo", currentScript.Name().c_str()))
		{
			for (size_t i = 0; i < myCurrentScriptManager->GetScripts().size(); ++i)
			{
				const Script& script = *myCurrentScriptManager->GetScripts()[i];
				bool isSelected = &currentScript == &script;

				if (ImGui::Selectable(script.Name().c_str(), isSelected))
				{
					myCurrentScript = &script;
					myCurrentIndex = i;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();*/

		char buffer[35]{};
		strcpy_s(buffer, GetCurrentContext().script->Name().c_str());

		if (ImGui::InputText("##", buffer, IM_ARRAYSIZE(buffer)))
		{
			GetCurrentContext().script->Name() = buffer;
		}
	}

	void VisualScriptingWindow::ScriptLoadingMenu()
	{
		Script& currentScript = *GetCurrentContext().script;

		bool canSave = myCommandTracker->GetUndoSize() == 0;

		ImGui::BeginDisabled(canSave);

		if (ImGui::Button("Save"))
		{
			ScriptLoader::Save(currentScript);
			myCommandTracker->Clear();
		}

		ImGui::EndDisabled();
		ImGui::SameLine();

		if (ImGui::Button("Reload All"))
		{
			ScriptLoader::LoadAll();
		}

		ImGui::SameLine();

		if (ImGui::Button("Create New Script"))
		{
			ImGui::OpenPopup("Create New Script");
		}

		if (ImGui::BeginPopupModal("Create New Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("##", myScriptNameText, IM_ARRAYSIZE(myScriptNameText), ImGuiInputTextFlags_AutoSelectAll);
			ImGui::Separator();

			/*auto it = std::find_if(myCurrentScriptManager->GetScripts().begin(), myCurrentScriptManager->GetScripts().end(), [&](const std::unique_ptr<Script>& aScript) -> bool
				{
					return aScript->Name() == std::string(myScriptNameText);
				});

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, it != myCurrentScriptManager->GetScripts().end());*/

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				Script& script = ScriptFoundation::GetInstance().CreateScript(0, myScriptNameText);
				//myCurrentScriptManager->CreateScript(myScriptNameText);
				myScriptNameText[0] = (char)0;

				myImNodesContexts.emplace(&script.GetEventGraph(), ImNodes::CreateContext());
				//myCurrentIndex = myCurrentScriptManager->GetScripts().size() - 1;


				ImGui::CloseCurrentPopup();
			}

			//ImGui::PopItemFlag();

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();
		ImGui::SameLine();

		if (ImGui::Button("Create Copy"))
		{
			ImGui::OpenPopup("Create Copy Script");
		}

		if (ImGui::BeginPopupModal("Create Copy Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("##", myScriptNameText, IM_ARRAYSIZE(myScriptNameText), ImGuiInputTextFlags_AutoSelectAll);
			ImGui::Separator();

			/*auto it = std::find_if(myCurrentScriptManager->GetScripts().begin(), myCurrentScriptManager->GetScripts().end(), [&](const std::unique_ptr<Script>& aScript) -> bool
				{
					return aScript->Name() == std::string(myScriptNameText);
				});

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, it != myCurrentScriptManager->GetScripts().end());*/

			/*if (ImGui::Button("Create Copy", ImVec2(120, 0)))
			{
				ScriptLoader::CreateCopy(*GetCurrentContext().script, myScriptNameText);
				ScriptLoader::LoadAll();

				for (size_t i = 0; i < myCurrentScriptManager->GetScripts().size(); i++)
				{
					if (myCurrentScriptManager->GetScripts()[i]->Name() == myScriptNameText)
					{
						myCurrentIndex = i;
					}
				}

				myScriptNameText[0] = (char)0;

				ImGui::CloseCurrentPopup();
			}*/

			//ImGui::PopItemFlag();
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
			ScriptLoader::SaveCustomEvents("Assets/VisualScripting");
		}

		static int currentEvent = 0;
		const char* events[] = { "Begin Play", "Tick", "End Play", };

		ImGui::Combo("Event Type", &currentEvent, events, IM_ARRAYSIZE(events));
		ImGui::SameLine();

		if (ImGui::Button("Trigger Event"))
		{
			ScriptInstance& scriptInstance = myCurrentScript->CreateScriptInstance();
			ExecutionContextBase c;
			scriptInstance.ExecuteEvent(static_cast<eNodeEventType>(currentEvent + 1), c);

			myCurrentScript->DestroyScriptInstance(scriptInstance);
			//GetCurrentContext().script->ExecuteEvent(static_cast<eNodeEventType>(currentEvent + 1), c);
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Instance"))
		{
			ScriptInstance& instance = GetCurrentContext().script->CreateScriptInstance();
			instance;
		}
	}

	void VisualScriptingWindow::VisualizeNodes()
	{
		Script& currentScript = *GetCurrentContext().script;
		const NodeManager& nodeManager = *GetCurrentContext().nodeGraph->myNodeManager;
		const VariableManager& variableManager = ScriptProxy::GetVariableManager(currentScript);

		ImNodes::BeginNodeEditor();

		// Render nodes
		for (auto& [nodeID, node, nodeType] : nodeManager)
		{
			if (node->isDestroyed)
			{
				continue;
			}

			ImNodesStyle& style = ImNodes::GetStyle();

			if (nodeType->nodeRecipe.eventID == EnumCast(eNodeEventType::None))
			{
				style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(Color{ 0.1f, 0.3f, 0.6f, 1.f });
				style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(Color{ 0.1f, 0.3f, 0.7f, 1.f });
				style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(Color{ 0.1f, 0.3f, 0.8f, 1.f });
			}
			else
			{
				style.Colors[ImNodesCol_TitleBar] = ToImGuiColor(Color{ 0.8f, 0.1f, 0.1f, 1.f });
				style.Colors[ImNodesCol_TitleBarHovered] = ToImGuiColor(Color{ 0.9f, 0.1f, 0.1f, 1.f });
				style.Colors[ImNodesCol_TitleBarSelected] = ToImGuiColor(Color{ 1.f, 0.1f, 0.1f, 1.f });
			}

			ImNodes::SetNodeGridSpacePos(nodeID, { node->position.x, node->position.y });
			ImNodes::BeginNode(nodeID);

			float nodeNameWidth = 0;

			{ // Node title bar

				ImNodes::BeginNodeTitleBar();

				std::string nodeName = !HasFlag(nodeType->nodeRecipe.traits, eNodeTrait::Accessor)
					? NodeTypeManager::GetInstance().GetShortName(node->typeID)
					: HasFlag(nodeType->nodeRecipe.traits, eNodeTrait::Getter)
					? "Get " + ScriptProxy::GetVariable(currentScript, variableManager.GetVariableIDByNodeID(nodeID)).name
					: "Set " + ScriptProxy::GetVariable(currentScript, variableManager.GetVariableIDByNodeID(nodeID)).name;

				std::string nodeLabel = nodeName;
				ImGui::TextUnformatted(nodeLabel.c_str());

				nodeNameWidth = ImGui::CalcTextSize(nodeLabel.c_str()).x;

				ImNodes::EndNodeTitleBar();
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();

			float nodeWidthLeft = node->inputPins.empty() ? 0.f : 100.f;

			for (int i = 0; i < node->inputPins.size(); ++i)
			{
				const PinID pinID = node->inputPins[i];

				std::string pinLabel = GetPinLabel(PinTypeManager::GetPinType(ScriptProxy::GetPin(*GetCurrentContext().nodeGraph, pinID).typeID));
				const float labelWidth = ImGui::CalcTextSize(pinLabel.c_str()).x;

				nodeWidthLeft = std::max(nodeWidthLeft, labelWidth);
			}

			float nodeWidthRight = 0.f;

			for (int i = 0; i < node->outputPins.size(); ++i)
			{
				const PinID pinID = node->outputPins[i];
				const std::string& pinLabel = GetPinLabel(PinTypeManager::GetPinType(ScriptProxy::GetPin(*GetCurrentContext().nodeGraph, pinID).typeID));
				const float labelWidth = ImGui::CalcTextSize(pinLabel.c_str()).x;

				nodeWidthRight = std::max(nodeWidthRight, labelWidth);
			}

			float extraWidth = 0.f;

			if (nodeWidthLeft > 0.f && nodeWidthRight > 0.f)
			{
				extraWidth += 30.f;
			}

			// Render input pins
			for (int i = 0; i < node->inputPins.size(); ++i)
			{
				const PinID pinID = node->inputPins[i];
				const Pin& pin = ScriptProxy::GetPin(*GetCurrentContext().nodeGraph, pinID);
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(Global::GetDataTypeManager().GetColor(pinType.dataTypeID)));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(Global::GetDataTypeManager().GetHoverColor(pinType.dataTypeID)));

				bool shouldBeHighlighted = std::find(myPinIDsToHighlight.begin(), myPinIDsToHighlight.end(), pinID) != myPinIDsToHighlight.end();

				ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;
				ImNodes::BeginInputAttribute(pinID, shape);

				std::string pinLabel = GetPinLabel(pinType);
				if (!pinLabel.empty())
				{
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				if (pin.connectedPinIDs.empty())
				{
					const float itemWidth = std::max(20.f, nodeWidthLeft);
					ImGui::PushItemWidth(itemWidth);

					Modify::EditPin(pinID, *GetCurrentContext().nodeGraph, myCommandTracker.get());

					ImGui::PopItemWidth();
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}

			ImGui::SetCursorPos(cursorPos);

			// Render output pins
			for (int i = 0; i < node->outputPins.size(); ++i)
			{
				const PinID pinID = node->outputPins[i];
				const Pin& pin = ScriptProxy::GetPin(*GetCurrentContext().nodeGraph, pinID);
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(Global::GetDataTypeManager().GetColor(pinType.dataTypeID)));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(Global::GetDataTypeManager().GetHoverColor(pinType.dataTypeID)));

				bool shouldBeHighlighted = std::find(myPinIDsToHighlight.begin(), myPinIDsToHighlight.end(), pinID) != myPinIDsToHighlight.end();

				ImNodesPinShape shape = shouldBeHighlighted ? ImNodesPinShape_Triangle : ImNodesPinShape_CircleFilled;

				ImNodes::BeginOutputAttribute(pinID, shape);

				std::string pinLabel = GetPinLabel(PinTypeManager::GetPinType(pin.typeID));

				if (!pinLabel.empty())
				{
					const float plusWidth = std::max(nodeWidthLeft + extraWidth, nodeNameWidth - nodeWidthRight);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + plusWidth + extraWidth + nodeWidthRight - ImGui::CalcTextSize(pinLabel.c_str()).x);

					ImGui::TextUnformatted(pinLabel.c_str());
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}

			ImNodes::EndNode();
		}

		// Render links
		for (PinID inputPinID : ScriptFilter::GetInputPins(*GetCurrentContext().nodeGraph))
		{
			const Pin& pin = ScriptProxy::GetPin(ScriptProxy::GetEventGraph(currentScript), inputPinID);
			const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

			if (!pin.connectedPinIDs.empty())
			{

				ImNodes::PushColorStyle(ImNodesCol_Link, ToImGuiColor(Global::GetDataTypeManager().GetColor(pinType.dataTypeID)));
				ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ToImGuiColor(Global::GetDataTypeManager().GetSelectionColor(pinType.dataTypeID)));
				ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ToImGuiColor(Global::GetDataTypeManager().GetHoverColor(pinType.dataTypeID)));

				ImNodes::Link(inputPinID, inputPinID, pin.connectedPinIDs[0]);

				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}
		}

		ImNodes::MiniMap(.2f, ImNodesMiniMapLocation_BottomRight);
		ImNodes::EndNodeEditor();
	}

	void VisualScriptingWindow::UpdateNodes()
	{
		Script& currentScript = *GetCurrentContext().script;
		const NodeManager& nodeManager = *GetCurrentContext().nodeGraph->myNodeManager;
		//ScriptModifier& modifier = currentScript.GetModifier();

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

		std::vector<NodeDragData> endNodeDragData;

		// Update node positions
		for (auto& [nodeID, node, nodeType] : nodeManager)
		{
			if (node->isDestroyed)
			{
				continue;
			}

			ScriptVec2 newPos = ToScriptVec2(ImNodes::GetNodeGridSpacePos(nodeID));
			ScriptVec2 oldPos = node->position;
			if (dragStarted && ImNodes::IsNodeSelected(nodeID))
			{
				//Modify::BeginNodeDrag(nodeID, oldPos);
			}
			else if (dragEnded)
			{
				endNodeDragData.emplace_back(nodeID, newPos);
			}
			if (newPos != oldPos)
			{
				Modify::SetNodePosition(nodeID, { newPos.x, newPos.y }, *GetCurrentContext().nodeGraph, myCommandTracker.get());
			}
		}

		Modify::CommitNodeDrag(endNodeDragData, *GetCurrentContext().nodeGraph, myCommandTracker.get());

		// See if links should be created

		PinID createdLinkPinID1 = InvalidID<PinID>();
		PinID createdLinkPinID2 = InvalidID<PinID>();

		if (ImNodes::IsLinkCreated(&createdLinkPinID1, &createdLinkPinID2))
		{
			Modify::TryCreateLink(createdLinkPinID1, createdLinkPinID2, *GetCurrentContext().nodeGraph, myCommandTracker.get());
			myPinIDsToHighlight.clear();
		}

		// See if links or nodes should be destroyed

		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			std::vector<PinID> selectedLinks(ImNodes::NumSelectedLinks());
			std::vector<NodeID> selectedNodes(ImNodes::NumSelectedNodes());

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
				Modify::DestroySelection(selectedNodes, selectedLinks, *GetCurrentContext().nodeGraph, myCommandTracker.get());
			}

			ImNodes::ClearNodeSelection();
			ImNodes::ClearLinkSelection();
		}

		PinID startedPinID = InvalidID<PinID>();
		if (ImNodes::IsLinkStarted(&startedPinID))
		{
			myStartedLinkPinID = startedPinID;

			const Pin& pin = ScriptProxy::GetPin(ScriptProxy::GetEventGraph(currentScript), myStartedLinkPinID);

			const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);
			myPinIDsToHighlight = ScriptFilter::GetNonConnectedPinsOfTypeAndHash(*GetCurrentContext().nodeGraph, InvertPinType(pinType.flowType), pinType.dataTypeID);

			for (PinID i = 0; i < myPinIDsToHighlight.size(); i++)
			{
				if (ScriptProxy::GetPin(ScriptProxy::GetEventGraph(currentScript), myPinIDsToHighlight[i]).nodeID == pin.nodeID)
				{
					myPinIDsToHighlight.erase(myPinIDsToHighlight.begin() + i);
					i--;
				}
			}
		}

		PinID droppedPinID = InvalidID<PinID>();

		if (ImNodes::IsLinkDropped(&droppedPinID))

		{
			ImGui::OpenPopup("Node Create Popup");
			myLinkCreationPinID = droppedPinID;
			myPinIDsToHighlight.clear();

			UpdateClickPos();
		}

		// Drop link create popup
		if (ImGui::BeginPopup("Node Create Popup"))
		{
			const Pin& pin = ScriptProxy::GetPin(ScriptProxy::GetEventGraph(currentScript), myLinkCreationPinID);

			DataTypeID droppedPinDataTypeID = PinTypeManager::GetPinType(pin.typeID).dataTypeID;

			auto nodeTypePopulationFunc = [&](NodeTypeCategory& aMainCategory) -> void
				{
					const std::vector<NodeType>& nodeTypes = NodeTypeManager::GetInstance().GetNodeTypes();

					const std::vector<NodeTypeID> filtered = IndexStream<NodeType, NodeTypeID>(nodeTypes, [](NodeTypeID anID) -> const NodeType& { return NodeTypeManager::GetInstance().GetNodeType(anID); })
						.Filter([&](const NodeType& aNodeType) -> bool
							{
								size_t size = 0;
								switch (PinTypeManager::GetPinType(pin.typeID).flowType)
								{
								case ePinFlowType::Input:
								{
									size = Stream<PinTypeID>(aNodeType.nodeRecipe.outputPinTypeIDs).Filter([&droppedPinDataTypeID](const PinTypeID aPinTypeID) -> bool
										{
											return PinTypeManager::GetPinType(aPinTypeID).dataTypeID == droppedPinDataTypeID;
										}
									).Size();
									break;
								}
								case ePinFlowType::Output:
								{
									size = Stream<PinTypeID>(aNodeType.nodeRecipe.inputPinTypeIDs).Filter([&droppedPinDataTypeID](const PinTypeID aPinTypeID) -> bool
										{
											return PinTypeManager::GetPinType(aPinTypeID).dataTypeID == droppedPinDataTypeID;
										}
									).Size();
									break;
								}
								default:
									break;
								}

								return size > 0;
							}
						).Get();

					for (NodeTypeID nodeTypeID : filtered)
					{
						PopulateCategories(NodeTypeManager::GetInstance().GetFullName(nodeTypeID), nodeTypeID, aMainCategory);

					}
				};

			auto onClickCallback = [&](NodeTypeID aNodeTypeID) -> void
				{

					Modify::CreateNodeAutoLink(*GetCurrentContext().nodeGraph, aNodeTypeID, myLinkCreationPinID, ScriptVec2{ myNodeCreationClickPos.x, myNodeCreationClickPos.y }, myCommandTracker.get());


					myPinIDsToHighlight.clear();
					myNodeTypeSearch[0] = '\0';
				};

			ShowNodeCreationMenu(nodeTypePopulationFunc, onClickCallback);

			ImGui::EndPopup();
		}
	}

	void VisualScriptingWindow::PopulateCategories(const std::string& aName, const size_t aNodeTypeID, NodeTypeCategory& aCategory)
	{
		std::string categoryName = aName.substr(0, aName.find_first_of('/'));
		std::string rest = aName.substr(categoryName.length(), aName.length() - 1);

		if (rest.empty())
		{
			aCategory.nodeTypesIDs.push_back(aNodeTypeID);
			return;
		}

		rest = rest.substr(1, rest.length());

		bool hasFoundCategory = false;
		for (NodeTypeCategory& childCategory : aCategory.childCategories)
		{
			if (childCategory.name == categoryName)
			{
				PopulateCategories(rest, aNodeTypeID, childCategory);
				hasFoundCategory = true;
				break;
			}
		}

		if (!hasFoundCategory)
		{
			aCategory.childCategories.emplace_back(categoryName);
			PopulateCategories(rest, aNodeTypeID, aCategory.childCategories.back());
		}
	}

	void VisualScriptingWindow::ShowNodeTypeCreationMenu(const std::vector<NodeTypeID>& aNodeTypeIDs, const std::function<void(NodeTypeID)>& aOnClickFunc)
	{
		for (const NodeTypeID nodeTypeID : aNodeTypeIDs)
		{
			if (ImGui::MenuItem(NodeTypeManager::GetInstance().GetShortName(nodeTypeID).c_str()))
			{
				aOnClickFunc(nodeTypeID);
			}
		}
	}

	void VisualScriptingWindow::ShowNodeCreationMenuByCategory(const NodeTypeCategory& aCategory, const std::function<void(NodeTypeID)>& aOnClickFunc)
	{
		if (ImGui::BeginMenu(aCategory.name.c_str()))
		{

			for (const NodeTypeCategory& childCategory : aCategory.childCategories)
			{
				ShowNodeCreationMenuByCategory(childCategory, aOnClickFunc);
			}

			ShowNodeTypeCreationMenu(aCategory.nodeTypesIDs, aOnClickFunc);

			ImGui::EndMenu();
		}
	}

	void VisualScriptingWindow::ShowNodeCreationMenu(const std::function<void(NodeTypeCategory&)>& aCategoryFunction, const std::function<void(NodeTypeID)>& aOnClickFunction)
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
			ShowNodeTypeCreationMenu(mainCategory.nodeTypesIDs, aOnClickFunction);
		}
	}

	void VisualScriptingWindow::NodeCreation()
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
					const std::vector<NodeType>& nodeTypes = NodeTypeManager::GetInstance().GetNodeTypes();

					if (myNodeTypeSearch[0] == '\0')
					{

						std::vector<NodeTypeID> nodeTypeIDs = IndexStream<NodeType>(nodeTypes, [](const NodeTypeID anID) -> const NodeType& { return NodeTypeManager::GetInstance().GetNodeType(anID); })
							.Filter([](const NodeType& aNodeType) -> bool
								{
									if (HasFlag(aNodeType.nodeRecipe.traits, eNodeTrait::NonTrivial))
									{
										return false;
									}
									return true;
								}
							).Get();
						for (NodeTypeID nodeTypeID : nodeTypeIDs)
						{
							PopulateCategories(NodeTypeManager::GetInstance().GetFullName(nodeTypeID), nodeTypeID, aMainCategory);
						}
					}
					else
					{
						for (NodeTypeID i = 0; i < nodeTypes.size(); ++i)
						{
							const NodeType& nodeType = nodeTypes[i];

							if (HasFlag(nodeType.nodeRecipe.traits, eNodeTrait::NonTrivial))
							{
								continue;
							}

							bool isSearched = std::strstr(nodeType.name.c_str(), myNodeTypeSearch);
							if (isSearched)
							{
								aMainCategory.nodeTypesIDs.push_back(i);
							}
						}
					}
				};

			auto onClickCallback = [&](NodeTypeID aNodeTypeID) -> void
				{

					Modify::CreateNode(*GetCurrentContext().nodeGraph, aNodeTypeID, ScriptVec2{ myNodeCreationClickPos.x, myNodeCreationClickPos.y }, myCommandTracker.get());


					myPinIDsToHighlight.clear();
					myNodeTypeSearch[0] = '\0';
				};

			ShowNodeCreationMenu(categoryFunc, onClickCallback);
			ImGui::EndPopup();
		}
	}

	ImVec2 VisualScriptingWindow::GetMiddlePos() const
	{
		return GetCurrentContext().imNodesContext->CanvasOriginScreenSpace + ImNodes::EditorContextGetPanning() / 2.f;
	}


	void VisualScriptingWindow::UpdateClickPos()
	{
		myNodeCreationClickPos = GetMousePos();
	}

	ImVec2 VisualScriptingWindow::GetMousePos() const
	{
		return ImGui::GetMousePosOnOpeningCurrentPopup() - GetCurrentContext().imNodesContext->CanvasOriginScreenSpace - ImNodes::EditorContextGetPanning();
	}
}