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

		class Setter : public HkxConcreteVisitor<Setter> {
			QVariant _value;
			int _row;
			int _column;

			bool _result = true;

			template<typename T>
			void check(T& value);
			void check(void* value);

		public:

			Setter(const size_t row, const size_t column, const QVariant& value) :
				HkxConcreteVisitor(*this), 
				_row(row), _column(column), _value(value) {}

			QVariant value() { return _value; }
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

			virtual void visit(char*& value) override;
			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void* value, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* value, const hkClassEnum& enum_type, size_t storage) override;

		};

		template<typename T>
		void Setter::check(T& value) {
			if (_row == 0)
			{
				value = _value.value<T>();
			}
			_row -= 1;
		}

		template<typename T>
		void Setter::visit(T& value) {
			check(value);
		}

		template<>
		void Setter::visit(hkVector4& value) {
			if (_row == 0) {
				auto inner_value = _value.value<HkxItemReal>();
				value.set(
					inner_value.value(0, 0),
					inner_value.value(0, 1),
					inner_value.value(0, 2)
				);
			}
			_row -= 1;
		}

		template<>
		void Setter::visit(::hkQuaternion& value) {
			if (_row == 0)
			{
				auto inner_value = _value.value<HkxItemReal>();
				value.set(
					inner_value.value(0, 0),
					inner_value.value(0, 1),
					inner_value.value(0, 2),
					inner_value.value(0, 3)
				);
			}
			_row -= 1;
		}

		template<>
		void Setter::visit(hkMatrix3& value) {
			if (_row == 0)
			{
				auto inner_value = _value.value<HkxItemReal>();
				value(0, 0) = inner_value.value(0, 0); value(0, 1) = inner_value.value(0, 1); value(0, 2) = inner_value.value(0, 2);
				value(1, 0) = inner_value.value(1, 0); value(1, 1) = inner_value.value(1, 1); value(1, 2) = inner_value.value(1, 2);
				value(2, 0) = inner_value.value(2, 0); value(2, 1) = inner_value.value(2, 1); value(2, 2) = inner_value.value(2, 2);
			}
			_row -= 1;
		}

		template<>
		void Setter::visit(hkRotation& value) {
			if (_row == 0)
			{
				auto inner_value = _value.value<HkxItemReal>();
				value(0, 0) = inner_value.value(0, 0); value(0, 1) = inner_value.value(0, 1); value(0, 2) = inner_value.value(0, 2);
				value(1, 0) = inner_value.value(1, 0); value(1, 1) = inner_value.value(1, 1); value(1, 2) = inner_value.value(1, 2);
				value(2, 0) = inner_value.value(2, 0); value(2, 1) = inner_value.value(2, 1); value(2, 2) = inner_value.value(2, 2);
			}
			_row -= 1;
		}

		template<>
		void Setter::visit(hkQsTransform& value) {
			if (_row == 0) {
				auto inner_value = _value.value<HkxItemReal>();
				value.m_translation(0) = inner_value.value(0, 0); value.m_translation(1) = inner_value.value(0, 1); value.m_translation(2) = inner_value.value(0, 2); value.m_translation(3) = inner_value.value(0, 3);
				value.m_rotation.set(inner_value.value(1, 0), inner_value.value(1, 1), inner_value.value(1, 2), inner_value.value(1, 3)); 
				value.m_scale(0) = inner_value.value(2, 0); value.m_scale(1) = inner_value.value(2, 1); value.m_scale(2) = inner_value.value(2, 2); value.m_scale(3) = inner_value.value(2, 3);
			}
			_row -= 1;
		}

		template<>
		void Setter::visit(hkMatrix4& value) {
			if (_row == 0)
			{
				auto inner_value = _value.value<HkxItemReal>();
				value(0, 0) = inner_value.value(0, 0); value(0, 1) = inner_value.value(0, 1); value(0, 2) = inner_value.value(0, 2); value(0, 3) = inner_value.value(0, 3);
				value(1, 0) = inner_value.value(1, 0); value(1, 1) = inner_value.value(1, 1); value(1, 2) = inner_value.value(1, 2); value(1, 3) = inner_value.value(1, 3);
				value(2, 0) = inner_value.value(2, 0); value(2, 1) = inner_value.value(2, 1); value(2, 2) = inner_value.value(2, 2); value(2, 3) = inner_value.value(2, 3);
				value(3, 0) = inner_value.value(3, 0); value(3, 1) = inner_value.value(3, 1); value(3, 2) = inner_value.value(3, 2); value(3, 3) = inner_value.value(3, 3);
			}
			_row -= 1;
		}

		template<>
		void Setter::visit(hkTransform& value) {
			if (_row == 0)
			{
				auto inner_value = _value.value<HkxItemReal>();
				value(0, 0) = inner_value.value(0, 0); value(0, 1) = inner_value.value(0, 1); value(0, 2) = inner_value.value(0, 2); value(0, 3) = inner_value.value(0, 3);
				value(1, 0) = inner_value.value(1, 0); value(1, 1) = inner_value.value(1, 1); value(1, 2) = inner_value.value(1, 2); value(1, 3) = inner_value.value(1, 3);
				value(2, 0) = inner_value.value(2, 0); value(2, 1) = inner_value.value(2, 1); value(2, 2) = inner_value.value(2, 2); value(2, 3) = inner_value.value(2, 3);
				value(3, 0) = inner_value.value(3, 0); value(3, 1) = inner_value.value(3, 1); value(3, 2) = inner_value.value(3, 2); value(3, 3) = inner_value.value(3, 3);
			}
			_row -= 1;
		}

		template<>
		void Setter::visit(hkBool& value) {
			if (_row == 0)
				value = _value.value<bool>();
			_row -= 1;
		}

		template<>
		void Setter::visit(hkUlong& value) {
			if (_row == 0)
				value = _value.value<unsigned int>();
			_row -= 1;
		}

		template<>
		void Setter::visit(hkHalf& value) {
			if (_row == 0)
				value = _value.value<unsigned short>();
			_row -= 1;
		}

		template<>
		void Setter::visit(hkStringPtr& value) {
			if (_row == 0)
				value = _value.value<QString>().toUtf8().constData();
			_row -= 1;
		}

		template<>
		void Setter::visit(hkVariant& value) {
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

