#pragma once

#include <set>
#include <map>

class hkClass;

namespace ckcmd {
	namespace HKX {

		enum class MemberIndexType {
			eventIndex,
			variableIndex,
			boneIndex,
			ragdollBoneIndex,
			stateIndex,
			generatorIndex,
			bindingIndex,
			eventPayload
		};

		class Utility {
		public:
			static std::map<std::pair<const hkClass*, int>, MemberIndexType> indexedMembersMap;
		};

	}
}