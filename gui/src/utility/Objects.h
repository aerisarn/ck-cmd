#pragma once

#include <Common/Base/hkBase.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachine_4.h>
#include <hkbClipGenerator_2.h>
#include <hkbClipTrigger_1.h>

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

		template <>
		void initializeValues<hkbClipGenerator>(hkbClipGenerator& obj)
		{
			obj.m_animationBindingIndex = -1;
			obj.m_playbackSpeed = 1.;
		}

		template <>
		void initializeValues<hkbClipTrigger>(hkbClipTrigger& obj)
		{
		}

		void initializeValues(void* obj, const hkClass* hkclass)
		{
			if (hkclass == &hkbStateMachineStateInfoClass)
				initializeValues<hkbStateMachineStateInfo>(static_cast<hkbStateMachineStateInfo&>(*(hkbStateMachineStateInfo*)obj));
			else if (hkclass == &hkbStateMachineClass)
				initializeValues<hkbStateMachine>(static_cast<hkbStateMachine&>(*(hkbStateMachine*)obj));
			else if (hkclass == &hkbClipGeneratorClass)
				initializeValues<hkbClipGenerator>(static_cast<hkbClipGenerator&>(*(hkbClipGenerator*)obj));
			else if (hkclass == &hkbClipTriggerClass)
				initializeValues<hkbClipTrigger>(static_cast<hkbClipTrigger&>(*(hkbClipTrigger*)obj));
		}
	}
}