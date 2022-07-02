#pragma once

#include <src/models/ModelEdge.h>

#include <Animation/Animation/Rig/hkaSkeleton.h>

namespace ckcmd {
	namespace HKX {


		struct SkeletonModel
		{
			static const size_t DATA_SUPPORTS = 2;

			static const char* DataListsName(NodeType type)
			{
				switch (type) {
				case NodeType::SkeletonBones:
					return "Bones";
				case NodeType::SkeletonFloats:
					return "Floats";
				default:
					break;
				}
				return "Invalid Character Entry";
			};

			static int rows(const ModelEdge& edge, ResourceManager& manager);
			static int columns(int row, const ModelEdge& edge, ResourceManager& manager);
			static int childCount(const ModelEdge& edge, ResourceManager& manager);
			static std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager);
			static int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager);
			static ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager);

			static QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager);
		};


	}
}
