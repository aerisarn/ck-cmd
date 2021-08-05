#pragma once

#include <QMetaType>

namespace ckcmd {
	namespace HKX {
		class HkxItemPointer {
			int _file_index;
			const void* _pointer;
		public:
			HkxItemPointer() = default;
			~HkxItemPointer() = default;
			HkxItemPointer(const HkxItemPointer&) = default;
			HkxItemPointer& operator=(const HkxItemPointer&) = default;

			HkxItemPointer(int file_index, const void* pointer) : _file_index(file_index), _pointer(pointer) {}
			const void* get() { return _pointer; }
			const int file_index() { return _file_index; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemPointer);