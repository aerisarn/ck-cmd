#pragma once

#include <src/hkx/ResourceManager.h>

#include <QAbstractItemModel>

namespace ckcmd {
    namespace HKX {

        class ProjectModel;
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
            SkeletonHkxNode,
            SkeletonBones,
            SkeletonBone,
            SkeletonFloats,
            SkeletonFloat,
            RagdollHkxNode,
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
            friend class ProjectModel;
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
                        _childType == NodeType::SkeletonBones ||
                        _childType == NodeType::RagdollBones
                        ) && (
                            rhs._childType == NodeType::SkeletonBones ||
                            rhs._childType == NodeType::RagdollBones
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

            int childRows(int row, int column, ResourceManager& manager) const;
            int rowColumns(int row, int column, ResourceManager& _resourceManager) const;
            int childColumns(int row, int column, ResourceManager& manager) const;

            QVariant data(int row, int column, ResourceManager& manager) const;
            bool setData(int row, int column, const QVariant& value, ResourceManager& manager);
            bool hasChild(int row, int column, ResourceManager& manager) const;
            ModelEdge childEdge(int row, int column, ResourceManager& manager) const;
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