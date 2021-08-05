#pragma once

#include "HkxItemVisitor.h"
#include <vector>

namespace ckcmd {
	namespace HKX {

		class ColumnCalculator : public HkxConcreteVisitor<ColumnCalculator> {
			size_t _columns = 1;
			std::vector<size_t> _columns_size;

			inline void fit(size_t columns) {
				_columns_size.push_back(columns);
				_columns = columns > _columns ? _columns = columns : _columns;
			}

		public:

			ColumnCalculator() : HkxConcreteVisitor(*this) {}

			size_t columns() { return _columns; }
			size_t column(size_t row) { return _columns_size[row]; }
			size_t rows() { return _columns_size.size(); }

			template<typename T>
			void visit(T& value) { fit(1); }

			void visit(char* value) { fit(1); }

			template<> void visit(hkVector4& value);
			template<> void visit(::hkQuaternion& value);
			template<> void visit(hkMatrix3& value);
			template<> void visit(hkRotation& value);
			template<> void visit(hkQsTransform& value);
			template<> void visit(hkMatrix4& value);
			template<> void visit(hkTransform& value);

			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;
		};

		template<> void ColumnCalculator::visit(hkVector4& value) { fit(4); }
		template<> void ColumnCalculator::visit(::hkQuaternion& value) { fit(4); }
		template<> void ColumnCalculator::visit(hkMatrix3& value) { fit(9); }
		template<> void ColumnCalculator::visit(hkRotation& value) { fit(9); }
		template<> void ColumnCalculator::visit(hkQsTransform& value) { fit(12); }
		template<> void ColumnCalculator::visit(hkMatrix4& value) { fit(16); }
		template<> void ColumnCalculator::visit(hkTransform& value) { fit(16); }
	}
}
