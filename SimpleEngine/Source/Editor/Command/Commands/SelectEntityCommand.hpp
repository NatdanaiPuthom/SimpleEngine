#pragma once

namespace Editor
{

	class SelectEntityCommand final
	{
	public:
		size_t myNewIndex = std::numeric_limits<size_t>::max();
		size_t myOldIndex = std::numeric_limits<size_t>::max();
		size_t* mySelectedEntityIndexPtr = nullptr;

		void Execute() const
		{
			*mySelectedEntityIndexPtr = myNewIndex;
		}

		void Undo() const
		{
			*mySelectedEntityIndexPtr = myOldIndex;
		}
	};


}