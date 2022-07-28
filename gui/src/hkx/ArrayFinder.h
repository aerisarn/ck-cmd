#pragma once

#include <src/hkx/HkxItemVisitor.h>
#include <src/hkx/HkxVariant.h>

#include <vector>

namespace ckcmd {
	namespace HKX {


		class ArrayFinder : public HkxConcreteVisitor<ArrayFinder> {

			std::vector<int> _arrayrows;
			int _row = 0;

			bool ignoreNotSerializable = true;

		public:


			ArrayFinder();

			const std::vector<int>& arrayrows() const;

			void setIgnoreNotSerializable(bool value) { ignoreNotSerializable = value; }

			virtual bool ignoreNotSerializables() const override
			{
				return ignoreNotSerializable;
			}

			template<typename T>
			void visit(T& value) 
			{
				_row += 1;
			}

			void visit(char*& value);

			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;

		};
	}
}