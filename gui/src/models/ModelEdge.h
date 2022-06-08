#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ProjectNode.h>

#include <QAbstractItemModel>

//Proof of concept, but unuseful. look for table/tree models instead

class hkbBehaviorGraph;
class hkbCharacterData;

namespace ckcmd {
    namespace HKX {

        class ProjectTreeModel;
        class ProjectTreeHkHandler;

        enum class NodeType
        {
            Invalid = 0,
            ProjectNode,
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
            SkeletonFloat
        };

        class ModelEdge
        {
            friend class ProjectTreeModel;
            friend class ProjectTreeHkHandler;

            NodeType _parentType = NodeType::Invalid;
            void* _parentItem = nullptr;
            QModelIndex _parent;
            int _file = -1;
            int _row = -1;
            int _column = -1;
            NodeType _childType = NodeType::Invalid;
            void* _childItem = nullptr;
            QModelIndex _child;

            auto as_tuple() const { return std::tie(_parentItem, _parentType, _file, _row, _column, _childItem, _childType); }

        public:
            bool operator < (const ModelEdge& rhs) const {
                return
                    as_tuple() < rhs.as_tuple();
            }

            ModelEdge() {}

            ModelEdge(ProjectNode*, int file, int row, int column, ProjectNode*);
            ModelEdge(ProjectNode*, int file, int row, int column, hkVariant*);
            ModelEdge(hkVariant*, int file, int row, int column, hkVariant*);
            ModelEdge(hkVariant*, int file, int row, int column, hkVariant*, NodeType childType);

            QVariant data(int row, int column) const;

            ModelEdge childEdge(int index, ResourceManager& manager) const;
            int childCount() const;
        };

        struct ModelEdgeComparator {
            bool operator()(const ModelEdge* a, const ModelEdge* b) const {
                return *a < *b;
            }
        };
    }
}