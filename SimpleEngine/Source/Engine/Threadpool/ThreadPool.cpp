#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Threadpool/ThreadPool.hpp"

namespace Simple
{
	ThreadPool::ThreadPool(const size_t aSize) 
		: myShouldStop(false)
		, myTasksInProgress(0)
	{
		const size_t numberOfMainThreads = 2;
		const size_t avaliableThreads = std::thread::hardware_concurrency();

		size_t useableThreads = aSize;

		if (useableThreads == 0)
		{
			useableThreads = avaliableThreads - numberOfMainThreads; //Avoid unnessary context switching overhead due to more threads than there are hardware threads
		}

		assert((useableThreads < 12 && useableThreads > 0) && "Attempting to create more than 12 threads. Is this a mistake? If not, increase this value");
		assert(useableThreads > 0 && "There are no avaliable thread to reserve");

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
							myCondition.wait(lock, [this] { return myShouldStop || !myTaskQueue.empty() || !myPriorityQueue.empty(); });

							if (myShouldStop && myTaskQueue.empty() && myPriorityQueue.empty())
								return;

							if (myPriorityQueue.empty() == false)
							{
								task = myPriorityQueue.top().second;
								myPriorityQueue.pop();
							}
							else
							{
								task = std::move(myTaskQueue.front());
								myTaskQueue.pop();
							}
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

			while (!myPriorityQueue.empty())
			{
				myPriorityQueue.pop();
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
