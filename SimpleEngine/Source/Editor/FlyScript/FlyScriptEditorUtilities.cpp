#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "NodeScript/Fly.hpp"
#include "NodeScript/Command/FlyCommandTracker.hpp"
#include <imnodes/imnodes.h>
#include <imnodes/imnodes_internal.h>


namespace Editor
{

	static bool DataTypeCombo(const char* aComboLabel, Fly::GenericDataTypeProxy& aDataTypeProxy, auto aFilterPredicate)
	{
		bool wasSelected = false;
		if (ImGui::BeginCombo(aComboLabel, aDataTypeProxy.GetName().c_str()))
		{
			const std::vector<Fly::GenericDataTypeProxy> dataTypes = Fly::GetGenericDataTypesFiltered(aFilterPredicate);

			for (const Fly::GenericDataTypeProxy& dataType : dataTypes)
			{
				if (ImGui::Selectable(dataType.GetName().c_str(), aDataTypeProxy == dataType))
				{
					aDataTypeProxy = dataType;
					wasSelected = true;
				}
			}

			ImGui::EndCombo();
		}
		return wasSelected;
	}

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::GenericDataTypeProxy& aDataTypeProxy)
	{
		return DataTypeCombo(aComboLabel, aDataTypeProxy, [](const Fly::GenericDataTypeProxy& aDataType) -> bool { return aDataType.IsViewAndEditable(); });
	}

	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::GenericDataTypeProxy& aDataTypeProxy)
	{
		return DataTypeCombo(aComboLabel, aDataTypeProxy, [](const Fly::GenericDataTypeProxy& aDataType) -> bool { return aDataType.IsTargetable(); });
	}

	bool DataTypeComboNoFilter(const char* aComboLabel, Fly::GenericDataTypeProxy& aDataTypeProxy)
	{
		return DataTypeCombo(aComboLabel, aDataTypeProxy, [](const Fly::GenericDataTypeProxy&) -> bool { return true; });
	}

	void UpdateClickPos(NodeGraphContext& aNodeGraphContext)
	{
		aNodeGraphContext.myNodeCreationClickPos = GetMousePos(aNodeGraphContext);
	}

	ImVec2 GetMousePos(const NodeGraphContext& aNodeGraphContext)
	{
		return ImGui::GetMousePosOnOpeningCurrentPopup() - aNodeGraphContext.myImNodesContext->CanvasOriginScreenSpace - ImNodes::EditorContextGetPanning();
	}

	void ShowNodeGraph(NodeGraphContext& aNodeGraphContext)
	{
		ImNodes::SetCurrentContext(aNodeGraphContext.myImNodesContext);
		VisualizeNodeGraph(aNodeGraphContext);
		UpdateNodeGraph(aNodeGraphContext);
		NodeCreation(aNodeGraphContext);
	}

	void VisualizeNodeGraph(NodeGraphContext& aNodeGraphContext)
	{
		
		const Fly::NodeGraphProxy& nodeGraphProxy = aNodeGraphContext.myNodeGraphProxy;
		ImNodes::BeginNodeEditor();

		for (const auto& node : nodeGraphProxy.IterateNodes())
		{

			ImNodesStyle& style = ImNodes::GetStyle();

			if (node.GetEventID() == Fly::InvalidID<Fly::EventID>())
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

			style.Colors[ImNodesCol_NodeOutline] = ImNodes::IsNodeSelected(node.GetID()) ? ToImGuiColor(Fly::Color(0.8f, 0.8f, 0.8f, 1.f)) : ToImGuiColor(Fly::Color(0.f, 0.f, 0.f, 1.f));

			ImNodes::SetNodeGridSpacePos(node.GetID(), ImVec2{ node.GetPosition().x, node.GetPosition().y });
			ImNodes::BeginNode(node.GetID());

			float nodeNameWidth = 0;

			{ // Node title bar

				ImNodes::BeginNodeTitleBar();

				std::string nodeLabel;

				if (node.IsAccessor())
				{
					/*const Fly::VariableProxy variable = nodeProxy.GetVariableProxy();
					const bool isGetter = Fly::HasFlag(nodeProxy.GetTraits(), Fly::eNodeTrait::Getter);
					const char* const prefixLabel = isGetter ? "Get " : "Set ";
					nodeLabel = prefixLabel + variable.GetName();*/
				}
				else
				{
					nodeLabel = node.GetNodeType().GetShortName();
				}

				if (Fly::IsDebugging())
				{
					nodeLabel += ", ID: " + std::to_string(node.GetID());
				}
				ImGui::TextUnformatted(nodeLabel.c_str());

				nodeNameWidth = ImGui::CalcTextSize(nodeLabel.c_str()).x;

				ImNodes::EndNodeTitleBar();
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();

			std::vector<Fly::PinProxy> inputPinProxys = node.GetSplitInputPins();
			float nodeWidthLeft = inputPinProxys.empty() ? 0.f : 100.f;

			for (const Fly::PinProxy& inputPinProxy : inputPinProxys)
			{
				const std::string_view pinLabel = inputPinProxy.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(std::string(pinLabel).c_str()).x;

				nodeWidthLeft = std::max(nodeWidthLeft, labelWidth);
			}

			float nodeWidthRight = 0.f;

			std::vector<Fly::PinProxy> outputPins = node.GetSplitOutputPins();

			for (const Fly::PinProxy& outputPinProxy : outputPins)
			{
				const std::string_view pinLabel = outputPinProxy.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(std::string(pinLabel).c_str()).x;

				nodeWidthRight = std::max(nodeWidthRight, labelWidth);
			}

			float extraWidth = 0.f;

			if (nodeWidthLeft > 0.f && nodeWidthRight > 0.f)
			{
				extraWidth += 30.f;
			}

			// Render input pins
			for (Fly::PinProxy& inputPinProxy : inputPinProxys)
			{
				const Fly::GenericDataTypeProxy pinDataType(inputPinProxy.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor()/* - myHoverTint*/));

				const bool shouldBeHighlighted = false;// std::find(currentNodeContext.myPinProxysToHighlight.begin(), currentNodeContext.myPinProxysToHighlight.end(), inputPinProxy) != currentNodeContext.myPinProxysToHighlight.end();


				const ImNodesPinShape shape = [shouldBeHighlighted, inputPinProxy]()
					{
						if (shouldBeHighlighted)
						{
							return ImNodesPinShape_Triangle;
						}

						const bool isPointer = Fly::GenericDataTypeProxy(inputPinProxy.GetDataTypeID()).IsPointer();
						return isPointer ? ImNodesPinShape_QuadFilled : ImNodesPinShape_CircleFilled;
					}();
				ImNodes::BeginInputAttribute(inputPinProxy.GetID(), shape);

				std::string pinLabel(inputPinProxy.GetPinTypeName());
				if (!pinLabel.empty())
				{
					if (Fly::IsDebugging())
					{
						pinLabel += ", " + std::to_string(inputPinProxy.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				if (inputPinProxy.GetConnectedPinIDs().empty())
				{
					const float itemWidth = std::max(20.f, nodeWidthLeft);
					ImGui::PushItemWidth(itemWidth);

					inputPinProxy.ViewAndEdit(aNodeGraphContext.myCommandTracker.get());

					ImGui::PopItemWidth();
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}


			ImGui::SetCursorPos(cursorPos);

			// Render output pins
			for (const Fly::PinProxy& outputPinProxy : outputPins)
			{
				const Fly::GenericDataTypeProxy pinDataType(outputPinProxy.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor() - NodeGraphContext::HOVER_TINT));

				const bool shouldBeHighlighted = std::find(aNodeGraphContext.myPinProxysToHighlight.begin(), aNodeGraphContext.myPinProxysToHighlight.end(), outputPinProxy) != aNodeGraphContext.myPinProxysToHighlight.end();

				const ImNodesPinShape shape = [shouldBeHighlighted, outputPinProxy]()
					{
						if (shouldBeHighlighted)
						{
							return ImNodesPinShape_Triangle;
						}

						const bool isPointer = Fly::GenericDataTypeProxy(outputPinProxy.GetDataTypeID()).IsPointer();
						return isPointer ? ImNodesPinShape_QuadFilled : ImNodesPinShape_CircleFilled;
					}();

				ImNodes::BeginOutputAttribute(outputPinProxy.GetID(), shape);

				std::string pinLabel(outputPinProxy.GetPinTypeName());

				if (!pinLabel.empty())
				{
					const float plusWidth = std::max(nodeWidthLeft + extraWidth, nodeNameWidth - nodeWidthRight);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + plusWidth + extraWidth + nodeWidthRight - ImGui::CalcTextSize(pinLabel.c_str()).x);

					if (Fly::IsDebugging())
					{
						pinLabel += ", " + std::to_string(outputPinProxy.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				ImNodes::EndInputAttribute();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}

			ImNodes::EndNode();
		}

		for (const Fly::LinkProxy& link : nodeGraphProxy.IterateLinks())
		{
			if (link.IsDestroyed())
			{
				continue;
			}
			const Fly::PinProxy pinProxy = link.GetInputPin();
			const Fly::GenericDataTypeProxy pinDataType(pinProxy.GetDataTypeID());

			const Fly::Color dataTypeColor = pinDataType.GetColor();
			Fly::Color linkColor = dataTypeColor;
			if (std::find(aNodeGraphContext.myTraversedLinks.begin(), aNodeGraphContext.myTraversedLinks.end(), link) != aNodeGraphContext.myTraversedLinks.end())
			{
				linkColor = NodeGraphContext::TRAVERSED_LINK_COLOR;
			}
			ImNodes::PushColorStyle(ImNodesCol_Link, ToImGuiColor(linkColor));
			ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ToImGuiColor(linkColor - NodeGraphContext::SELECTION_TINT));
			ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ToImGuiColor(linkColor - NodeGraphContext::HOVER_TINT));

			ImNodes::Link(link.GetID(), link.GetInputPin().GetID(), link.GetOutputPin().GetID());

			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
		}


		aNodeGraphContext.myIsNodeEditorHovered = ImNodes::IsEditorHovered();

		ImNodes::MiniMap(.2f, ImNodesMiniMapLocation_BottomRight);
		ImNodes::EndNodeEditor();
	}


	void UpdateNodeGraph(NodeGraphContext& aNodeGraphContext)
	{
		Fly::NodeGraphProxy& nodeGraphProxy = aNodeGraphContext.myNodeGraphProxy;
		bool dragStarted = ImGui::IsKeyDown(ImGuiKey_MouseLeft) && !aNodeGraphContext.myIsDraggingNode;
		if (dragStarted)
		{
			if (ImNodes::NumSelectedNodes() > 0) // Drag node started
			{
				aNodeGraphContext.myIsDraggingNode = true;
			}
		}

		bool dragEnded = ImGui::IsKeyReleased(ImGuiKey_MouseLeft) && aNodeGraphContext.myIsDraggingNode;
		if (dragEnded)
		{
			aNodeGraphContext.myIsDraggingNode = false;
		}

		for (Fly::NodeProxy node : nodeGraphProxy.IterateNodes())
		{

			const Fly::Vec2 newPos = ToFlyVec2(ImNodes::GetNodeGridSpacePos(node.GetID()));
			Fly::Vec2 oldPos = node.GetPosition();
			if (dragStarted && ImNodes::IsNodeSelected(node.GetID()))
			{
				aNodeGraphContext.myNodeDragData.emplace(node.GetID(), Fly::NodeDragData{ .mStartPos = oldPos });
			}
			else if (dragEnded)
			{
				auto it = aNodeGraphContext.myNodeDragData.find(node.GetID());

				if (it != aNodeGraphContext.myNodeDragData.end())
				{
					it->second.mEndPos = newPos;
				}
			}
			if (newPos != oldPos)
			{
				node.SetPosition(newPos);
			}
		}

		if (dragEnded)
		{
			aNodeGraphContext.myNodeGraphProxy.CommitNodeDrag(aNodeGraphContext.myNodeDragData, aNodeGraphContext.myCommandTracker.get());
			aNodeGraphContext.myNodeDragData.clear();
		}

		Fly::NodeID hoveredNodeID = Fly::InvalidID<Fly::NodeID>();
		if (ImNodes::IsNodeHovered(&hoveredNodeID.mID) && ImGui::IsKeyPressed(ImGuiKey_MouseRight))
		{
			aNodeGraphContext.myClickedNodeProxy = Fly::NodeProxy(hoveredNodeID, aNodeGraphContext.myNodeGraphProxy);

			ImGui::OpenPopup("Node Popup");
		}

		if (ImGui::BeginPopup("Node Popup"))
		{
			ImGui::Text("Node Options");

			ImGui::Separator();
			if (ImGui::Selectable("Destroy Node"))
			{
				aNodeGraphContext.myClickedNodeProxy.Destroy(aNodeGraphContext.myCommandTracker.get());
				ImGui::CloseCurrentPopup();
			}

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedNodeProxy.HasAnyConnectedLinks());
			if (ImGui::Selectable("Destroy Links"))
			{
				aNodeGraphContext.myClickedNodeProxy.DestroyConnectedLinks(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::Separator();

			if (aNodeGraphContext.myClickedNodeProxy.IsReplacable() && ImGui::BeginMenu("Data Type"))
			{
				const std::vector<Fly::DataTypeProxy> replacableDataTypeProxys = aNodeGraphContext.myClickedNodeProxy.GetNodeType().GetReplacableDataTypes();
				for (const Fly::DataTypeProxy& replacableDataTypeProxy : replacableDataTypeProxys)
				{
					ImGui::PushID(static_cast<int>(replacableDataTypeProxy.GetID()));
					if (ImGui::Selectable(replacableDataTypeProxy.GetName().c_str()))
					{
						aNodeGraphContext.myNodeGraphProxy.ReplaceTemplateNode(aNodeGraphContext.myClickedNodeProxy, replacableDataTypeProxy, aNodeGraphContext.myCommandTracker.get());
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
			aNodeGraphContext.myClickedNodeProxy = {};
		}

		Fly::PinID hoveredPinID;
		if (ImNodes::IsPinHovered(&hoveredPinID.mID) && ImGui::IsKeyPressed(ImGuiKey_MouseRight))
		{
			aNodeGraphContext.myClickedPinProxy = Fly::PinProxy(hoveredPinID, aNodeGraphContext.myNodeGraphProxy);

			ImGui::OpenPopup("Pin Popup");

		}

		if (ImGui::BeginPopup("Pin Popup"))
		{

			ImGui::Text("Pin Options");

			ImGui::Separator();

			Fly::GenericDataTypeProxy pinDataType(aNodeGraphContext.myClickedPinProxy.GetDataTypeID());
			ImGui::Text("Type: %s", pinDataType.GetName().c_str());
			

			if (aNodeGraphContext.myClickedPinProxy.GetFlowType() == Fly::eFlowType::Input)
			{
				aNodeGraphContext.myClickedPinProxy.ViewAndEdit(aNodeGraphContext.myCommandTracker.get());
			}
			else
			{
				aNodeGraphContext.myClickedPinProxy.View();
			}

			ImGui::Separator();

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedPinProxy.IsSplitable());
			if (ImGui::Selectable("Split Pin"))
			{
				aNodeGraphContext.myClickedPinProxy.Split(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedPinProxy.IsRecombinable());
			if (ImGui::Selectable("Recombine Pin"))
			{
				aNodeGraphContext.myClickedPinProxy.RecombineParentPin(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedPinProxy.HasAnyConnectedLinks());
			if (ImGui::Selectable("Destroy Links"))
			{
				aNodeGraphContext.myClickedPinProxy.DestroyConnectedLinks(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::Separator();

			if (aNodeGraphContext.myClickedPinProxy.IsParentNodeReplacable() && ImGui::BeginMenu("Choose Type"))
			{
				const Fly::GenericDataTypeID d = aNodeGraphContext.myClickedPinProxy.GetDataTypeID();
				const Fly::DataTypeID* clickedPinDataTypeID = std::get_if<Fly::DataTypeID>(&d.mID);
				if (clickedPinDataTypeID)
				{
					const std::vector<Fly::DataTypeProxy> replacableDataTypes = Fly::GetNodeOwner(aNodeGraphContext.myClickedPinProxy).GetNodeType().GetReplacableDataTypes();
					for (const Fly::DataTypeProxy& replacableDataType : replacableDataTypes)
					{
						ImGui::PushID(static_cast<int>(replacableDataType.GetID()));
						if (ImGui::Selectable(replacableDataType.GetName().c_str(), replacableDataType.GetID() == *clickedPinDataTypeID))
						{
							aNodeGraphContext.myNodeGraphProxy.ReplaceTemplateNode(aNodeGraphContext.myClickedPinProxy, replacableDataType, aNodeGraphContext.myCommandTracker.get());
							ImGui::CloseCurrentPopup();
						}
						ImGui::PopID();
					}
				}


				ImGui::EndMenu();
			}


			ImGui::EndPopup();
		}
		else
		{
			aNodeGraphContext.myClickedPinProxy = Fly::PinProxy();
		}


		// See if links should be created

		Fly::PinID createdLinkPinID1;
		Fly::PinID createdLinkPinID2;

		if (ImNodes::IsLinkCreated(&createdLinkPinID1.mID, &createdLinkPinID2.mID))
		{
			aNodeGraphContext.myNodeGraphProxy.TryCreateLink(Fly::PinProxy(createdLinkPinID1, aNodeGraphContext.myNodeGraphProxy), Fly::PinProxy(createdLinkPinID2, aNodeGraphContext.myNodeGraphProxy), aNodeGraphContext.myCommandTracker.get());
			aNodeGraphContext.myPinProxysToHighlight.clear();
		}

		// See if links or nodes should be destroyed

		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			std::vector<Fly::LinkID> selectedLinks(ImNodes::NumSelectedLinks());
			std::vector<Fly::NodeID> selectedNodes(ImNodes::NumSelectedNodes());

			if (!selectedLinks.empty())
			{
				ImNodes::GetSelectedLinks(reinterpret_cast<int*>(selectedLinks.data()));
			}

			if (!selectedNodes.empty())
			{
				ImNodes::GetSelectedNodes(reinterpret_cast<int*>(selectedNodes.data()));
			}

			if (!selectedLinks.empty() || !selectedNodes.empty())
			{
				aNodeGraphContext.myNodeGraphProxy.DestroySelection(selectedNodes, selectedLinks, aNodeGraphContext.myCommandTracker.get());
			}

			ImNodes::ClearNodeSelection();
			ImNodes::ClearLinkSelection();
		}

		// Highlight pins
		{
			Fly::PinID startedPinID;
			if (ImNodes::IsLinkStarted(&startedPinID.mID))
			{
				aNodeGraphContext.myStartedLinkPinID = startedPinID;

				const Fly::PinProxy startedPin(startedPinID, aNodeGraphContext.myNodeGraphProxy);

				aNodeGraphContext.myPinProxysToHighlight = startedPin.GetPotentialConnections();

				std::erase_if(aNodeGraphContext.myPinProxysToHighlight,
					[&](const Fly::PinProxy& aPinProxy)-> bool
					{
						return aPinProxy.GetID() == startedPin.GetID();
					}
				);
			}
		}

		// Dropped link
		Fly::PinID droppedPinID = Fly::InvalidID<Fly::PinID>();

		if (ImNodes::IsLinkDropped(&droppedPinID.mID))
		{
			ImGui::OpenPopup(NodeGraphContext::NODE_SEARCH_POPUP_NAME);
			aNodeGraphContext.myLinkCreationPinID = droppedPinID;
			aNodeGraphContext.myPinProxysToHighlight.clear();
			aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] = '\0';

			UpdateClickPos(aNodeGraphContext);

			auto categoryFunc = [&aNodeGraphContext](NodeTypeCategory& aMainCategory) -> void
				{
					const Fly::PinProxy pinProxy(aNodeGraphContext.myLinkCreationPinID, aNodeGraphContext.myNodeGraphProxy);

					auto nodeTypePredicate = [pinProxy](const Fly::NodeTypeProxy& aNodeType) 
						{ 
							const auto& pinTypes = Fly::SelectByFlowType(Fly::InvertFlowType(pinProxy.GetFlowType()), aNodeType.GetInputPinTypes(), aNodeType.GetOutputPinTypes());
							for (const Fly::PinTypeProxy pinType : pinTypes)
							{
								if (pinType.GetDataTypeID() == pinProxy.GetDataTypeID())
								{
									return true;
								}
							}
							return false;
						};

					if (aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] == '\0')
					{
						for (const Fly::NodeTypeProxy& nodeType : Fly::IterateNodeTypes(nodeTypePredicate))
						{
							PopulateNodeCategories(nodeType.GetName(), nodeType, aMainCategory);
						}
					}
					else
					{
						for (const Fly::NodeTypeProxy& nodeType : Fly::IterateNodeTypes(nodeTypePredicate))
						{
							const bool isSearched = SearchString(nodeType.GetName(), std::string_view(aNodeGraphContext.mySearchNodeData.myNodeTypeSearch));
							if (isSearched)
							{
								aMainCategory.myNodeTypes.push_back(nodeType);
							}
						}
					}

				};

			auto onClickCallback = [&aNodeGraphContext](const Fly::NodeTypeProxy& aNodeTypeProxy) -> void
				{
					aNodeGraphContext.myNodeGraphProxy.CreateNodeAutoLink(aNodeTypeProxy, aNodeGraphContext.myLinkCreationPinID, Fly::Vec2{ aNodeGraphContext.myNodeCreationClickPos.x,aNodeGraphContext.myNodeCreationClickPos.y }, aNodeGraphContext.myCommandTracker.get());

					aNodeGraphContext.myPinProxysToHighlight.clear();
					aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] = '\0';
				};

			aNodeGraphContext.mySearchNodeData.myCategoryFunction = categoryFunc;
			aNodeGraphContext.mySearchNodeData.myOnClickFunction = onClickCallback;
		}

		if (Fly::IsDebugging())
		{

			if (ImGui::Begin("Debug Data"))
			{

				ImGui::Text("Links:");

				for (const Fly::LinkProxy& link : aNodeGraphContext.myNodeGraphProxy.IterateLinks())
				{
					if (link.IsDestroyed())
					{
						continue;
					}
					ImGui::Separator();
					ImGui::Text("In: %d, Out: %d", link.GetInputPin().GetID(), link.GetOutputPin().GetID());
				}

				ImGui::Separator();

				if (aNodeGraphContext.myHoveredPinProxy)
				{
					ImGui::Text("Hovered PinID: %d", aNodeGraphContext.myHoveredPinProxy.GetID());
					ImGui::Text("Connections:");
					for (const Fly::PinID connectionID : aNodeGraphContext.myHoveredPinProxy.GetConnectedPinIDs())
					{
						ImGui::Text("%d", connectionID);
					}
				}

			}
			ImGui::End();
		}

	}

	void NodeCreation(NodeGraphContext& aNodeGraphContext)
	{

		if (ImGui::IsKeyPressed(ImGuiKey_MouseRight) && aNodeGraphContext.myIsNodeEditorHovered && !aNodeGraphContext.myClickedPinProxy && !aNodeGraphContext.myClickedNodeProxy)
		{
			ImGui::OpenPopup(NodeGraphContext::NODE_SEARCH_POPUP_NAME);
			UpdateClickPos(aNodeGraphContext);
			aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] = '\0';


			auto categoryFunc = [&](NodeTypeCategory& aMainCategory) -> void
				{

					if (aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] == '\0')
					{
						auto filterPredicate = [](const Fly::NodeTypeProxy& aNodeType) { return Fly::HasNotFlag(aNodeType.GetTraits(), Fly::eNodeTrait::NonTrivial); };
						for (const Fly::NodeTypeProxy& nodeType : Fly::IterateNodeTypes(filterPredicate))
						{
							PopulateNodeCategories(nodeType.GetName(), nodeType, aMainCategory);
						}
					}
					else
					{
						for (const Fly::NodeTypeProxy& nodeType : Fly::IterateNodeTypes())
						{
							const bool isSearched = SearchString(nodeType.GetName(), std::string_view(aNodeGraphContext.mySearchNodeData.myNodeTypeSearch));
							if (isSearched)
							{
								aMainCategory.myNodeTypes.push_back(nodeType);
							}
						}
					}
				};

			auto onClickCallback = [&](const Fly::NodeTypeProxy& aNodeTypeProxy) -> void
				{

					aNodeGraphContext.myNodeGraphProxy.CreateNode(aNodeTypeProxy, Fly::Vec2{ aNodeGraphContext.myNodeCreationClickPos.x,aNodeGraphContext.myNodeCreationClickPos.y }, aNodeGraphContext.myCommandTracker.get());


					aNodeGraphContext.myPinProxysToHighlight.clear();
					aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] = '\0';
				};
			aNodeGraphContext.mySearchNodeData.myCategoryFunction = categoryFunc;
			aNodeGraphContext.mySearchNodeData.myOnClickFunction = onClickCallback;
		}

		ShowNodeSearchMenu(aNodeGraphContext);
	}

	bool ShowNodeSearchMenu(const std::vector<Fly::NodeTypeProxy>& aNodeTypes, const NodeGraphContext& aNodeGraphContext)
	{
		bool wasClicked = false;
		for (int i = 0; i < aNodeTypes.size(); i++)
		{
			const Fly::NodeTypeProxy& nodeType = aNodeTypes[i];

			const bool isCurrentlySelected = aNodeGraphContext.mySearchNodeData.myCurrentIndex == i;
			if (ImGui::MenuItem(nodeType.GetShortName().c_str(), nullptr, isCurrentlySelected) ||
				(isCurrentlySelected && ImGui::IsKeyPressed(ImGuiKey_Enter)))
			{
				aNodeGraphContext.mySearchNodeData.myOnClickFunction(nodeType);
				wasClicked = true;
			}
		}
		return wasClicked;
	}

	bool ShowNodeSearchMenuByCategory(const NodeTypeCategory& aCategory, const NodeGraphContext& aNodeGraphContext)
	{
		bool wasClicked = false;
		if (ImGui::BeginMenu(aCategory.myName.c_str()))
		{

			for (const NodeTypeCategory& childCategory : aCategory.myChildCategories)
			{
				wasClicked |= ShowNodeSearchMenuByCategory(childCategory, aNodeGraphContext);
			}

			wasClicked |= ShowNodeSearchMenu(aCategory.myNodeTypes, aNodeGraphContext);

			ImGui::EndMenu();
		}
		return wasClicked;
	}

	void ShowNodeSearchMenu(NodeGraphContext& aNodeGraphContext)
	{
		if (ImGui::BeginPopup(NodeGraphContext::NODE_SEARCH_POPUP_NAME))
		{

			NodeTypeCategory mainCategory{ "Create Node" };
			aNodeGraphContext.mySearchNodeData.myCategoryFunction(mainCategory);

			ImGui::SetKeyboardFocusHere();
			ImGui::InputTextWithHint("##", "Node Type", aNodeGraphContext.mySearchNodeData.myNodeTypeSearch, IM_ARRAYSIZE(aNodeGraphContext.mySearchNodeData.myNodeTypeSearch));

			bool wasClicked = false;

			if (aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] == '\0')
			{

				for (const NodeTypeCategory& category : mainCategory.myChildCategories)
				{
					wasClicked |= ShowNodeSearchMenuByCategory(category, aNodeGraphContext);
				}
			}
			else
			{
				wasClicked |= ShowNodeSearchMenu(mainCategory.myNodeTypes, aNodeGraphContext);
			}

			if (wasClicked)
			{
				aNodeGraphContext.mySearchNodeData.myCurrentIndex = 0;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		else
		{
			aNodeGraphContext.mySearchNodeData = {};
		}
	}

	void PopulateNodeCategories(const std::string& aName, const Fly::NodeTypeProxy& aNodeType, NodeTypeCategory& aCategory)
	{
		std::string categoryName = aName.substr(0, aName.find_first_of('/'));
		std::string rest = aName.substr(categoryName.length(), aName.length() - 1);

		if (rest.empty())
		{
			aCategory.myNodeTypes.push_back(aNodeType);
			return;
		}

		rest = rest.substr(1, rest.length());

		bool hasFoundCategory = false;
		for (NodeTypeCategory& childCategory : aCategory.myChildCategories)
		{
			if (childCategory.myName == categoryName)
			{
				PopulateNodeCategories(rest, aNodeType, childCategory);
				hasFoundCategory = true;
				break;
			}
		}

		if (!hasFoundCategory)
		{
			aCategory.myChildCategories.emplace_back(categoryName);
			PopulateNodeCategories(rest, aNodeType, aCategory.myChildCategories.back());
		}
	}
	NodeGraphContext::NodeGraphContext()
	{
		myImNodesContext = ImNodes::CreateContext();
		myCommandTracker = std::make_unique<Fly::CommandTracker>();
	}

	NodeGraphContext::~NodeGraphContext()
	{
		ImNodes::DestroyContext(myImNodesContext);
	}
}
