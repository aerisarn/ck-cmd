#pragma once

#include <Common/Base/hkBase.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachine_4.h>

namespace ckcmd
{
	namespace HKX
	{

		template <typename T>
		void initializeValues(T& obj) {}

		template <>
		void initializeValues<hkbStateMachineStateInfo>(hkbStateMachineStateInfo& obj)
		{
			obj.m_enable = true;
			obj.m_probability = 1.;
		}

		void initializeValues(void* obj, const hkClass* hkclass)
		{
			if (hkclass == &hkbStateMachineStateInfoClass)
				initializeValues<hkbStateMachineStateInfo>(static_cast<hkbStateMachineStateInfo&>(*(hkbStateMachineStateInfo*)obj));
			else if (hkclass == &hkbStateMachineClass)
				initializeValues<hkbStateMachine>(static_cast<hkbStateMachine&>(*(hkbStateMachine*)obj));
		}
	}
}