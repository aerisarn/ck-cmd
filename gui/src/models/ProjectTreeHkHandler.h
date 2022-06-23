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
			static int childRowColumns(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static int childColumns(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static int getChildCount(int project, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static bool hasChild(hkVariant*, int row, int column, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);
			static ModelEdge getChild(hkVariant*, int row, int column, int subindex, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);

			static QVariant data(int file, int row, int column, int subindex, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static bool setData(int row, int column, int project, int file, hkVariant* variant, NodeType childType, const QVariant& value, ResourceManager& manager);
			static bool addRows(int row_start, int count, int project, int file, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static bool removeRows(int row_start, int count, int project, int file, hkVariant* variant, NodeType childType, ResourceManager& manager);
			static bool changeColumns(int row, int column_start, int delta, int _project, int _file, hkVariant* variant, NodeType childType, ResourceManager& manager);
		};
	}
}