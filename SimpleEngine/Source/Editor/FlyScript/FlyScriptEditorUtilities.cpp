#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "NodeScript/Fly.hpp"
#include "NodeScript/Command/FlyCommandTracker.hpp"
#include <imnodes/imnodes.h>
#include <imnodes/imnodes_internal.h>


namespace Editor
{

	static bool DataTypeCombo(const char* aComboLabel, Fly::GenericDataTypeFacade& aDataTypeFacade, auto aFilterPredicate)
	{
		bool wasSelected = false;
		if (ImGui::BeginCombo(aComboLabel, aDataTypeFacade.GetName().c_str()))
		{
			const std::vector<Fly::GenericDataTypeFacade> dataTypes = Fly::GetGenericDataTypesFiltered(aFilterPredicate);

			for (const Fly::GenericDataTypeFacade& dataType : dataTypes)
			{
				if (ImGui::Selectable(dataType.GetName().c_str(), aDataTypeFacade == dataType))
				{
					aDataTypeFacade = dataType;
					wasSelected = true;
				}
			}

			ImGui::EndCombo();
		}
		return wasSelected;
	}

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::GenericDataTypeFacade& aDataTypeFacade)
	{
		return DataTypeCombo(aComboLabel, aDataTypeFacade, [](const Fly::GenericDataTypeFacade& aDataType) -> bool { return aDataType.IsViewAndEditable(); });
	}

	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::GenericDataTypeFacade& aDataTypeFacade)
	{
		return DataTypeCombo(aComboLabel, aDataTypeFacade, [](const Fly::GenericDataTypeFacade& aDataType) -> bool { return aDataType.IsTargetable(); });
	}

	bool DataTypeComboNoFilter(const char* aComboLabel, Fly::GenericDataTypeFacade& aDataTypeFacade)
	{
		return DataTypeCombo(aComboLabel, aDataTypeFacade, [](const Fly::GenericDataTypeFacade&) -> bool { return true; });
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
		
		const Fly::NodeGraphFacade& nodeGraphFacade = aNodeGraphContext.myNodeGraphFacade;
		ImNodes::BeginNodeEditor();

		const std::vector<Fly::NodeFacade> nodeFacades = nodeGraphFacade.GetNodeFacades();

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

				if (nodeFacade.IsAccessor())
				{
					/*const Fly::VariableFacade variable = nodeFacade.GetVariableFacade();
					const bool isGetter = Fly::HasFlag(nodeFacade.GetTraits(), Fly::eNodeTrait::Getter);
					const char* const prefixLabel = isGetter ? "Get " : "Set ";
					nodeLabel = prefixLabel + variable.GetName();*/
				}
				else
				{
					nodeLabel = nodeFacade.GetNodeTypeFacade().GetShortName();
				}

				if (Fly::IsDebugging())
				{
					nodeLabel += ", ID: " + std::to_string(nodeFacade.GetID());
				}
				ImGui::TextUnformatted(nodeLabel.c_str());

				nodeNameWidth = ImGui::CalcTextSize(nodeLabel.c_str()).x;

				ImNodes::EndNodeTitleBar();
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();

			std::vector<Fly::PinFacade> inputPinFacades = nodeFacade.GetSplitInputPinFacades();
			float nodeWidthLeft = inputPinFacades.empty() ? 0.f : 100.f;

			for (const Fly::PinFacade& inputPinFacade : inputPinFacades)
			{
				const std::string_view pinLabel = inputPinFacade.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(std::string(pinLabel).c_str()).x;

				nodeWidthLeft = std::max(nodeWidthLeft, labelWidth);
			}

			float nodeWidthRight = 0.f;

			std::vector<Fly::PinFacade> outputPinFacades = nodeFacade.GetSplitOutputPinFacades();

			for (const Fly::PinFacade& outputPinFacade : outputPinFacades)
			{
				const std::string_view pinLabel = outputPinFacade.GetPinTypeName();
				const float labelWidth = ImGui::CalcTextSize(std::string(pinLabel).c_str()).x;

				nodeWidthRight = std::max(nodeWidthRight, labelWidth);
			}

			float extraWidth = 0.f;

			if (nodeWidthLeft > 0.f && nodeWidthRight > 0.f)
			{
				extraWidth += 30.f;
			}

			// Render input pins
			for (Fly::PinFacade& inputPinFacade : inputPinFacades)
			{
				const Fly::GenericDataTypeFacade pinDataType(inputPinFacade.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor()/* - myHoverTint*/));

				const bool shouldBeHighlighted = false;// std::find(currentNodeContext.myPinFacadesToHighlight.begin(), currentNodeContext.myPinFacadesToHighlight.end(), inputPinFacade) != currentNodeContext.myPinFacadesToHighlight.end();


				const ImNodesPinShape shape = [shouldBeHighlighted, inputPinFacade]()
					{
						if (shouldBeHighlighted)
						{
							return ImNodesPinShape_Triangle;
						}

						const bool isPointer = Fly::GenericDataTypeFacade(inputPinFacade.GetDataTypeID()).IsPointer();
						return isPointer ? ImNodesPinShape_QuadFilled : ImNodesPinShape_CircleFilled;
					}();
				ImNodes::BeginInputAttribute(inputPinFacade.GetID(), shape);

				std::string pinLabel(inputPinFacade.GetPinTypeName());
				if (!pinLabel.empty())
				{
					if (Fly::IsDebugging())
					{
						pinLabel += ", " + std::to_string(inputPinFacade.GetID());
					}
					ImGui::TextUnformatted(pinLabel.c_str());
				}

				if (inputPinFacade.GetConnectedPinIDs().empty())
				{
					const float itemWidth = std::max(20.f, nodeWidthLeft);
					ImGui::PushItemWidth(itemWidth);

					inputPinFacade.ViewAndEdit(aNodeGraphContext.myCommandTracker.get());

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
				const Fly::GenericDataTypeFacade pinDataType(outputPinFacade.GetDataTypeID());
				ImNodes::PushColorStyle(ImNodesCol_Pin, ToImGuiColor(pinDataType.GetColor()));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ToImGuiColor(pinDataType.GetColor() - NodeGraphContext::HOVER_TINT));

				const bool shouldBeHighlighted = std::find(aNodeGraphContext.myPinFacadesToHighlight.begin(), aNodeGraphContext.myPinFacadesToHighlight.end(), outputPinFacade) != aNodeGraphContext.myPinFacadesToHighlight.end();

				const ImNodesPinShape shape = [shouldBeHighlighted, outputPinFacade]()
					{
						if (shouldBeHighlighted)
						{
							return ImNodesPinShape_Triangle;
						}

						const bool isPointer = Fly::GenericDataTypeFacade(outputPinFacade.GetDataTypeID()).IsPointer();
						return isPointer ? ImNodesPinShape_QuadFilled : ImNodesPinShape_CircleFilled;
					}();

				ImNodes::BeginOutputAttribute(outputPinFacade.GetID(), shape);

				std::string pinLabel(outputPinFacade.GetPinTypeName());

				if (!pinLabel.empty())
				{
					const float plusWidth = std::max(nodeWidthLeft + extraWidth, nodeNameWidth - nodeWidthRight);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + plusWidth + extraWidth + nodeWidthRight - ImGui::CalcTextSize(pinLabel.c_str()).x);

					if (Fly::IsDebugging())
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

		std::vector<Fly::LinkFacade> linkFacades = nodeGraphFacade.GetLinkFacades();

		for (const Fly::LinkFacade& linkFacade : linkFacades)
		{
			if (linkFacade.IsDestroyed())
			{
				continue;
			}
			const Fly::PinFacade pinFacade = linkFacade.GetInputPin();
			const Fly::GenericDataTypeFacade pinDataType(pinFacade.GetDataTypeID());

			const Fly::Color dataTypeColor = pinDataType.GetColor();
			Fly::Color linkColor = dataTypeColor;
			if (std::find(aNodeGraphContext.myTraversedLinks.begin(), aNodeGraphContext.myTraversedLinks.end(), linkFacade) != aNodeGraphContext.myTraversedLinks.end())
			{
				linkColor = NodeGraphContext::TRAVERSED_LINK_COLOR;
			}
			ImNodes::PushColorStyle(ImNodesCol_Link, ToImGuiColor(linkColor));
			ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ToImGuiColor(linkColor - NodeGraphContext::SELECTION_TINT));
			ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ToImGuiColor(linkColor - NodeGraphContext::HOVER_TINT));

			ImNodes::Link(linkFacade.GetID(), linkFacade.GetInputPin().GetID(), linkFacade.GetOutputPin().GetID());

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
		Fly::NodeGraphFacade& nodeGraphFacade = aNodeGraphContext.myNodeGraphFacade;
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

		const std::vector<Fly::NodeFacade> nodeFacades = nodeGraphFacade.GetNodeFacades();

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
				aNodeGraphContext.myNodeDragData.emplace(nodeFacade.GetID(), Fly::NodeDragData{ .mStartPos = oldPos });
			}
			else if (dragEnded)
			{
				auto it = aNodeGraphContext.myNodeDragData.find(nodeFacade.GetID());

				if (it != aNodeGraphContext.myNodeDragData.end())
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
			aNodeGraphContext.myNodeGraphFacade.CommitNodeDrag(aNodeGraphContext.myNodeDragData, aNodeGraphContext.myCommandTracker.get());
			aNodeGraphContext.myNodeDragData.clear();
		}

		Fly::NodeID hoveredNodeID = Fly::InvalidID<Fly::NodeID>();
		if (ImNodes::IsNodeHovered(&hoveredNodeID.mID) && ImGui::IsKeyPressed(ImGuiKey_MouseRight))
		{
			aNodeGraphContext.myClickedNodeFacade = Fly::NodeFacade(hoveredNodeID, aNodeGraphContext.myNodeGraphFacade);

			ImGui::OpenPopup("Node Popup");
		}

		if (ImGui::BeginPopup("Node Popup"))
		{
			ImGui::Text("Node Options");

			ImGui::Separator();
			if (ImGui::Selectable("Destroy Node"))
			{
				aNodeGraphContext.myClickedNodeFacade.Destroy(aNodeGraphContext.myCommandTracker.get());
				ImGui::CloseCurrentPopup();
			}

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedNodeFacade.HasAnyConnectedLinks());
			if (ImGui::Selectable("Destroy Links"))
			{
				aNodeGraphContext.myClickedNodeFacade.DestroyConnectedLinks(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::Separator();

			if (aNodeGraphContext.myClickedNodeFacade.IsReplacable() && ImGui::BeginMenu("Data Type"))
			{
				const std::vector<Fly::DataTypeFacade> replacableDataTypeFacades = aNodeGraphContext.myClickedNodeFacade.GetNodeTypeFacade().GetReplacableDataTypes();
				for (const Fly::DataTypeFacade& replacableDataTypeFacade : replacableDataTypeFacades)
				{
					ImGui::PushID(static_cast<int>(replacableDataTypeFacade.GetID()));
					if (ImGui::Selectable(replacableDataTypeFacade.GetName().c_str()))
					{
						aNodeGraphContext.myNodeGraphFacade.ReplaceTemplateNode(aNodeGraphContext.myClickedNodeFacade, replacableDataTypeFacade, aNodeGraphContext.myCommandTracker.get());
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
			aNodeGraphContext.myClickedNodeFacade = {};
		}

		Fly::PinID hoveredPinID;
		if (ImNodes::IsPinHovered(&hoveredPinID.mID) && ImGui::IsKeyPressed(ImGuiKey_MouseRight))
		{
			aNodeGraphContext.myClickedPinFacade = Fly::PinFacade(hoveredPinID, aNodeGraphContext.myNodeGraphFacade);

			ImGui::OpenPopup("Pin Popup");

		}

		if (ImGui::BeginPopup("Pin Popup"))
		{

			ImGui::Text("Pin Options");

			ImGui::Separator();

			Fly::GenericDataTypeFacade pinDataType(aNodeGraphContext.myClickedPinFacade.GetDataTypeID());
			ImGui::Text("Type: %s", pinDataType.GetName().c_str());
			

			if (aNodeGraphContext.myClickedPinFacade.GetFlowType() == Fly::eFlowType::Input)
			{
				aNodeGraphContext.myClickedPinFacade.ViewAndEdit(aNodeGraphContext.myCommandTracker.get());
			}
			else
			{
				aNodeGraphContext.myClickedPinFacade.View();
			}

			ImGui::Separator();

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedPinFacade.IsSplitable());
			if (ImGui::Selectable("Split Pin"))
			{
				aNodeGraphContext.myClickedPinFacade.Split(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedPinFacade.IsRecombinable());
			if (ImGui::Selectable("Recombine Pin"))
			{
				aNodeGraphContext.myClickedPinFacade.RecombineParentPin(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!aNodeGraphContext.myClickedPinFacade.HasAnyConnectedLinks());
			if (ImGui::Selectable("Destroy Links"))
			{
				aNodeGraphContext.myClickedPinFacade.DestroyConnectedLinks(aNodeGraphContext.myCommandTracker.get());
			}
			ImGui::EndDisabled();

			ImGui::Separator();

			if (aNodeGraphContext.myClickedPinFacade.IsParentNodeReplacable() && ImGui::BeginMenu("Choose Type"))
			{
				const Fly::GenericDataTypeID d = aNodeGraphContext.myClickedPinFacade.GetDataTypeID();
				const Fly::DataTypeID* clickedPinDataTypeID = std::get_if<Fly::DataTypeID>(&d.mID);
				if (clickedPinDataTypeID)
				{
					const std::vector<Fly::DataTypeFacade> replacableDataTypeFacades = Fly::GetNodeOwner(aNodeGraphContext.myClickedPinFacade).GetNodeTypeFacade().GetReplacableDataTypes();
					for (const Fly::DataTypeFacade& replacableDataTypeFacade : replacableDataTypeFacades)
					{
						ImGui::PushID(static_cast<int>(replacableDataTypeFacade.GetID()));
						if (ImGui::Selectable(replacableDataTypeFacade.GetName().c_str(), replacableDataTypeFacade.GetID() == *clickedPinDataTypeID))
						{
							aNodeGraphContext.myNodeGraphFacade.ReplaceTemplateNode(aNodeGraphContext.myClickedPinFacade, replacableDataTypeFacade, aNodeGraphContext.myCommandTracker.get());
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
			aNodeGraphContext.myClickedPinFacade = Fly::PinFacade();
		}


		// See if links should be created

		Fly::PinID createdLinkPinID1;
		Fly::PinID createdLinkPinID2;

		if (ImNodes::IsLinkCreated(&createdLinkPinID1.mID, &createdLinkPinID2.mID))
		{
			aNodeGraphContext.myNodeGraphFacade.TryCreateLink(Fly::PinFacade(createdLinkPinID1, aNodeGraphContext.myNodeGraphFacade), Fly::PinFacade(createdLinkPinID2, aNodeGraphContext.myNodeGraphFacade), aNodeGraphContext.myCommandTracker.get());
			aNodeGraphContext.myPinFacadesToHighlight.clear();
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
				aNodeGraphContext.myNodeGraphFacade.DestroySelection(selectedNodes, selectedLinks, aNodeGraphContext.myCommandTracker.get());
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

				const Fly::PinFacade startedPin(startedPinID, aNodeGraphContext.myNodeGraphFacade);

				aNodeGraphContext.myPinFacadesToHighlight = startedPin.GetPotentialConnections();

				std::erase_if(aNodeGraphContext.myPinFacadesToHighlight,
					[&](const Fly::PinFacade& aPinFacade)-> bool
					{
						return aPinFacade.GetID() == startedPin.GetID();
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
			aNodeGraphContext.myPinFacadesToHighlight.clear();
			aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] = '\0';

			UpdateClickPos(aNodeGraphContext);

			auto categoryFunc = [&aNodeGraphContext](NodeTypeCategory& aMainCategory) -> void
				{
					const Fly::PinFacade pinFacade(aNodeGraphContext.myLinkCreationPinID, aNodeGraphContext.myNodeGraphFacade);

					const std::vector<Fly::NodeTypeFacade> filteredNodeTypesByDataTypeAndFlowType = Fly::GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(pinFacade.GetDataTypeID(), InvertFlowType(pinFacade.GetFlowType()), Fly::eNodeTrait::NonTrivial, Fly::HasNotFlag);

					if (aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] == '\0')
					{
						for (const Fly::NodeTypeFacade& nodeType : filteredNodeTypesByDataTypeAndFlowType)
						{
							PopulateNodeCategories(nodeType.GetName(), nodeType, aMainCategory);
						}
					}
					else
					{
						for (const Fly::NodeTypeFacade& nodeType : filteredNodeTypesByDataTypeAndFlowType)
						{
							const bool isSearched = SearchString(nodeType.GetName(), std::string_view(aNodeGraphContext.mySearchNodeData.myNodeTypeSearch));
							if (isSearched)
							{
								aMainCategory.myNodeTypes.push_back(nodeType);
							}
						}
					}

				};

			auto onClickCallback = [&aNodeGraphContext](const Fly::NodeTypeFacade& aNodeTypeFacade) -> void
				{
					aNodeGraphContext.myNodeGraphFacade.CreateNodeAutoLink(aNodeTypeFacade, aNodeGraphContext.myLinkCreationPinID, Fly::Vec2{ aNodeGraphContext.myNodeCreationClickPos.x,aNodeGraphContext.myNodeCreationClickPos.y }, aNodeGraphContext.myCommandTracker.get());

					aNodeGraphContext.myPinFacadesToHighlight.clear();
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

				const std::vector<Fly::LinkFacade> linkFacades = aNodeGraphContext.myNodeGraphFacade.GetLinkFacades();

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

				if (aNodeGraphContext.myHoveredPinFacade)
				{
					ImGui::Text("Hovered PinID: %d", aNodeGraphContext.myHoveredPinFacade.GetID());
					ImGui::Text("Connections:");
					for (const Fly::PinID connectionID : aNodeGraphContext.myHoveredPinFacade.GetConnectedPinIDs())
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

		if (ImGui::IsKeyPressed(ImGuiKey_MouseRight) && aNodeGraphContext.myIsNodeEditorHovered && !aNodeGraphContext.myClickedPinFacade && !aNodeGraphContext.myClickedNodeFacade)
		{
			ImGui::OpenPopup(NodeGraphContext::NODE_SEARCH_POPUP_NAME);
			UpdateClickPos(aNodeGraphContext);
			aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] = '\0';


			auto categoryFunc = [&](NodeTypeCategory& aMainCategory) -> void
				{

					if (aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] == '\0')
					{

						const std::vector<Fly::NodeTypeFacade> filteredNodeTypes = Fly::GetNodeTypesFilteredByTrait(Fly::eNodeTrait::NonTrivial, Fly::HasNotFlag);

						for (const Fly::NodeTypeFacade& nodeType : filteredNodeTypes)
						{
							PopulateNodeCategories(nodeType.GetName(), nodeType, aMainCategory);
						}
					}
					else
					{
						const std::vector<Fly::NodeTypeFacade> filteredNodeTypes = Fly::GetNodeTypesFilteredByTrait(Fly::eNodeTrait::NonTrivial, Fly::HasNotFlag);

						for (const Fly::NodeTypeFacade& nodeType : filteredNodeTypes)
						{
							const bool isSearched = SearchString(nodeType.GetName(), std::string_view(aNodeGraphContext.mySearchNodeData.myNodeTypeSearch));
							if (isSearched)
							{
								aMainCategory.myNodeTypes.push_back(nodeType);
							}
						}
					}
				};

			auto onClickCallback = [&](const Fly::NodeTypeFacade& aNodeTypeFacade) -> void
				{

					aNodeGraphContext.myNodeGraphFacade.CreateNode(aNodeTypeFacade, Fly::Vec2{ aNodeGraphContext.myNodeCreationClickPos.x,aNodeGraphContext.myNodeCreationClickPos.y }, aNodeGraphContext.myCommandTracker.get());


					aNodeGraphContext.myPinFacadesToHighlight.clear();
					aNodeGraphContext.mySearchNodeData.myNodeTypeSearch[0] = '\0';
				};
			aNodeGraphContext.mySearchNodeData.myCategoryFunction = categoryFunc;
			aNodeGraphContext.mySearchNodeData.myOnClickFunction = onClickCallback;
		}

		ShowNodeSearchMenu(aNodeGraphContext);
	}

	bool ShowNodeSearchMenu(const std::vector<Fly::NodeTypeFacade>& aNodeTypes, const NodeGraphContext& aNodeGraphContext)
	{
		bool wasClicked = false;
		for (int i = 0; i < aNodeTypes.size(); i++)
		{
			const Fly::NodeTypeFacade& nodeType = aNodeTypes[i];

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

	void PopulateNodeCategories(const std::string& aName, const Fly::NodeTypeFacade& aNodeType, NodeTypeCategory& aCategory)
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
