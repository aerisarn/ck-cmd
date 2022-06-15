#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ModelEdge.h>

#include <QVariant>

struct hkVariant;

namespace ckcmd {
	namespace HKX {

		class ProjectTreeHkHandler
		{

		public:

			static int getChildCount(int project, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static ModelEdge getChild(hkVariant*, int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);

			static QVariant data(int row, int column, hkVariant* variant, NodeType childType);
			static bool setData(int project, int row, int column, hkVariant* variant, NodeType childType, const QVariant& value, ResourceManager& manager);
		};
	}
}