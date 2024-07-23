#pragma once
#include "../FlyDefines.hpp"
#include "../SystemTypes/FlyVec2.hpp"
#include "../Pin/FlyPin.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	template<typename T, size_t Capacity>
	class DualArray
	{
		using Byte = unsigned char;
	public:

		DualArray()
			: myBuffer{}
			, myCurrentSize(0)
		{
			for (size_t i = 0; i < Capacity; ++i)
			{
				Byte* currentByte = reinterpret_cast<Byte*>(reinterpret_cast<size_t>(&myBuffer[0]) + i * sizeof(T));
				
				::new(currentByte)T();

			}
		}

		~DualArray()
		{

		}

		template<size_t Side> requires IsInRange<Side, 0, 1>
		T& PushBack(const T& aValue)
		{
			Byte* currentByte = &myBuffer[0] + myCurrentSize;
			::new(currentByte) T(aValue);

			myCurrentSize += sizeof(T);

			return *reinterpret_cast<T*>(currentByte);
		}

	private:

		Byte myBuffer[sizeof(T) * Capacity];
		size_t myCurrentSize = 0;
		size_t mySegment = sizeof(T) * Capacity / 2;
	};

	class Node
	{
	public:

		template<typename IteratorType1, typename IteratorType2>
		Node(const NodeTypeID aTypeID, const IteratorType1& aInputPins, const IteratorType2& aOutputPins)
			: mTypeID(aTypeID)
			, mInputPins(std::vector<PinID>(aInputPins.begin(), aInputPins.end()))
			, mOutputPins(std::vector<PinID>(aOutputPins.begin(), aOutputPins.end()))
		{

		}

		Node(const NodeTypeID aTypeID, const std::vector<PinID>& aInputPins, const std::vector<PinID>& aOutputPins)
			: mTypeID(aTypeID)
			, mInputPins(aInputPins)
			, mOutputPins(aOutputPins)
		{

		}

		~Node() = default;

		Node(const Node&) = default;
		Node(Node&&) = default;
		Node& operator=(const Node&) = delete;
		Node& operator=(Node&&) = delete;

	public:


		const NodeTypeID mTypeID;
		std::vector<PinID> mInputPins;
		std::vector<PinID> mOutputPins;

		Vec2 mPosition;
		bool mIsDestroyed = false;
		
	};

	
}
