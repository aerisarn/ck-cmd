#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/hkx/HkxItemVisitor.h>
#include <src/hkx/HkxTableVariant.h>
#include <src/models/ProjectNode.h>

namespace ckcmd {
	namespace HKX {

		class TreeBuilder : public HkxConcreteVisitor<TreeBuilder> {
			ResourceManager& _resourceManager;
			const fs::path& _file;
			ProjectNode* _parent;
			std::set<void*>& _visited_objects;
		public:
			TreeBuilder(
				ProjectNode* parent, 
				ResourceManager& resourceManager,
				const fs::path& file,
				std::set<void*>& _visited_objects
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