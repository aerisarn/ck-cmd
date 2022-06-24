#pragma once

#include <src/models/ModelEdge.h>

#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>

namespace ckcmd {
	namespace HKX {

		struct  RagdollModel
		{
			static const size_t DATA_SUPPORTS = 1;

			static const char* DataListsName(int row)
			{
				switch ((NodeType)row) {
				case NodeType::RagdollBones:
					return "Bones";
				default:
					break;
				}
				return "Invalid Character Entry";
			};

			static int getChildCount(hkVariant* variant, NodeType childType)
			{
				auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
				if (data == NULL)
					return 0;
				if (childType == NodeType::RagdollHkxNode)
				{
					return DATA_SUPPORTS;
				}
				else if (childType == NodeType::RagdollBones)
				{
					if (NULL == data->m_skeleton)
						return 0;
					return data->m_skeleton->m_bones.getSize();
				}
				else {
					return 0;
				}
			}

			static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
			{
				auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
				if (childType == NodeType::RagdollBones)
				{
					return DataListsName((int)childType);
				}
				else if (childType == NodeType::RagdollBone)
				{
					return data->m_skeleton->m_bones[row].m_name.cString();
				}
				else if (childType == NodeType::RagdollHkxNode)
				{
					return "Ragdoll";
				}
				else {
					return 0;
				}
			}

			static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
			{
				if (childType == NodeType::RagdollHkxNode)
				{
					switch (index) {
					case 0:
						return ModelEdge(variant, project, file, index, 0, variant, NodeType::RagdollBones);
					default:
						break;
					}
					return ModelEdge();
				}
				else if (childType == NodeType::RagdollBones)
				{
					return ModelEdge(variant, project, file, index, 0, variant, NodeType::RagdollBone);
				}
				else {
					return ModelEdge();
				}
			}
		};


	}
}
