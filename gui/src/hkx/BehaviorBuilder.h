#pragma once

#include <src/hkx/TreeBuilder.h>
#include <src/hkx/CommandManager.h>
#include "ISpecialFieldsHandler.h"
#include "SkeletonBuilder.h"

#include <core/AnimationCache.h>

#include <hkbBehaviorGraphData_2.h>
#include <hkbBehaviorGraphStringData_1.h>
#include <hkbClipGenerator_2.h>


namespace ckcmd {
	namespace HKX {

		class BehaviorBuilder : 
			public ITreeBuilderClassHandler, 
			public ISpecialFieldsHandler
		{
		protected:
			struct buildContext {
				const hkbBehaviorGraphData* data;
				const hkbBehaviorGraphStringData* string_data;
				const fs::path& _file;
				int object_index;
				ProjectNode* parent;
			};

			virtual std::vector<member_id_t> getEventFields();
			virtual std::vector<member_id_t> getStateIdFields();
			virtual std::vector<member_id_t> getVariableFields();

			void buildEvents(const buildContext& context);
			void buildVariables(const buildContext& context);
			void buildProperties(const buildContext& context);
			void buildAnimationStyles(const buildContext& context);

			SkeletonBuilder* _skeleton_builder;
			hkbBehaviorGraphData* _data;
			hkbBehaviorGraphStringData* _strings;
			std::set<std::string> _referenced_behaviors;
			bool _root_behavior;

			size_t _file_index;
			size_t _project_file_index;
			size_t _character_file_index;
			ResourceManager& _manager;
			CommandManager& _command_manager;
			ProjectNode* _animationsNode;
			ProjectNode* _eventsNode;
			ProjectNode* _variablesNode;

			ProjectNode* buildBranch(hkVariant& variant, ProjectNode* root_node, const fs::path& path);
			void addCacheToClipNode(ProjectNode* animation_node, const hkbClipGenerator* clip);

		public:

			BehaviorBuilder(
				CommandManager& commandManager, 
				ResourceManager& manager, 
				size_t _project_file_index,
				size_t _character_file_index,
				size_t file_index, 
				ProjectNode* animationsNode,
				bool root_behavior
			);

			const std::set<std::string>& referenced_behaviors() { return _referenced_behaviors; }

			virtual std::vector<const hkClass*> getHandledClasses() override;

			virtual ProjectNode* visit(
				const fs::path& _file,
				int object_index,
				ProjectNode* parent) override;

			virtual std::vector<member_id_t> getHandledFields() override;

			virtual QVariant handle(void* value, const hkClass*, const hkClassMember*, const hkVariant*) override;

			SkeletonBuilder* skeletonBuilder() { return _skeleton_builder; }
			void setSkeleton(SkeletonBuilder* skeleton_builder) { _skeleton_builder = skeleton_builder; }


			QStringList getEvents() const;
			QString getEvent(size_t index) const;
			QStringList getVariables() const;
			QString getVariable(size_t index) const;
			QStringList getFSMStates(size_t fsm_index) const;
			QString getFSMState(size_t fsm_index, size_t index) const;
			size_t getFSMStateId(size_t fsm_index, size_t combo_index) const;

			//Actions
			virtual size_t addEvent(const QString&);
			virtual bool renameEvent(size_t index, const QString&);
			virtual size_t removeEvent(const QString&);

			virtual size_t addVariable(const QString&);
			virtual size_t removeVariable(const QString&);
		};
	}
}

