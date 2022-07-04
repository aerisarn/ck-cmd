#pragma once

#include <QMetaType>

namespace ckcmd {
	namespace HKX {
		class HkxItemPointer {
			const void* _pointer;
			const hkClassMember* _member_class;

		public:
			HkxItemPointer() = default;
			~HkxItemPointer() = default;
			HkxItemPointer(const HkxItemPointer&) = default;
			HkxItemPointer& operator=(const HkxItemPointer&) = default;

			HkxItemPointer(const void* pointer, const hkClassMember* member_class) : _pointer(pointer), _member_class(member_class) {}
			const void* get() const { return _pointer; }
			const hkClassMember* field_class() const { return _member_class; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemPointer);