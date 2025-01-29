#pragma once
#include "Editor/Command/Core/EditorCommandTracker.hpp"
#include "Engine/ECS/ECS.hpp"
#include <vector>
#include <memory>

namespace Editor
{
	class EditorEngine;
}

namespace Editor
{
	class EditorProxy final
	{
		friend class EditorEngine;
	public:
		EditorProxy() = delete;
		~EditorProxy() = delete;

		static void Init(EditorEngine* aEditorEngine);
	public:
		static void SetSelectedEntityID(size_t aEntityID);
		static void SetSelectedEntityIndex(size_t aEntityIndex);
		static void SetSelectedEntityIDToInvalid();
		static void SetSelectedEntityIndexToInvalid();
	public:
		static ECS::EntityComponentSystem& GetEditorECS();
		static std::vector<size_t>& GetVisibleEntityIDs();
		static CommandTracker& GetCommandTracker();
		static size_t GetSelectedEntityID();
		static size_t GetSelectedEntityIndex();
		static size_t& GetSelectedEntityIndexRef();
	private:
		inline static ECS::EntityComponentSystem myTemporaryECSEditor;
		inline static std::vector<size_t> myVisibleEntityIDs;
		inline static EditorEngine* myEditorEngine = nullptr;
		inline static size_t mySelectedEntityID = 0;
		inline static size_t mySelectedEntityIndex = 0;
	};
}