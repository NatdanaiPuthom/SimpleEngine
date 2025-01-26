#pragma once
#include "Editor/EditorEngine.hpp"

namespace Editor
{
	class EditorProxy final
	{
		friend class EditorEngine;
	public:
		EditorProxy() = delete;
		~EditorProxy() = delete;

		static CommandTracker& GetCommandTracker();

	private:
		inline static EditorEngine* myEditorEngine = nullptr;
	};
}