#pragma once

#include <src/models/ModelEdge.h>

#include <hkbBehaviorReferenceGenerator_0.h>

namespace ckcmd {
	namespace HKX {

		struct  BehaviorReferenceModel {

			static int getChildCount(hkVariant* variant, NodeType childType)
			{
				return 1;
			}

			static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
			{
				hkbBehaviorReferenceGenerator* bfg = reinterpret_cast<hkbBehaviorReferenceGenerator*>(variant->m_object);
				return bfg->m_behaviorName.cString();
			}

			static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
			{
				int behavior_file = manager.behaviorFileIndex(project, variant);
				hkVariant* root = manager.behaviorFileRoot(behavior_file);
				return ModelEdge(variant, project, behavior_file, index, 0, root, NodeType::BehaviorHkxNode);
			}

		};


	}
}
