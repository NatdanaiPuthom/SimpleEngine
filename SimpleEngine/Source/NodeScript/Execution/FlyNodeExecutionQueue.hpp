#pragma once
#include "../FlyDefines.hpp"
#include "FlyInternalExecutionContext.hpp"

namespace FLY_NAMESPACE
{

	class NodeExecutor;

	class NodeExecutionQueue final
	{
	public:

		NodeExecutionQueue(NodeExecutor& aNodeExecutor);

		void Push(const NodeExecutionData& aNode)
		{
			mExecutionQueue.insert(mExecutionQueue.begin() + mCurrentInsertionIndex, aNode);
			++mCurrentInsertionIndex;
		}

		void Execute();

		void Clear()
		{
			mExecutionQueue.clear();
		}

	private:

		std::vector<NodeExecutionData> mExecutionQueue;

		size_t mCurrentInsertionIndex = 0;

		NodeExecutor* mNodeExecutor = nullptr;

	};
}
