#pragma once

#include <src/models/ModelEdge.h>

#include <Animation/Animation/Rig/hkaSkeleton.h>

namespace ckcmd {
	namespace HKX {


		struct SkeletonModel
		{
			static const size_t DATA_SUPPORTS = 2;

			static const char* DataListsName(int row)
			{
				switch ((NodeType)row) {
				case NodeType::SkeletonBones:
					return "Bones";
				case NodeType::SkeletonFloats:
					return "Floats";
				default:
					break;
				}
				return "Invalid Character Entry";
			};

			static int getChildCount(hkVariant* variant, NodeType childType)
			{
				auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
				if (data == NULL)
					return 0;
				if (childType == NodeType::SkeletonHkxNode)
				{
					return DATA_SUPPORTS;
				}
				else if (childType == NodeType::SkeletonBones)
				{
					return data->m_bones.getSize();
				}
				else if (childType == NodeType::SkeletonFloats)
				{
					return data->m_floatSlots.getSize();
				}
				else {
					return 0;
				}
			}

			static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
			{
				auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
				if (childType == NodeType::SkeletonBones ||
					childType == NodeType::SkeletonFloats)
				{
					return DataListsName((int)childType);
				}
				else if (childType == NodeType::SkeletonBone)
				{
					return data->m_bones[row].m_name.cString();
				}
				else if (childType == NodeType::SkeletonFloat)
				{
					return data->m_floatSlots[row].cString();
				}
				else if (childType == NodeType::SkeletonHkxNode) {
					return "Skeleton";
				}
				return 0;

			}

			static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
			{
				if (childType == NodeType::SkeletonHkxNode)
				{
					switch (index) {
					case 0:
						return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonBones);
					case 1:
						return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonFloats);
					default:
						break;
					}
					return ModelEdge();
				}
				else if (childType == NodeType::SkeletonBones)
				{
					return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonBone);
				}
				else if (childType == NodeType::SkeletonFloats)
				{
					return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonFloat);
				}
				else {
					return ModelEdge();
				}
			}
		};


	}
}
