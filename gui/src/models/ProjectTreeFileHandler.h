#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ModelEdge.h>

#include <QVariant>

struct hkVariant;

namespace ckcmd {
	namespace HKX {

		class ProjectTreeFileHandler
		{

		public:

			static int ProjectTreeFileHandler::getChildCount(int file_index, NodeType childType, ResourceManager& _manager);
			static QVariant ProjectTreeFileHandler::data(int row, int column, int file_index, NodeType childType, ResourceManager& _manager);
			static ModelEdge ProjectTreeFileHandler::getChild(int index, int project, int file, NodeType childType, ResourceManager& _manager);
		};


	}
}