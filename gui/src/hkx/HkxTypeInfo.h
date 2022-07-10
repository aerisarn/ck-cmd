#pragma once

#include <Common/Base/hkBase.h>
#include <Common\Base\Reflection\hkClassMember.h>

namespace ckcmd {
	namespace HKX {

		struct TypeInfo {
			hkClassMember::Type _type;
			const hkClass* _class;
		};
	}
}