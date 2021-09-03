#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/hkx/HkxItemVisitor.h>
#include <src/hkx/HkxTableVariant.h>
#include <src/models/ProjectNode.h>
#include <src/hkx/ITreeBuilderClassHandler.h>
#include <queue>

namespace ckcmd {
	namespace HKX {

		class TreeBuilder : public HkxConcreteVisitor<TreeBuilder> {
			ResourceManager& _resourceManager;
			const fs::path& _file;
			size_t _file_index;
			ProjectNode* _parent;
			std::set<void*>& _visited_objects;
			std::map<const hkClass*, ITreeBuilderClassHandler*> _handlers;
		public:
			TreeBuilder(
				ProjectNode* parent, 
				ResourceManager& resourceManager,
				const fs::path& file,
				std::set<void*>& _visited_objects
			);

			TreeBuilder(
				ProjectNode* parent,
				ResourceManager& resourceManager,
				const fs::path& file,
				std::set<void*>& _visited_objects,
				const std::map<const hkClass*, ITreeBuilderClassHandler*>& _handlers
			);

			TreeBuilder(
				ProjectNode* parent,
				ResourceManager& resourceManager,
				const fs::path& file,
				size_t _file_index,
				std::set<void*>& _visited_objects,
				const std::map<const hkClass*, ITreeBuilderClassHandler*>& _handlers
			);

			template<typename T>
			void visit(T& value) {}

			void visit(char* value);

			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;


			void registerClassHandler(const hkClass* hk_class, ITreeBuilderClassHandler* handler) { _handlers[hk_class] = handler; }
			void unregisterClassHandler(const hkClass* hk_class, ITreeBuilderClassHandler* handler) { _handlers.erase(hk_class); }

			void registerClassHandler(ITreeBuilderClassHandler* handler) {
				for (auto hk_class : handler->getHandledClasses())
					registerClassHandler(hk_class, handler);
			}
			void unregisterClassHandler(ITreeBuilderClassHandler* handler) {
				for (auto hk_class : handler->getHandledClasses())
					unregisterClassHandler(hk_class, handler);
			}

		};
	}
}