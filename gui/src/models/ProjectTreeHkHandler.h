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

			static int childRows(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static int childColumns(int project, int file, int row, int column, hkVariant* variant, ResourceManager& manager);
			static int getChildCount(int project, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static bool hasChild(hkVariant*, int row, int column, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);
			static ModelEdge getChild(hkVariant*, int row, int column, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);

			static QVariant data(int row, int column, hkVariant* variant, NodeType childType);
			static bool setData(int project, int row, int column, hkVariant* variant, NodeType childType, const QVariant& value, ResourceManager& manager);
		};
	}
}