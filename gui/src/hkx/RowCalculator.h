#pragma once

#include "HkxItemVisitor.h"

namespace ckcmd {
	namespace HKX {
		class RowCalculator : public HkxConcreteVisitor<RowCalculator> {
			size_t _rows = 0;
		public:

			RowCalculator() : HkxConcreteVisitor(*this) {}

			size_t rows() { return _rows > 0 ? _rows : 1; }

			template<typename T>
			void visit(T& value) { _rows += 1; }

			void visit(char* value) { _rows += 1; }

			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;

		};
	}
}