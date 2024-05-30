#include "RandomNodes.h"
#include "../Node/NodeTypeRegistry.h"
#include <random>

namespace SCR
{

	template<std::integral T>
	static T GetRandomNumberNode(const T aMin, const T aMax)
	{
		if (aMin > aMax)
		{
			return std::numeric_limits<T>::max();
		}
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<T> distr(aMin, aMax); // define the range

		return distr(gen);
	}

	void Do(float aFlow)
	{
		aFlow;
	}

	template<typename T>
	T& MemFunc(MemoryPool* aPool, MemoryPoolID anID)
	{
		return aPool->At<T>(anID);
	}


	void RegisterRandomNodes()
	{
		MemoryPool memPool;
		MemoryPoolID floatID = memPool.Allocate<float>(5.f);
		MemoryTuple<Flow, float> memTuple;

		auto f = Do;
		auto func = [f](Flow, float&& aFloat) -> std::tuple<Flow>
			{
				f(aFloat);
				return true;
			};
		{



			memTuple.At<0>() = [](MemoryPool* aPool, MemoryPoolID anID) -> Flow&
				{
					return aPool->At<Flow>(anID);
				};
			memTuple.At<1>() = [](MemoryPool* aPool, MemoryPoolID anID) -> float&
				{
					return aPool->At<float>(anID);
				};

			std::array<std::tuple<MemoryPoolID>, 1> idArray{ floatID };

			

			using Result = MemoryTuple<Flow, float>::ResultTuple;

			Result resultTuple;
			//std::tuple<std::reference_wrapper<Flow>> resolveTuple;


			memTuple.Resolve(&memPool);
		}

		//std::tuple<Flow> returns = std::apply(func, resultTuple);
		//std::tuple<Flow> returns = memTuple.Call(func, &memPool, std::move(idArray));


		{
			MemoryTupleNew<Flow, float> n;

			n.At<0>() = [](MemoryPool* aPool, MemoryPoolID anID) -> Flow&
				{
					return aPool->At<Flow>(anID);
				};
			n.At<1>() = [](MemoryPool* aPool, MemoryPoolID anID) -> float&
				{
					return aPool->At<float>(anID);
				};

			//std::tuple<Flow> result = n.Call(func, &memPool, { floatID });
			//std::tuple<Flow>
		}





		NodeTypeRegistry::RegisterFlowNodeType(Do, "FSKF");

		NodeTypeRegistry::RegisterNodeType(GetRandomNumberNode<int>, "Utility/Random/Get Random Number (Int)", NodeTypeDesc{ { "Min", "Max" }, { "Value" } });
		NodeTypeRegistry::RegisterNodeType(GetRandomNumberNode<unsigned int>, "Utility/Random/Get Random Number (Unsigned Int)", NodeTypeDesc{ { "Min", "Max" }, { "Value" } });
		NodeTypeRegistry::RegisterNodeType(GetRandomNumberNode<unsigned long long>, "Utility/Random/Get Random Number (Unsigned Long Long)", NodeTypeDesc{ { "Min", "Max" }, { "Value" } });
	}

}
