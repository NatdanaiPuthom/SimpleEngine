#pragma once
#include "../FlyDefines.hpp"
#include "../Execution/FlyExecutionTypes.hpp"

namespace FLY_NAMESPACE
{

	class ClassInstance;

	class ClassInstanceFacade final
	{
	public:

		ClassInstanceFacade() = default;
		explicit ClassInstanceFacade(ClassInstance& aClassInstance);

		void InitRuntime();

		template<typename EventFunction, typename TargetType>
		void ExecuteEvent(EventFunction aEventFunction, TargetType* aTarget, const ExecutionContextBase& aContext);

		template<typename TargetType>
		void ExecuteAutoTickers(TargetType* aTarget, const ExecutionContextBase& aContext);

		const std::string& GetName() const;



		ClassInstance& GetClassInstance();


		explicit operator bool() const;

		friend bool operator==(const ClassInstanceFacade& a, const ClassInstanceFacade& b);

	private:

		bool IsSameTarget(DataTypeID aDataTypeID) const;
		void ExecuteEventInternal(EventID aEventID, void* aTarget, const ExecutionContextBase& aContext);
		void ExecuteAutoTickersInternal(void* aTarget, const ExecutionContextBase& aContext);

	private:

		ClassInstance* mClassInstance = nullptr;


	};

	template<typename EventFunction, typename TargetType>
	inline void ClassInstanceFacade::ExecuteEvent(EventFunction aEventFunction, TargetType* aTarget, const ExecutionContextBase& aContext)
	{
		const EventID eventID = std::hash<EventFunction>()(aEventFunction);
		assert(IsSameTarget(GetDataTypeID<TargetType*>()));
		ExecuteEventInternal(eventID, aTarget, aContext);
	}

	template<typename TargetType>
	inline void ClassInstanceFacade::ExecuteAutoTickers(TargetType* aTarget, const ExecutionContextBase& aContext)
	{
		assert(IsSameTarget(GetDataTypeID<TargetType>()));
		ExecuteAutoTickersInternal(aTarget, aContext);
	}
}