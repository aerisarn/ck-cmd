#pragma once

#include <QMetaType>

namespace ckcmd {
	namespace HKX {
		class HkxItemPointer {
			int _file_index;
			const void* _pointer;
			const hkClassMember* _member_class;

		public:
			HkxItemPointer() = default;
			~HkxItemPointer() = default;
			HkxItemPointer(const HkxItemPointer&) = default;
			HkxItemPointer& operator=(const HkxItemPointer&) = default;

			HkxItemPointer(int file_index, const void* pointer, const hkClassMember* member_class) : _file_index(file_index), _pointer(pointer), _member_class(member_class) {}
			const void* get() const { return _pointer; }
			const hkClassMember* field_class() const { return _member_class; }
			const int file_index() const { return _file_index; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemPointer);