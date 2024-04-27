#pragma once
#include "Game/Test/ECS2/EntityM.hpp"

namespace Simple
{
	class EntityE final
	{
	public:
		EntityE(const size_t aID, EntityM* aEntityManager);
		~EntityE();

		template<typename T>
		inline void AddComponent();

		template<typename T>
		inline T*& GetComponent();

		size_t GetID() const;

	private:
		const size_t myID;
		EntityM* myEntityManager;
	};

	template<typename T>
	inline void EntityE::AddComponent()
	{
		myEntityManager->AddComponent<T>(myID);
	}

	template<typename T>
	inline T*& EntityE::GetComponent()
	{
		return myEntityManager->GetComponent<T>(myID);
	}
}