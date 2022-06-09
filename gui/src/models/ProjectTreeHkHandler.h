#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ModelEdge.h>

#include <QVariant>

struct hkVariant;

namespace ckcmd {
	namespace HKX {

		class ProjectTreeHkHandler
		{

			static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);

		public:

			static int getChildCount(hkVariant* variant, NodeType childType);

			static ModelEdge getChild(hkVariant*, int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);

			static QVariant data(int row, int column, hkVariant* variant, NodeType childType);
		};
	}
}