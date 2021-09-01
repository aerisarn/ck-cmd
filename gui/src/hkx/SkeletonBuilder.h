#pragma once

#include "TreeBuilder.h"
#include "ISpecialFieldsHandler.h"


namespace ckcmd {
	namespace HKX {
		class SkeletonBuilder : 
			public ITreeBuilderClassHandler, 
			public ISpecialFieldsHandler
		{
			hkRefPtr<hkaSkeleton> _skeleton;
			hkRefPtr<hkaSkeleton> _ragdoll;

			struct buildContext {
				const fs::path& _file;
				int object_index;
				ProjectNode* parent;
				ResourceManager& resourceManager;
			};

			virtual std::vector<member_id_t> getSkeletonBoneFields();
			virtual std::vector<member_id_t> getRagdollBoneFields();

			void buildSkeleton(const buildContext& context);

			ProjectNode* buildBranch(hkVariant& root, ProjectNode* root_node, const fs::path& path, ResourceManager& _resourceManager);

		public:

			SkeletonBuilder();

			virtual std::vector<const hkClass*> getHandledClasses() override;

			virtual ProjectNode* visit(
				const fs::path& _file,
				int object_index,
				ProjectNode* parent,
				ResourceManager& resourceManager) override;

			virtual std::vector<member_id_t> getHandledFields() override;

			virtual QVariant handle(void* value, const hkClass*, const hkClassMember*, const hkVariant*, const hkVariant* parent_container) override;

			hkRefPtr<hkaSkeleton> skeleton() { return _skeleton; }
			hkRefPtr<hkaSkeleton> ragdoll() { return _ragdoll; }

		};
	}
}

