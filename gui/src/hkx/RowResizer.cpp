#include "RowResizer.h"

#include "HkxVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"

using namespace ckcmd::HKX;

template<typename T>
void RowResizer::visit(T& value)
{
	_target_row -= 1;
}

void RowResizer::visit(char* value)
{
	_target_row -= 1;
}

void RowResizer::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	_target_row -= 1;
}

void RowResizer::visit(void* object, const hkClassMember& definition)
{
	const auto& arrayclass = definition.getStructClass();
	const auto& arraytype = definition.getArrayType();
	const auto& arraysubtype = definition.getSubType();

	int resizeByts = 0;
	bool isClass = false;

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


		if (_target_row >= 0 && _target_row - class_rows <= 0)
		{
			//array of classes;
			resizeByts = definition.getClass()->getObjectSize();
			isClass = true;
		}
		_target_row -= class_rows;
	}
	else {
		if (_target_row == 0)
		{
			//array of pointers
			resizeByts = sizeof(uintptr_t);
		}
		_target_row -= 1;
	}
	if (resizeByts > 0)
	{
		char* m_data = (char*)*(uintptr_t*)object; //m_data
		int* elements_ptr = (int*)((char*)object + sizeof(uintptr_t)); // m_size
		int* m_capacity_ptr = (int*)((char*)object + 2 * sizeof(uintptr_t)); // m_capacity_flags
		int elements = *elements_ptr & int(0x3FFFFFFF);
		int flags = *m_capacity_ptr & int(0xC0000000);
		int new_size = (elements + _delta) * resizeByts;
		if (new_size > elements * resizeByts)
		{
			auto& allocator = hkContainerHeapAllocator::s_alloc;
			uintptr_t* new_buffer = (uintptr_t*)allocator.bufAlloc(/*m_data, elements * resizeByts,*/ new_size);
			memcpy(new_buffer, m_data, elements * resizeByts);
			for (int i = elements; i < (elements + _delta); ++i)
			{
				char* object = (char*)new_buffer + i * resizeByts;
				memset(object, 0, resizeByts);
				if (isClass)
				{
					auto info = hkTypeInfoRegistry::getInstance().finishLoadedObject(object, definition.getClass()->getName());
				}
			}
			if (nullptr != new_buffer && new_size == ((elements + _delta) * resizeByts))
			{
				*(uintptr_t*)object = (uintptr_t)new_buffer;
				*elements_ptr = (elements + _delta);
				*m_capacity_ptr = *elements_ptr | flags;
				_result = true;
			}
		}
		else if (new_size < elements * resizeByts) {
			//move elements
			if (_column < elements - 1)
			{
				char* destination = m_data + _column * resizeByts;
				char* source = m_data + (_column + 1) * resizeByts;
				int size = ((elements - 1) - (_column));
				memcpy(destination, source, size + resizeByts);
			}
			//just shrink
			*elements_ptr = (elements + _delta);
			_result = true;
		}
	}
}

void RowResizer::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type)
{
	_target_row -= 1;
}

void RowResizer::visit(void* object, const hkClass& object_type, const char* member_name)
{
	if (&object_type != &hkReferencedObjectClass)
	{
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxVariant h(v);
		RowCalculator r;
		h.accept(r);
		if (_target_row >= 0 && _target_row < r.rows())
		{
			RowResizer n(_target_row, _column, _delta);
			h.accept(n);
		}
		else {
			_target_row -= r.rows();
		}
	}
	else {
		_target_row -= 1;
	}
}

void RowResizer::visit(void* v, const hkClassEnum& enum_type, size_t storage)
{
	_target_row -= 1;
}