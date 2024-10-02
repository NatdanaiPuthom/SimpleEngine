#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	class Class;

	class VariableRef final
	{
	public:
		VariableRef() = default;
		VariableRef(VarID aVarID, Class& aClass)
			: mClass(&aClass)
			, mVarID(aVarID)
		{

		}

		Class& GetClass() const
		{
			return *mClass;
		}

		VarID GetVarID() const
		{
			return mVarID;
		}


		friend bool operator==(const VariableRef& a, const VariableRef& b)
		{
			return a.mClass == b.mClass && a.mVarID == b.mVarID;
		}

		
	private:

		Class* mClass = nullptr;
		VarID mVarID = InvalidID<VarID>();
	};

}