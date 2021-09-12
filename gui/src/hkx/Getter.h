#pragma once

#include "HkxItemVisitor.h"
#include "HkxTableVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"
#include "HkxItemReal.h"

#include <src/hkx/ISpecialFieldsHandler.h>

#include <QVariant>
#include <vector>

namespace ckcmd {
	namespace HKX {

		class Getter : public HkxConcreteVisitor<Getter>, public SpecialFieldsListener {
			QVariant _value;
			int _row;
			int _column;
			int _file_index;

			template<typename T>
			void check(T& value);
			void check(void* value);

		public:

			Getter(const size_t row, const size_t column, int file_index) :
				HkxConcreteVisitor(*this), 
				_row(row), _column(column), _file_index(file_index) {}

			Getter(const size_t row, const size_t column, int file_index, const std::map<member_id_t, ISpecialFieldsHandler*>& handlers) :
				HkxConcreteVisitor(*this),
				_row(row), _column(column), _file_index(file_index) 
			{
				_handlers = handlers;
			}

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
			{
				if (_handlers.find({ _class, _classmember }) != _handlers.end())
				{
					_value = _handlers[{_class, _classmember}]->value(value, _class, _classmember, _lastVariant);
				}
				else {
					_value = value;
				}
			}
			_row -= 1;
		}

		template<typename T>
		void Getter::visit(T& value) {
			check(value);
		}

		template<>
		void Getter::visit(hkVector4& value) {
			if (_row == 0)
				_value.setValue(HkxItemReal({ { value(0), value(1), value(2) } }));
			_row -= 1;
		}

		template<>
		void Getter::visit(::hkQuaternion& value) {
			if (_row == 0)
				_value.setValue(HkxItemReal({ { value(0), value(1), value(2), value(3) } }));
			_row -= 1;
		}

		template<>
		void Getter::visit(hkMatrix3& value) {
			if (_row == 0)
				_value.setValue(HkxItemReal(
					{ 
						{ value(0,0), value(0,1), value(0,2) },
						{ value(1,0), value(1,1), value(1,2) },
						{ value(2,0), value(2,1), value(2,2) }
					}
				));
			_row -= 1;
		}

		template<>
		void Getter::visit(hkRotation& value) {
			if (_row == 0)
				_value.setValue(HkxItemReal(
					{
						{ value(0,0), value(0,1), value(0,2) },
						{ value(1,0), value(1,1), value(1,2) },
						{ value(2,0), value(2,1), value(2,2) }
					}
				));
			_row -= 1;
		}

		template<>
		void Getter::visit(hkQsTransform& value) {
			if (_row == 0)
				_value.setValue(HkxItemReal(
					{
						{ value.m_translation(0), value.m_translation(1), value.m_translation(2),  value.m_translation(3) },
						{ value.m_rotation(0), value.m_rotation(1), value.m_rotation(2),  value.m_rotation(3) },
						{ value.m_scale(0), value.m_scale(1), value.m_scale(2),  value.m_scale(3) }
					}
			));
			_row -= 1;
		}

		template<>
		void Getter::visit(hkMatrix4& value) {
			if (_row == 0)
				_value.setValue(HkxItemReal(
					{
						{ value(0,0), value(0,1), value(0,2), value(0,3) },
						{ value(1,0), value(1,1), value(1,2), value(1,3) },
						{ value(2,0), value(2,1), value(2,2), value(2,3) },
						{ value(3,0), value(3,1), value(3,2), value(3,3) }
					}
			));
			_row -= 1;
		}

		template<>
		void Getter::visit(hkTransform& value) {
			if (_row == 0)
				_value.setValue(HkxItemReal(
					{
						{ value(0,0), value(0,1), value(0,2), value(0,3) },
						{ value(1,0), value(1,1), value(1,2), value(1,3) },
						{ value(2,0), value(2,1), value(2,2), value(2,3) },
						{ value(3,0), value(3,1), value(3,2), value(3,3) }
					}
			));
			_row -= 1;
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
				if (NULL != value.cString())
					_value = QString::fromStdString(value.cString());
				else
					_value = "";
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
				h.accept(*this);
			}
			_row -= array_rows;
		}
	}
}

