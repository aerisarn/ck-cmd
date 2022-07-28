#pragma once

#include "HkxItemVisitor.h"
#include "HkxVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"
#include <src/items/HkxItemReal.h>

#include <QVariant>
#include <vector>

namespace ckcmd {
	namespace HKX {

		class RowResizer : public HkxConcreteVisitor<RowResizer> {
			int _delta = 0;
			int _rows = 0;
			int _column = 0;
			int _target_row = 0;

			bool _result = false;

		public:

			RowResizer(const size_t row, const size_t column, int size_delta) :
				HkxConcreteVisitor(*this), 
				_target_row(row), _column(column), _delta(size_delta) {}

			QVariant delta() { return _delta; }
			bool result() { return _result; }

			template<typename T> void visit(T& value);

			virtual void visit(char*& value) override;
			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;

		};
	}
}

