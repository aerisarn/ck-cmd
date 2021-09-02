#pragma once

#include "TreeBuilder.h"
#include "ISpecialFieldsHandler.h"
#include "SkeletonBuilder.h"

#include <hkbBehaviorGraphData_2.h>
#include <hkbBehaviorGraphStringData_1.h>


namespace ckcmd {
	namespace HKX {
		class BehaviorBuilder : 
			public ITreeBuilderClassHandler, 
			public ISpecialFieldsHandler
		{

			struct buildContext {
				const hkbBehaviorGraphData* data;
				const hkbBehaviorGraphStringData* string_data;
				const fs::path& _file;
				int object_index;
				ProjectNode* parent;
				ResourceManager& resourceManager;
			};

			virtual std::vector<member_id_t> getEventFields();
			virtual std::vector<member_id_t> getVariableFields();

			void buildEvents(const buildContext& context);
			void buildVariables(const buildContext& context);
			void buildProperties(const buildContext& context);

			SkeletonBuilder* _skeleton_builder;
			hkbBehaviorGraphStringData* _strings;
			std::set<std::string> _referenced_behaviors;

			ProjectNode* buildBranch(hkVariant& variant, ProjectNode* root_node, const fs::path& path, ResourceManager& _resourceManager);

		public:

			const std::set<std::string>& referenced_behaviors() { return _referenced_behaviors; }

			virtual std::vector<const hkClass*> getHandledClasses() override;

			virtual ProjectNode* visit(
				const fs::path& _file,
				int object_index,
				ProjectNode* parent,
				ResourceManager& resourceManager) override;

			virtual std::vector<member_id_t> getHandledFields() override;

			virtual QVariant handle(void* value, const hkClass*, const hkClassMember*, const hkVariant*, const hkVariant* parent_container) override;

			SkeletonBuilder* skeletonBuilder() { return _skeleton_builder; }

			void setSkeleton(SkeletonBuilder* skeleton_builder) { _skeleton_builder = skeleton_builder; }

		};
	}
}

