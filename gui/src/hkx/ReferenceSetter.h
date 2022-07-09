#pragma once

#include "HkxItemVisitor.h"
#include "HkxVariant.h"
#include "RowCalculator.h"
#include "ColumnCalculator.h"
#include <src/items/HkxItemReal.h>

#include <src/hkx/HkxSpecialMembersMaps.h>

#include <QVariant>
#include <vector>

namespace ckcmd {
	namespace HKX {

		class ReferenceSetter : public HkxConcreteVisitor<ReferenceSetter> {
			int _array_index_to_replace;
			int _replace_with;
			MemberIndexType _target_type;

			bool _result = true;

			template<typename T>
			void check(T& value)
			{
				auto it = Utility::indexedMembersMap.find({ _class, _memberIndex });
				if (it != Utility::indexedMembersMap.end())
				{
					if (_target_type == it->second && value == _array_index_to_replace)
					{
						value = _replace_with;
					}
				}
			}

		public:

			ReferenceSetter(int array_index_to_replace, int replace_with, MemberIndexType target_type) :
				HkxConcreteVisitor(*this), 
				_array_index_to_replace(array_index_to_replace), 
				_replace_with(replace_with),
				_target_type(target_type)
			{}

			bool result() { return _result; }


			template<typename T>
			void visit(T& value)
			{
			}

			template<>
			void visit(hkInt8& value)
			{
				check(value);
			}

			template<>
			void visit(hkUint8& value)
			{
				check(value);
			}

			template<>
			void visit(hkInt16& value)
			{
				check(value);
			}

			template<>
			void visit(hkUint16& value)
			{
				check(value);
			}

			template<>
			void visit(hkInt32& value)
			{
				check(value);
			}

			template<>
			void visit(hkUint32& value)
			{
				check(value);
			}

			virtual void visit(char* value) override;
			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void* value, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* value, const hkClassEnum& enum_type, size_t storage) override;

		};
	}
}

