#pragma once

#include <QMetaType>

#include <hkbStringEventPayload_0.h>

namespace ckcmd {
	namespace HKX {

		//Only string payloads supported
		class HkxItemEventPayload {
			const hkbStringEventPayload* _pointer;

		public:
			HkxItemEventPayload() = default;
			~HkxItemEventPayload() = default;
			HkxItemEventPayload(const HkxItemEventPayload&) = default;
			HkxItemEventPayload& operator=(const HkxItemEventPayload&) = default;

			HkxItemEventPayload(const void* pointer) : _pointer((hkbStringEventPayload*)pointer) {}
			QString get() const { return  _pointer == nullptr ? "<null>" :  _pointer->m_data.cString(); }
			hkbStringEventPayload* data() const { return const_cast<hkbStringEventPayload*>(_pointer); };
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemEventPayload);