#pragma once

#include <src/hkx/ResourceManager.h>

#include <QAbstractItemModel>

#define MODELEDGE_INVALID -1

namespace ckcmd {
    namespace HKX {

        class ProjectModel;

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
            characterProperty,
            behaviorEventNames,
            behaviorEventName,
            behaviorVariableNames,
            behaviorVariable,
            behaviorCharacterPropertyNames,
            behaviorCharacterProperty,
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

        enum class VariableType
        {
            Word,
            Quad,
            Variant
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


            NodeType _parentType = NodeType::Invalid;
            void* _parentItem = nullptr;
            QModelIndex _parent;
            int _project = -1;
            int _file = -1;
            int _row = -1;
            int _column = -1;
            int _subindex = -1;
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
                return std::tie(_parentType, _parentItem, _file, _row, _column, _subindex, _childItem, _childType);
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
                        _childType == NodeType::behaviorVariable ||
                        _childType == NodeType::behaviorCharacterProperty
                    ) && (
                        rhs._childType == NodeType::behaviorEventName ||
                        rhs._childType == NodeType::behaviorVariable ||
                        rhs._childType == NodeType::behaviorCharacterProperty
                    )
                ) 
                {
                    return as_file_index_tuple() < rhs.as_file_index_tuple();
                }             
                return as_tuple() < rhs.as_tuple();
            }

            ModelEdge() {}

            ModelEdge(const ModelEdge& other)
            {
                _parentType = other._parentType;
                _parentItem = other._parentItem;
                _parent = other._parent;
                _project = other._project;
                _file = other._file;
                _row = other._row;
                _column = other._column;
                _subindex = other._subindex;
                _childType = other._childType;
                _childItem = other._childItem;
                _child = other._child;
            }

            ModelEdge(hkVariant*, int project, int file, int row, int column, hkVariant*);
            ModelEdge(hkVariant*, int project, int file, int row, int column, hkVariant*, NodeType childType);
            ModelEdge(hkVariant*, int project, int file, int row, int column, int subindex, hkVariant*, NodeType childType);
            ModelEdge(const ModelEdge& parent, int project, int file, int row, int column, int subindex, hkVariant*, NodeType childType);

            NodeType parentType() const {return _parentType;}
            template<typename T>
            T* parentItem() const { return static_cast<T*>(_parentItem); }
            int project() const { return _project; }
            int file() const { return _file; }
            int row() const { return _row; }
            int column() const { return _column; }
            int subindex() const { return _subindex; }
            NodeType childType() const { return _childType; }
            template<typename T>
            T* childItem() const { return static_cast<T*>(_childItem); }

            void addSupport(int support_offset) { _row += support_offset; }
            
            int rows(ResourceManager& manager) const;
            int columns(int row, ResourceManager& manager) const;
            
            int childCount(ResourceManager& manager) const;
            std::pair<int, int> child(int index, ResourceManager& manager) const;
            int childIndex(int row, int column, ResourceManager& manager) const;
            bool hasChild(int row, int column, ResourceManager& manager) const;

            ModelEdge childEdge(int row, int column, ResourceManager& manager) const;

            QVariant data(int row, int column, ResourceManager& manager) const;
            bool setData(int row, int column, const QVariant& value, ResourceManager& manager);
            std::pair<int, int> dataStart();

            bool insertRows(int row_start, int count, ResourceManager& manager);
            bool removeRows(int row_start, int count, ResourceManager& manager);
            bool insertColumns(int row, int column_start, int count, ResourceManager& manager);
            bool removeColumns(int row, int column_start, int count, ResourceManager& manager);

            NodeType type() const;
            const hkClass* rowClass(int row, ResourceManager& manager) const;

            bool canAddRow(const ModelEdge& other, ResourceManager& manager);
        };

        struct ModelEdgeComparator {
            bool operator()(const ModelEdge* a, const ModelEdge* b) const {
                return *a < *b;
            }
        };
    }
}