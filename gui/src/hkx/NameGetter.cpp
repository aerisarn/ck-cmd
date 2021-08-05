#include "NameGetter.h"

#include "HkxTableVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"

using namespace ckcmd::HKX;

QString NameGetter::name() { 
	return _parent_prefix.empty() ? 
		_name :
		QString::fromStdString(_parent_prefix) + QString(".") + _name; 
}

const hkClassMember&  NameGetter::getSerializedMember(size_t row) {
	size_t actual_row = 0;
	for (size_t i = 0; i < _class->getNumMembers(); i++) {
		const auto& member_declaration = _class->getMember(i);
		if (member_declaration.getFlags().get() & hkClassMember::SERIALIZE_IGNORED)
			continue;
		if (actual_row == row)
			return _class->getMember(i);
		actual_row++;
	}
	return hkClassMember();
}

template<typename T>
void NameGetter::visit(T& value)
{
	if (_target_row == 0)
		_name = QString::fromStdString(getSerializedMember(_rows).getName());
	_rows += 1;
	_target_row -= 1;
}

void NameGetter::visit(char* value)
{
	if (_target_row == 0)
		_name = QString::fromStdString(getSerializedMember(_rows).getName());
	_rows += 1;
	_target_row -= 1;
}

void NameGetter::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	if (_target_row == 0)
		_name = QString::fromStdString(getSerializedMember(_rows).getName());
	_rows += 1;
	_target_row -= 1;
}

void NameGetter::visit(void* object, const hkClassMember& definition)
{
	const auto& arrayclass = definition.getStructClass();
	const auto& arraytype = definition.getArrayType();
	const auto& arraysubtype = definition.getSubType();

	if (definition.hasClass() && 
		hkClassMember::TYPE_POINTER != arraysubtype)
	{
		hkVariant v;
		v.m_class = definition.getClass();
		v.m_object = object;
		HkxTableVariant h(v);
		
		RowCalculator r;
		h.accept(r);
		int class_rows = r.rows();

		NameGetter n(_target_row, definition.getName());
		h.accept(n);
		if (_target_row >= 0 && _target_row - class_rows <= 0)
			_name = n.name();
		_rows += 1;
		_target_row -= class_rows;
		

	}
	else {
		if (_target_row == 0)
			_name = QString::fromStdString(getSerializedMember(_rows).getName());
		_rows += 1;
		_target_row -= 1;
	}
}

void NameGetter::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type)
{
	if (_target_row == 0)
		_name = QString::fromStdString(getSerializedMember(_rows).getName());
	_rows += 1;
	_target_row -= 1;
}

void NameGetter::visit(void* object, const hkClass& object_type, const char* member_name)
{
	if (&object_type != &hkReferencedObjectClass)
	{
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxTableVariant h(v);
		RowCalculator r;
		h.accept(r);
		NameGetter n(_target_row, member_name);
		h.accept(n);
		int after_object = _target_row - r.rows();
		if (_target_row >= 0 && after_object < 0)
			_name = n.name();
		_target_row -= r.rows();
	}
	else {
		_target_row -= 1;
	}
	_rows += 1;
}

void NameGetter::visit(void* v, const hkClassEnum& enum_type, size_t storage)
{
	if (_target_row == 0)
		_name = QString::fromStdString(getSerializedMember(_rows).getName());
	_rows += 1;
	_target_row -= 1;
}