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
			ResourceManager& _manager;
			size_t _file_index;

			struct buildContext {
				const fs::path& _file;
				int object_index;
				ProjectNode* parent;
			};

			virtual std::vector<member_id_t> getSkeletonBoneFields();
			virtual std::vector<member_id_t> getRagdollBoneFields();

			void buildSkeleton(const buildContext& context);

			ProjectNode* buildBranch(hkVariant& root, ProjectNode* root_node, const fs::path& path);

		public:

			SkeletonBuilder(ResourceManager& manager, size_t file_index);

			virtual std::vector<const hkClass*> getHandledClasses() override;

			virtual ProjectNode* visit(
				const fs::path& _file,
				int object_index,
				ProjectNode* parent) override;

			virtual std::vector<member_id_t> getHandledFields() override;

			virtual QVariant handle(void* value, const hkClass*, const hkClassMember*, const hkVariant*) override;
			virtual QVariant handle(size_t file_index, void* value, const hkClass*, const hkClassMember*, const hkVariant*);

			hkRefPtr<hkaSkeleton> skeleton() { return _skeleton; }
			hkRefPtr<hkaSkeleton> ragdoll() { return _ragdoll; }

			QStringList getSkeletonBones() const;
			QString getSkeletonBone(size_t index) const;
			QStringList getRagdollBones() const;
			QString getRagdollBone(size_t index) const;

		};
	}
}

