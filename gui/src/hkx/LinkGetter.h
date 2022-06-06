#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/hkx/HkxItemVisitor.h>
#include <src/hkx/HkxVariant.h>

namespace ckcmd {
	namespace HKX {

		class LinkGetter : public HkxConcreteVisitor<LinkGetter> {

			std::vector<hkVariant> _links;
			ResourceManager& _resourceManager;
			size_t _file;

		public:
			LinkGetter(
				size_t file,
				ResourceManager& resourceManager
			);

			template<typename T>
			void visit(T& value) {}

			void visit(char* value);

			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;

		};
	}
}