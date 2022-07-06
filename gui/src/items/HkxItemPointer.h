#pragma once

#include <QMetaType>

namespace ckcmd {
	namespace HKX {
		class HkxItemPointer {
			const void* _pointer;

		public:
			HkxItemPointer() = default;
			~HkxItemPointer() = default;
			HkxItemPointer(const HkxItemPointer&) = default;
			HkxItemPointer& operator=(const HkxItemPointer&) = default;

			HkxItemPointer(const void* pointer) : _pointer(pointer) {}
			const void* get() const { return _pointer; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemPointer);