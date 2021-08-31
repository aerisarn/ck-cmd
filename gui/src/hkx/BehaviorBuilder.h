#pragma once

#include "TreeBuilder.h"
#include "ISpecialFieldsHandler.h"

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

			hkRefPtr<hkbBehaviorGraphStringData> _strings;

		public:

			virtual std::vector<const hkClass*> getHandledClasses() override;

			virtual ProjectNode* visit(
				const fs::path& _file,
				int object_index,
				ProjectNode* parent,
				ResourceManager& resourceManager) override;

			virtual std::vector<member_id_t> getHandledFields() override;

			virtual QVariant handle(void* value, const hkClass*, const hkClassMember*) override;

		};
	}
}

