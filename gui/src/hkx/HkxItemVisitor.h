#pragma once

#include <core\hkxpch.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemVisitor {
		public:
			/// hkBool, boolean type
			virtual void visit(hkBool&) = 0;
			/// hkChar, signed char type
			virtual void visit(hkChar&) = 0;
			/// hkUint8, 8 bit unsigned integer type
			virtual void visit(hkUint8&) = 0;
			/// hkInt16, 16 bit signed integer type
			virtual void visit(hkInt16&) = 0;
			/// hkUint16, 16 bit unsigned integer type
			virtual void visit(hkUint16&) = 0;
			/// hkInt32, 32 bit signed integer type
			virtual void visit(hkInt32&) = 0;
			/// hkUint32, 32 bit unsigned integer type
			virtual void visit(hkUint32&) = 0;
			/// hkInt64, 64 bit signed integer type
			virtual void visit(hkInt64&) = 0;
			/// hkUint64, 64 bit unsigned integer type
			virtual void visit(hkUint64&) = 0;
			/// hkReal, float type
			virtual void visit(hkReal&) = 0;
			/// hkVector4 type
			virtual void visit(hkVector4&) = 0;
			/// hkQuaternion type
			virtual void visit(::hkQuaternion&) = 0;
			/// hkMatrix3 type
			virtual void visit(hkMatrix3&) = 0;
			/// hkRotation type
			virtual void visit(hkRotation&) = 0;
			/// hkQsTransform type
			virtual void visit(hkQsTransform&) = 0;
			/// hkMatrix4 type
			virtual void visit(hkMatrix4&) = 0;
			/// hkTransform type
			virtual void visit(hkTransform&) = 0;
			/// Serialize as zero - deprecated.
			//case hkClassMember::TYPE_ZERO:
			/// Generic pointer, see member flags for more info
			virtual void visit(void*, const hkClass& pointer_type, hkClassMember::Flags flags) = 0;
			/// Function pointer
			//case hkClassMember::TYPE_FUNCTIONPOINTER:
			/// hkArray<T>, array of items of type T
			/// hkInplaceArray<T,N> or hkInplaceArrayAligned16<T,N>, array of N items of type T
			/// Simple array (ptr(typed) and size only)
			/// Simple array of homogeneous types, so is a class id followed by a void* ptr and size
			/// hkRelArray<>, attached const array values
			virtual void visit(void*, const hkClassMember& definition) = 0;
			//	/// hkEnum<ENUM,STORAGE> - enumerated values
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) = 0;
			/// Object
			virtual void visit(void*, const hkClass& object_type, const char* member_name) = 0;
			/// hkVariant (void* and hkClass*) type
			virtual void visit(hkVariant&) = 0;
			/// char*, null terminated string
			virtual void visit(char*) = 0;
			/// hkUlong, unsigned long, defined to always be the same size as a pointer
			virtual void visit(hkUlong&) = 0;
			/// hkFlags<ENUM,STORAGE> - 8,16,32 bits of named values.
			virtual void visit(void*, const hkClassEnum& enum_type, size_t storage) = 0;
			/// hkHalf, 16-bit float value
			virtual void visit(hkHalf&) = 0;
			/// hkStringPtr, c-string
			virtual void visit(hkStringPtr&) = 0;

			virtual void setClass(const hkClass* hkclass) = 0;
			virtual void setClassMember(const hkClassMember* hkclassmember) = 0;
			virtual void setLastVariant(const hkVariant* variant) = 0;
		};

		template<typename T>
		class HkxConcreteVisitor : public HkxItemVisitor {
		protected:
			T& _handler;
			const hkClass* _class;
			const hkClassMember* _classmember;
			const hkVariant* _lastVariant;
		public:
			HkxConcreteVisitor(T& handler) : _handler(handler) {}

			/// hkBool, boolean type
			virtual void visit(hkBool& v) override { _handler.visit(v); }
			/// hkChar, signed char type
			virtual void visit(hkChar& v) override { _handler.visit(v); }
			/// hkUint8, 8 bit unsigned integer type
			virtual void visit(hkUint8& v) override { _handler.visit(v); }
			/// hkInt16, 16 bit signed integer type
			virtual void visit(hkInt16& v) override { _handler.visit(v); }
			/// hkUint16, 16 bit unsigned integer type
			virtual void visit(hkUint16& v) override { _handler.visit(v); }
			/// hkInt32, 32 bit signed integer type
			virtual void visit(hkInt32& v) override { _handler.visit(v); }
			/// hkUint32, 32 bit unsigned integer type
			virtual void visit(hkUint32& v) override { _handler.visit(v); }
			/// hkInt64, 64 bit signed integer type
			virtual void visit(hkInt64& v) override { _handler.visit(v); }
			/// hkUint64, 64 bit unsigned integer type
			virtual void visit(hkUint64& v) override { _handler.visit(v); }
			/// hkReal, float type
			virtual void visit(hkReal& v) override { _handler.visit(v); }
			/// hkVector4 type
			virtual void visit(hkVector4& v) override { _handler.visit(v); }
			/// hkQuaternion type
			virtual void visit(::hkQuaternion& v) override { _handler.visit(v); }
			/// hkMatrix3 type
			virtual void visit(hkMatrix3& v) override { _handler.visit(v); }
			/// hkRotation type
			virtual void visit(hkRotation& v) override { _handler.visit(v); }
			/// hkQsTransform type
			virtual void visit(hkQsTransform& v) override { _handler.visit(v); }
			/// hkMatrix4 type
			virtual void visit(hkMatrix4& v) override { _handler.visit(v); }
			/// hkTransform type
			virtual void visit(hkTransform& v) override { _handler.visit(v); }
			/// Serialize as zero - deprecated.
			//case hkClassMember::TYPE_ZERO:
			/// Generic pointer, see member flags for more info
			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override
			{
				_handler.visit(v, pointer_type, flags);
			}
			/// Function pointer
			//case hkClassMember::TYPE_FUNCTIONPOINTER:
			/// hkArray<T>, array of items of type T
			/// hkInplaceArray<T,N> or hkInplaceArrayAligned16<T,N>, array of N items of type T
			/// Simple array (ptr(typed) and size only)
			/// Simple array of homogeneous types, so is a class id followed by a void* ptr and size
			/// hkRelArray<>, attached const array values
			virtual void visit(void* v, const hkClassMember& definition) override
			{
				_handler.visit(v, definition);
			}
			//	/// hkEnum<ENUM,STORAGE> - enumerated values
			virtual void visit(void* v, const hkClassEnum& enum_type, hkClassMember::Type type) override
			{
				_handler.visit(v, enum_type, type);
			}
			virtual void visit(void* v, const hkClass& object_type, const char* member_name) override
			{
				_handler.visit(v, object_type, member_name);
			}
			/// hkVariant (void* and hkClass*) type
			virtual void visit(hkVariant& v) override { _handler.visit(v); }
			/// char*, null terminated string
			virtual void visit(char* v) override { _handler.visit(v); }
			/// hkUlong, unsigned long, defined to always be the same size as a pointer
			virtual void visit(hkUlong& v) override { _handler.visit(v); }
			/// hkFlags<ENUM,STORAGE> - 8,16,32 bits of named values.
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override
			{
				_handler.visit(v, enum_type, storage);
			}
			/// hkHalf, 16-bit float value
			virtual void visit(hkHalf& v) override { _handler.visit(v); }
			/// hkStringPtr, c-string
			virtual void visit(hkStringPtr& v) override { _handler.visit(v); }

			//recursion on scalars
			virtual void recurse(void* object, const hkClassMember::Type type) {
				switch (type) {
					/// No type
				case hkClassMember::TYPE_VOID:
					return;
				case hkClassMember::TYPE_BOOL:
					_handler.visit(*(hkBool*)object); break;
					/// hkChar, signed char type
				case hkClassMember::TYPE_CHAR:
					_handler.visit(*(hkChar*)object); break;
					/// hkInt8, 8 bit signed integer type
				case hkClassMember::TYPE_INT8:
					_handler.visit(*(hkInt8*)object); break;
					/// hkUint8, 8 bit unsigned integer type
				case hkClassMember::TYPE_UINT8:
					_handler.visit(*(hkUint8*)object); break;
					/// hkInt16, 16 bit signed integer type
				case hkClassMember::TYPE_INT16:
					_handler.visit(*(hkInt16*)object); break;
					/// hkUint16, 16 bit unsigned integer type
				case hkClassMember::TYPE_UINT16:
					_handler.visit(*(hkUint16*)object); break;
					/// hkInt32, 32 bit signed integer type
				case hkClassMember::TYPE_INT32:
					_handler.visit(*(hkInt32*)object); break;
					/// hkUint32, 32 bit unsigned integer type
				case hkClassMember::TYPE_UINT32:
					_handler.visit(*(hkUint32*)object); break;
					/// hkInt64, 64 bit signed integer type
				case hkClassMember::TYPE_INT64:
					_handler.visit(*(hkInt64*)object); break;
					/// hkUint64, 64 bit unsigned integer type
				case hkClassMember::TYPE_UINT64:
					_handler.visit(*(hkUint64*)object); break;
					/// hkReal, float type
				case hkClassMember::TYPE_REAL:
					_handler.visit(*(hkReal*)object); break;
					/// hkVector4 type
				case hkClassMember::TYPE_VECTOR4:
					_handler.visit(*(hkVector4*)object); break;
					/// hkQuaternion type
				case hkClassMember::TYPE_QUATERNION:
					_handler.visit(*(::hkQuaternion*)object); break;
					/// hkMatrix3 type
				case hkClassMember::TYPE_MATRIX3:
					_handler.visit(*(hkVector4*)object); break;
					/// hkRotation type
				case hkClassMember::TYPE_ROTATION:
					_handler.visit(*(hkRotation*)object); break;
					/// hkQsTransform type
				case hkClassMember::TYPE_QSTRANSFORM:
					_handler.visit(*(hkQsTransform*)object); break;
					/// hkMatrix4 type
				case hkClassMember::TYPE_MATRIX4:
					_handler.visit(*(hkMatrix4*)object); break;
					/// hkTransform type
				case hkClassMember::TYPE_TRANSFORM:
					_handler.visit(*(hkTransform*)object); break;
					/// char*, null terminated string
				case hkClassMember::TYPE_CSTRING:
					_handler.visit((char*)object); break;
					/// hkUlong, unsigned long, defined to always be the same size as a pointer
				case hkClassMember::TYPE_ULONG:
					_handler.visit(*(hkUlong*)object); break;
					/// hkFlags<ENUM,STORAGE> - 8,16,32 bits of named values.
				case hkClassMember::TYPE_HALF:
					_handler.visit(*(hkHalf*)object); break;
					/// hkStringPtr, c-string
				case hkClassMember::TYPE_STRINGPTR:
					_handler.visit(*(hkStringPtr*)object); break;
					/// hkRelArray<>, attached const array values
				default:
					return;
				}
			}

			virtual void setClass(const hkClass* hkclass) {
				_class = hkclass;
			}

			virtual void setClassMember(const hkClassMember* hkclassmember) {
				_classmember = hkclassmember;
			}

			virtual void setLastVariant(const hkVariant* variant) {
				_lastVariant = variant;
			}
		};

	}
}