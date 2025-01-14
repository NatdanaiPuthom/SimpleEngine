#pragma once
#include <array>

namespace Graphics
{
	class GenericDataManager final
	{
	public:
		GenericDataManager();
		~GenericDataManager();

		void Init();
	public:
		bool IsVSyncOn() const;
	public:
		void SetVSync(const bool aShouldTurnOn);
		void SetFPSLevelCap(const unsigned int aCapLevel);
	public:
		unsigned int GetFPSLevelCap() const;
		const std::array<float, 4>& GetClearColor() const;
	private:
		std::array<float, 4> myClearColor;
		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}