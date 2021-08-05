#pragma once

#include "HkxItemVisitor.h"
#include "HkxTableVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"

#include <QVariant>

namespace ckcmd {
	namespace HKX {

		class Getter : public HkxConcreteVisitor<Getter> {
			QVariant _value;
			int _row;
			int _column;

			template<typename T>
			void check(T& value);
			void check(void* value);

		public:

			Getter(const size_t row, const size_t column) : 
				HkxConcreteVisitor(*this), 
				_row(row), _column(column) {}

			QVariant value() { return _value; }

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
		void Getter::check(T& value) {
			if (_row == 0)
				_value = value;
			_row -= 1;
		}

		template<typename T>
		void Getter::visit(T& value) {
			check(value);
		}

		template<>
		void Getter::visit(hkVector4& value) {
			if (_column < 4)
			{
				if (_row == 0)
					_value = value(_column);
				_row -= 1;
			}
		}

		template<>
		void Getter::visit(::hkQuaternion& value) {
			if (_column < 4)
			{
				if (_row == 0)
					_value = value(_column);
				_row -= 1;
			}
		}

		template<>
		void Getter::visit(hkMatrix3& value) {
			if (_column < 9)
			{
				if (_row == 0)
					_value = value(_column % 3, _column / 3);
				_row -= 1;
			}
		}

		template<>
		void Getter::visit(hkRotation& value) {
			if (_column < 9)
			{
				if (_row == 0)
					_value = value(_column % 3, _column / 3);
				_row -= 1;
			}
		}

		template<>
		void Getter::visit(hkQsTransform& value) {
			if (_column < 12)
			{
				if (_row == 0)
				{
					if (_column / 3 == 0)
						_value = value.m_translation(_column % 4);
					else if (_column / 3 == 1)
						_value = value.m_rotation(_column % 4);
					else if (_column / 3 == 2)
						_value = value.m_scale(_column % 4);
				}
				_row -= 1;
			}
		}

		template<>
		void Getter::visit(hkMatrix4& value) {
			if (_column < 16)
			{
				if (_row == 0)
					_value = value(_column % 4, _column / 4);
				_row -= 1;
			}
		}

		template<>
		void Getter::visit(hkTransform& value) {
			if (_column < 16)
			{
				if (_row == 0)
					_value = value(_column % 4, _column / 4);
				_row -= 1;
			}
		}

		template<>
		void Getter::visit(hkBool& value) {
			if (_row == 0)
				_value = value.operator bool();
			_row -= 1;
		}

		template<>
		void Getter::visit(hkUlong& value) {
			if (_row == 0)
				_value = (unsigned int)value;
			_row -= 1;
		}

		template<>
		void Getter::visit(hkHalf& value) {
			if (_row == 0)
				_value = (unsigned short)value;
			_row -= 1;
		}

		template<>
		void Getter::visit(hkStringPtr& value) {
			if (_row == 0)
				_value = QString::fromStdString(value.cString());
			_row -= 1;
		}

		template<>
		void Getter::visit(hkVariant& value) {
			HkxTableVariant h(value);
			RowCalculator r;
			h.accept(r);
			size_t array_rows = r.rows();
			int next_rows = _row - array_rows;
			if (next_rows <= 0)
			{
				Getter g(_row, _column);
				h.accept(g);
				_value = g.value();
			}
			_row -= array_rows;
		}
	}
}

