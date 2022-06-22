#pragma once

#include "HkxItemVisitor.h"
#include "HkxVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"
#include "HkxItemReal.h"

#include <QVariant>
#include <vector>

namespace ckcmd {
	namespace HKX {

		class RowResizer : public HkxConcreteVisitor<RowResizer> {
			int _delta;
			int _row;
			int _column;

			bool _result = false;

			template<typename T>
			void check(T& value);
			void check(void* value);

		public:

			RowResizer(const size_t row, const size_t column, int size_delta) :
				HkxConcreteVisitor(*this), 
				_row(row), _column(column), _delta(size_delta) {}

			QVariant delta() { return _delta; }
			bool result() { return _result; }

			template<typename T>
			void visit(T& value);

			template<> void visit(hkVector4& value);
			template<> void visit(::hkQuaternion& value);
			template<> void visit(hkMatrix3& value);
			template<> void visit(hkRotation& value);
			template<> void visit(hkQsTransform& value);
			template<> void visit(hkMatrix4& value);
			template<> void visit(hkTransform& value);
			template<> void visit(hkBool& value);
			template<> void visit(hkUlong& value);
			template<> void visit(hkHalf& value);
			template<> void visit(hkStringPtr& value);
			template<> void visit(hkVariant& value);

			virtual void visit(char* value) override;
			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void* value, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* value, const hkClassEnum& enum_type, size_t storage) override;

		};

		template<typename T>
		void RowResizer::check(T& value) {
			_row -= 1;
		}

		template<typename T>
		void RowResizer::visit(T& value) {
			check(value);
		}

		template<>
		void RowResizer::visit(hkVector4& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(::hkQuaternion& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkMatrix3& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkRotation& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkQsTransform& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkMatrix4& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkTransform& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkBool& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkUlong& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkHalf& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkStringPtr& value) {
			_row -= 1;
		}

		template<>
		void RowResizer::visit(hkVariant& value) {
			HkxVariant h(value);
			RowCalculator r;
			h.accept(r);
			size_t array_rows = r.rows();
			int next_rows = _row - array_rows;
			if (next_rows <= 0)
			{
				h.accept(*this);
			}
			_row -= array_rows;
		}
	}
}

