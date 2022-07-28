#include <src/hkx/ArrayFinder.h>
#include <src/hkx/RowCalculator.h>

#include <hkbGenerator_0.h>
#include <hkbModifier_0.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbBlenderGeneratorChild_2.h>

#include <typeinfo>

using namespace ckcmd::HKX;

ArrayFinder::ArrayFinder() :
	HkxConcreteVisitor(*this)
{
}

const std::vector<int>& ArrayFinder::arrayrows() const
{
	return _arrayrows;
}

void ArrayFinder::visit(char*& value) {}

void ArrayFinder::visit(void* object, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	if (!hkReferencedObjectClass.isSuperClass(pointer_type))
	{
		hkVariant v;
		v.m_class = &pointer_type;
		v.m_object = object;
		HkxVariant h(v);
		h.accept(*this);
	}
	else {
		_row += 1;
	}
}

void ArrayFinder::visit(void* object, const hkClassMember& definition) {

	const auto& arraytype = definition.getArrayType();
	const auto& arrayclass = definition.getStructClass();
	const auto& arraysubtype = definition.getSubType();
	if (arraysubtype == hkClassMember::Type::TYPE_VOID)
		return;
	auto size = definition.getSizeInBytes();
	auto arraymembersize = definition.getArrayMemberSize();
	if (arraymembersize > 0)
	{
		int elements = size / arraymembersize;
		bool isArray = false;
		if (hkClassMember::TYPE_ARRAY == definition.getType())
		{
			isArray = true;
		}
		if (definition.hasClass()) {
			RowCalculator r;
			hkVariant v;
			v.m_class = definition.getClass();
			v.m_object = nullptr;
			HkxVariant h(v);
			h.accept(r);
			for (int i = 0; i < r.rows(); ++i)
			{
				if (isArray)
					_arrayrows.push_back(_row);
				_row += 1;
			}
		}
		else {
			if (isArray)
				_arrayrows.push_back(_row);
			_row += 1;
		}
	}
}

void ArrayFinder::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) 
{
	_row += 1;
}

void ArrayFinder::visit(void* object, const hkClass& object_type, const char* member_name)
{
	hkVariant v;
	v.m_class = &object_type;
	v.m_object = object;
	visit(v.m_object, *v.m_class, hkClassMember::Flags());
}

void ArrayFinder::visit(void* v, const hkClassEnum& enum_type, size_t storage) 
{
	_row += 1;
}