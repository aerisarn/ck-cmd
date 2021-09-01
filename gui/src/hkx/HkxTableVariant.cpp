#include "HkxTableVariant.h"

using namespace ckcmd::HKX;


void HkxTableVariant::accept(HkxItemVisitor& visitor) const
{
	visitor.setClass(_variant.m_class);
	if (hkReferencedObjectClass.isSuperClass(*_variant.m_class))
		visitor.setLastVariant(&_variant);
	for (size_t i = 0; i < _variant.m_class->getNumMembers(); i++) {
		const auto& member_declaration = _variant.m_class->getMember(i);
		visitor.setClassMember(&member_declaration);
		if (member_declaration.getFlags().get() & hkClassMember::SERIALIZE_IGNORED)
			continue;
		void* object = (void*)((char*)_variant.m_object + member_declaration.getOffset());
		switch (member_declaration.getType()) {
			/// No type
		case hkClassMember::TYPE_VOID:
			return;
		case hkClassMember::TYPE_BOOL:
			visitor.visit(*(hkBool*)object); break;
			/// hkChar, signed char type
		case hkClassMember::TYPE_CHAR:
			visitor.visit(*(hkChar*)object); break;
			/// hkInt8, 8 bit signed integer type
		case hkClassMember::TYPE_INT8:
			visitor.visit(*(hkInt8*)object); break;
			/// hkUint8, 8 bit unsigned integer type
		case hkClassMember::TYPE_UINT8:
			visitor.visit(*(hkUint8*)object); break;
			/// hkInt16, 16 bit signed integer type
		case hkClassMember::TYPE_INT16:
			visitor.visit(*(hkInt16*)object); break;
			/// hkUint16, 16 bit unsigned integer type
		case hkClassMember::TYPE_UINT16:
			visitor.visit(*(hkUint16*)object); break;
			/// hkInt32, 32 bit signed integer type
		case hkClassMember::TYPE_INT32:
			visitor.visit(*(hkInt32*)object); break;
			/// hkUint32, 32 bit unsigned integer type
		case hkClassMember::TYPE_UINT32:
			visitor.visit(*(hkUint32*)object); break;
			/// hkInt64, 64 bit signed integer type
		case hkClassMember::TYPE_INT64:
			visitor.visit(*(hkInt64*)object); break;
			/// hkUint64, 64 bit unsigned integer type
		case hkClassMember::TYPE_UINT64:
			visitor.visit(*(hkUint64*)object); break;
			/// hkReal, float type
		case hkClassMember::TYPE_REAL:
			visitor.visit(*(hkReal*)object); break;
			/// hkVector4 type
		case hkClassMember::TYPE_VECTOR4:
			visitor.visit(*(hkVector4*)object); break;
			/// hkQuaternion type
		case hkClassMember::TYPE_QUATERNION:
			visitor.visit(*(::hkQuaternion*)object); break;
			/// hkMatrix3 type
		case hkClassMember::TYPE_MATRIX3:
			visitor.visit(*(hkVector4*)object); break;
			/// hkRotation type
		case hkClassMember::TYPE_ROTATION:
			visitor.visit(*(hkRotation*)object); break;
			/// hkQsTransform type
		case hkClassMember::TYPE_QSTRANSFORM:
			visitor.visit(*(hkQsTransform*)object); break;
			/// hkMatrix4 type
		case hkClassMember::TYPE_MATRIX4:
			visitor.visit(*(hkMatrix4*)object); break;
			/// hkTransform type
		case hkClassMember::TYPE_TRANSFORM:
			visitor.visit(*(hkTransform*)object); break;
			/// Serialize as zero - deprecated.
		case hkClassMember::TYPE_ZERO:
			return;
			/// Generic pointer, see member flags for more info
		case hkClassMember::TYPE_POINTER:
			visitor.visit(object, *member_declaration.getClass(), member_declaration.getFlags()); break;
			/// Function pointer
		case hkClassMember::TYPE_FUNCTIONPOINTER:
			return;
			/// hkArray<T>, array of items of type T
		case hkClassMember::TYPE_ARRAY:
			visitor.visit(object, member_declaration); break;
			/// hkInplaceArray<T,N> or hkInplaceArrayAligned16<T,N>, array of N items of type T
		case hkClassMember::TYPE_INPLACEARRAY:
			visitor.visit(object, member_declaration); break;
			/// hkEnum<ENUM,STORAGE> - enumerated values
		case hkClassMember::TYPE_ENUM:
			visitor.visit(object, member_declaration.getEnumClass(), member_declaration.getSubType()); break;
			/// Object
		case hkClassMember::TYPE_STRUCT:
			visitor.visit(object, *member_declaration.getClass(), member_declaration.getName()); break;
			/// Simple array (ptr(typed) and size only)
		case hkClassMember::TYPE_SIMPLEARRAY:
			visitor.visit(object, member_declaration); break;
			/// Simple array of homogeneous types, so is a class id followed by a void* ptr and size
		case hkClassMember::TYPE_HOMOGENEOUSARRAY:
			visitor.visit(object, member_declaration); break;
			/// hkVariant (void* and hkClass*) type
		case hkClassMember::TYPE_VARIANT:
			visitor.visit(*(hkVariant*)object); break;
			/// char*, null terminated string
		case hkClassMember::TYPE_CSTRING:
			visitor.visit((char*)(*(uintptr_t*)object)); break;
			/// hkUlong, unsigned long, defined to always be the same size as a pointer
		case hkClassMember::TYPE_ULONG:
			visitor.visit(*(hkUlong*)object); break;
			/// hkFlags<ENUM,STORAGE> - 8,16,32 bits of named values.
		case hkClassMember::TYPE_FLAGS:
			visitor.visit(object, member_declaration.getEnumClass(), member_declaration.getSizeInBytes()); break;
			/// hkHalf, 16-bit float value
		case hkClassMember::TYPE_HALF:
			visitor.visit(*(hkHalf*)object); break;
			/// hkStringPtr, c-string
		case hkClassMember::TYPE_STRINGPTR:
			visitor.visit(*(hkStringPtr*)object); break;
			/// hkRelArray<>, attached const array values
		default:
			break;
		}
	}
}

void HkxTableVariant::accept(HkxItemVisitor& visitor) {
	const_cast<const HkxTableVariant*>(this)->accept(visitor);
}