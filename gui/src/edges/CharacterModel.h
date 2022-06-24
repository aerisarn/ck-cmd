#pragma once

#include <hkbCharacterData_7.h>

#include <src/models/ModelEdge.h>
#include <set>

namespace ckcmd {
	namespace HKX {

		struct  CharacterModel
		{
			//static const int DATA_SUPPORTS = 3;
			//static const int BEHAVIOR_INDEX = DATA_SUPPORTS;
			//static const int RIG_INDEX = BEHAVIOR_INDEX + 1;
			//static const int RAGDOLL_INDEX = RIG_INDEX + 1;
			//static const int SUPPORT_END = RAGDOLL_INDEX + 1;

			static const char* DataListsName(int row)
			{
				switch ((NodeType)row) {
				case NodeType::deformableSkinNames:
					return "Deformable Skins";
				case NodeType::animationNames:
					return "Animations";
				case NodeType::characterPropertyNames:
					return "Character Properties";
				default:
					break;
				}
				return "Invalid Character Entry";
			};

			//rows with files to be opened
			static const std::set<QString> children_rows;

			static int childRows(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static int childColumns(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static int childCount(const ModelEdge& edge, ResourceManager& manager);
			static bool hasChild(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			
			static QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager);
			static bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			static bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			static bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager);


#undef max


		};
	}
}
