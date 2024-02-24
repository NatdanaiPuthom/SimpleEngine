#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Threadpool/ThreadPool.hpp"

namespace Simple
{
	ThreadPool::ThreadPool() : myShouldStop(false), myTasksInProgress(0)
	{
		const size_t numberOfMainThreads = 2;
		const size_t avaliableThreads = std::thread::hardware_concurrency();
		const size_t useableThreads = avaliableThreads - numberOfMainThreads;

		const int stopSingleCoreUsers = static_cast<int>(avaliableThreads - numberOfMainThreads);
		assert(stopSingleCoreUsers > 0 && "Do Something About Single Core User");

		myWorkers.reserve(useableThreads);

		for (size_t i = 0; i < useableThreads; ++i)
		{
			myWorkers.emplace_back([this]
				{
					while (true)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(myQueueMutex);
							myCondition.wait(lock, [this] { return myShouldStop || !myTaskQueue.empty(); });

							if (myShouldStop && myTaskQueue.empty())
								return;

							task = std::move(myTaskQueue.front());
							myTaskQueue.pop();
						}

						myQueueSize--;
						myTasksInProgress++;
						task();
						myTasksInProgress--;
					}
				});
		}
	}

	ThreadPool::~ThreadPool()
	{
		myShouldStop = true;

		{
			std::unique_lock<std::mutex> lock(myQueueMutex);

			while (!myTaskQueue.empty())
			{
				myTaskQueue.pop();
				myQueueSize--;
			}
		}

		myCondition.notify_all();

		for (std::thread& worker : myWorkers)
		{
			if (worker.joinable())
				worker.join();
		}
	}

	size_t ThreadPool::GetTasksInProgress() const
	{
		return myTasksInProgress.load();
	}

	size_t ThreadPool::GetQueueSize() const
	{
		return myQueueSize.load();
	}
}
