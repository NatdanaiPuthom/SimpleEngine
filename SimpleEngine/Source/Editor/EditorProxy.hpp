#pragma once
#include "Editor/EditorEngine.hpp"
#include "Editor/Utility/EditorUtilities.hpp"

namespace Editor
{
	class EditorProxy final
	{
		friend class EditorEngine;
	public:
		EditorProxy() = delete;
		~EditorProxy() = delete;

	public:
		static void SetSelectedEntityID(size_t aEntityID);
		static void SetSelectedEntityIDToInvalid();
	public:
		static CommandTracker& GetCommandTracker();
		static size_t GetSelectedEntityID();
	private:
		inline static EditorEngine* myEditorEngine = nullptr;
		inline static size_t mySelectedEntityID = GetInvalidIndex<size_t>();
	};
}