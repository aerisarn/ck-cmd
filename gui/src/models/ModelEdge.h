#pragma once

#include <src/hkx/ResourceManager.h>

#include <QAbstractItemModel>

namespace ckcmd {
    namespace HKX {

        class ProjectTreeModel;
        class ProjectTreeHkHandler;

        enum class NodeType
        {
            Invalid = 0,
            CharactersNode,
            CharacterNode,
            CharacterHkxNode,
            MiscsNode,
            MiscNode,
            MiscHkxNode,
            BehaviorNode,
            BehaviorHkxNode,
            HavokNative,
            deformableSkinNames,
            deformableSkinName,
            animationNames,
            animationName,
            characterPropertyNames,
            characterPropertyName,
            behaviorEventNames,
            behaviorEventName,
            behaviorVariableNames,
            behaviorVariableName,
            behaviorCharacterPropertyNames,
            behaviorCharacterPropertyName,
            FSMWildcardTransitions,
            FSMWildcardTransition,
            FSMStateTransitions,
            FSMStateTransition,
            SkeletonBones,
            SkeletonBone,
            SkeletonFloats,
            SkeletonFloat,
            RagdollBones,
            RagdollBone
        };

        static constexpr bool isVariant(NodeType t) {
            return t == NodeType::CharacterHkxNode ||
                t == NodeType::MiscHkxNode ||
                t == NodeType::HavokNative;
        }

        static constexpr bool isAssetsNode(NodeType t) {
            return t == NodeType::animationNames ||
                t == NodeType::deformableSkinNames;
        }

        class ModelEdge
        {
            friend class ProjectTreeModel;
            friend class ProjectTreeHkHandler;

            NodeType _parentType = NodeType::Invalid;
            void* _parentItem = nullptr;
            QModelIndex _parent;
            int _project = -1;
            int _file = -1;
            int _row = -1;
            int _column = -1;
            NodeType _childType = NodeType::Invalid;
            void* _childItem = nullptr;
            QModelIndex _child;

            auto as_project_index_tuple() const
            {
                return std::tie(_project, _row, _column, _childType);
            }

            auto as_file_index_tuple() const
            {
                return std::tie(_file, _row, _column, _childType);
            }

            auto as_tuple() const
            { 
                return std::tie(_parentType, _parentItem, _file, _row, _column, _childItem, _childType);
            }

        public:
            bool operator < (const ModelEdge& rhs) const {
                if (
                        (
                        _childType == NodeType::SkeletonBone ||
                        _childType == NodeType::RagdollBone
                        ) && (
                            rhs._childType == NodeType::SkeletonBone ||
                            rhs._childType == NodeType::RagdollBone
                        )
                    )
                {
                    return as_project_index_tuple() < rhs.as_project_index_tuple();
                }
                if (
                    (
                        _childType == NodeType::behaviorEventName ||
                        _childType == NodeType::behaviorVariableName ||
                        _childType == NodeType::behaviorCharacterPropertyName
                    ) && (
                        rhs._childType == NodeType::behaviorEventName ||
                         rhs._childType == NodeType::behaviorVariableName ||
                         rhs._childType == NodeType::behaviorCharacterPropertyName
                    )
                ) 
                {
                    return as_file_index_tuple() < rhs.as_file_index_tuple();
                }             
                return as_tuple() < rhs.as_tuple();
            }

            ModelEdge() {}

            ModelEdge(hkVariant*, int project, int file, int row, int column, hkVariant*);
            ModelEdge(hkVariant*, int project, int file, int row, int column, hkVariant*, NodeType childType);

            QVariant data(int row, int column, ResourceManager& manager) const;
            bool setData(int row, int column, const QVariant& value, ResourceManager& manager);
            ModelEdge childEdge(int index, ResourceManager& manager) const;
            int childCount(ResourceManager& manager) const;

            NodeType type();
        };

        struct ModelEdgeComparator {
            bool operator()(const ModelEdge* a, const ModelEdge* b) const {
                return *a < *b;
            }
        };
    }
}