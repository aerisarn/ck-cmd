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

			template <typename parentNodeType>
			static ModelEdge getChild(parentNodeType*, int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
			{}

			template <>
			static ModelEdge getChild(ProjectNode*, int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);


			template <>
			static ModelEdge getChild(hkVariant*, int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType);

			static QVariant data(int row, int column, hkVariant* variant, NodeType childType);
		};

		template <>
		ModelEdge ProjectTreeHkHandler::getChild(ProjectNode* node, int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
		{
			auto edge = get_child(index, project, file, variant, manager, childType);
			edge._parentItem = node;
			edge._parentType = NodeType::ProjectNode;
			return edge;
		}

		template <>
		ModelEdge ProjectTreeHkHandler::getChild(hkVariant*, int project, int index, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
		{
			auto edge = get_child(index, project, file, variant, manager, childType);
			edge._parentItem = variant;
			edge._parentType = NodeType::HavokNative;
			return edge;
		}

	}
}