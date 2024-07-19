#pragma once

namespace SCRIPT
{
	class SimpleNodeScript final
	{
	public:
		SimpleNodeScript();
		~SimpleNodeScript();

		void Init();

		static constexpr const char* SCRIPT_FILE_PATH = "Assets/Scripts/";
	};
}