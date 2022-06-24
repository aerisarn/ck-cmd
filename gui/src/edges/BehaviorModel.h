#pragma once

#include <src/models/ModelEdge.h>

#include <hkbBehaviorGraph_1.h>
#include <hkbBehaviorGraphData_2.h>
#include <hkbBehaviorGraphStringData_1.h>

namespace ckcmd {
	namespace HKX {


		struct BehaviorModel
		{

			static const size_t DATA_SUPPORTS = 3;

			static const char* DataListsName(int row)
			{
				switch ((NodeType)row) {
				case NodeType::behaviorEventNames:
					return "Events";
				case NodeType::behaviorVariableNames:
					return "Variables";
				case NodeType::behaviorCharacterPropertyNames:
					return "Properties";
				default:
					break;
				}
				return "Invalid Behavior Entry";
			};

			static int childRows(int row, int column, ModelEdge& edge, ResourceManager& manager);
			static int childRowColumns(int row, ModelEdge& edge, ResourceManager& manager);
			static int childColumns(int row, int column, ModelEdge& edge, ResourceManager& manager);
			static int childCount(ModelEdge& edge, ResourceManager& manager);
			static bool hasChild(int row, int column, ModelEdge& edge, ResourceManager& manager);
			static ModelEdge child(int row, int column, ModelEdge& edge, ResourceManager& manager);

			static QVariant data(int row, int column, ModelEdge& edge, ResourceManager& manager);
			static bool setData(int row, int column, ModelEdge& edge, const QVariant& data, ResourceManager& manager);
			static bool addRows(int row_start, int count, ModelEdge& edge, ResourceManager& manager);
			static bool removeRows(int row_start, int count, ModelEdge& edge, ResourceManager& manager);
			static bool changeColumns(int row, int column_start, int delta, ModelEdge& edge, ResourceManager& manager);

			
		};


	}
}
