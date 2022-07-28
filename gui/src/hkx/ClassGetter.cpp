#include "ClassGetter.h"

#include "HkxVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"

using namespace ckcmd::HKX;

template<typename T>
void ClassGetter::visit(T& value)
{
	if (_target_row == 0)
	{
		_type = { _lastType, nullptr };
	}
	_rows += 1;
	_target_row -= 1;
}

void ClassGetter::visit(char*& value)
{
	if (_target_row == 0)
	{
		_type = { _lastType, nullptr };
	}
	_rows += 1;
	_target_row -= 1;
}

void ClassGetter::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	if (_target_row == 0)
	{
		_type = { hkClassMember::Type::TYPE_STRUCT, &_classmember->getStructClass() };
	}
	_rows += 1;
	_target_row -= 1;
}

void ClassGetter::visit(void* object, const hkClassMember& definition)
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
		HkxVariant h(v);
		
		RowCalculator r;
		h.accept(r);
		int class_rows = r.rows();

		ClassGetter n(_target_row);
		h.accept(n);
		if (_target_row >= 0 && _target_row - class_rows <= 0)
			_type = n.hkclass();
		_rows += 1;
		_target_row -= class_rows;
		

	}
	else {
		if (_target_row == 0)
			_type = { hkClassMember::Type::TYPE_STRUCT, &arrayclass };
		_rows += 1;
		_target_row -= 1;
	}
}

void ClassGetter::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type)
{
	if (_target_row == 0)
	{
		_type = { hkClassMember::Type::TYPE_STRUCT, &_classmember->getStructClass() };
	}
	_rows += 1;
	_target_row -= 1;
}

void ClassGetter::visit(void* object, const hkClass& object_type, const char* member_name)
{
	if (&object_type != &hkReferencedObjectClass)
	{
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxVariant h(v);
		RowCalculator r;
		h.accept(r);
		ClassGetter n(_target_row);
		h.accept(n);
		int after_object = _target_row - r.rows();
		if (_target_row >= 0 && after_object < 0)
			_type = n.hkclass();
		_target_row -= r.rows();
	}
	else {
		_type = { hkClassMember::Type::TYPE_STRUCT, &_classmember->getStructClass() };
		_target_row -= 1;
	}
	_rows += 1;
}

void ClassGetter::visit(void* v, const hkClassEnum& enum_type, size_t storage)
{
	if (_target_row == 0)
	{
		_type = { hkClassMember::Type::TYPE_STRUCT, &_classmember->getStructClass() };
	}
	_rows += 1;
	_target_row -= 1;
}