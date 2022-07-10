#pragma once

#include <src/hkx/HkxTypeInfo.h>
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

            static const int CHARACTER_RIG_ROW = 3;

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
                return std::tie(_project, _childType);
            }

            auto as_file_only_tuple() const
            {
                return std::tie(_file, _childType);
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
                            _childType == NodeType::SkeletonHkxNode ||
                            _childType == NodeType::RagdollHkxNode ||
                            _childType == NodeType::SkeletonBones ||
                            _childType == NodeType::RagdollBones
                        ) || (
                            rhs._childType == NodeType::SkeletonHkxNode ||
                            rhs._childType == NodeType::RagdollHkxNode ||
                            rhs._childType == NodeType::SkeletonBones ||
                            rhs._childType == NodeType::RagdollBones
                        )
                    )
                {
                    return as_project_index_tuple() < rhs.as_project_index_tuple();
                }
                if (
                    (
                        _childType == NodeType::behaviorEventNames ||
                        _childType == NodeType::behaviorVariableNames ||
                        _childType == NodeType::behaviorCharacterPropertyNames
                        ) || (
                        rhs._childType == NodeType::behaviorEventNames ||
                        rhs._childType == NodeType::behaviorVariableNames ||
                        rhs._childType == NodeType::behaviorCharacterPropertyNames
                        )
                    )
                {
                    return as_file_only_tuple() < rhs.as_file_only_tuple();
                }
                if (
                    (
                        _childType == NodeType::behaviorEventName ||
                        _childType == NodeType::behaviorVariable ||
                        _childType == NodeType::behaviorCharacterProperty
                    ) || (
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

            bool operator == (const ModelEdge& rhs) const {
                if (
                    (
                        _childType == NodeType::SkeletonHkxNode ||
                        _childType == NodeType::RagdollHkxNode ||
                        _childType == NodeType::SkeletonBones ||
                        _childType == NodeType::RagdollBones
                        ) || (
                            rhs._childType == NodeType::SkeletonHkxNode ||
                            rhs._childType == NodeType::RagdollHkxNode ||
                            rhs._childType == NodeType::SkeletonBones ||
                            rhs._childType == NodeType::RagdollBones
                            )
                    )
                {
                    return as_project_index_tuple() == rhs.as_project_index_tuple();
                }
                if (
                    (
                        _childType == NodeType::behaviorEventNames ||
                        _childType == NodeType::behaviorVariableNames ||
                        _childType == NodeType::behaviorCharacterPropertyNames
                        ) || (
                            rhs._childType == NodeType::behaviorEventNames ||
                            rhs._childType == NodeType::behaviorVariableNames ||
                            rhs._childType == NodeType::behaviorCharacterPropertyNames
                            )
                    )
                {
                    return as_file_only_tuple() == rhs.as_file_only_tuple();
                }
                if (
                    (
                        _childType == NodeType::behaviorEventName ||
                        _childType == NodeType::behaviorVariable ||
                        _childType == NodeType::behaviorCharacterProperty
                        ) || (
                            rhs._childType == NodeType::behaviorEventName ||
                            rhs._childType == NodeType::behaviorVariable ||
                            rhs._childType == NodeType::behaviorCharacterProperty
                            )
                    )
                {
                    return as_file_index_tuple() == rhs.as_file_index_tuple();
                }
                return as_tuple() == rhs.as_tuple();
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
            TypeInfo rowClass(int row, ResourceManager& manager) const;
            bool isArray(int row, ResourceManager& manager) const;

            bool canAddRow(const ModelEdge& other, ResourceManager& manager);

            size_t hash_combine(size_t lhs, size_t rhs) const {
                lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
                return lhs;
            }


            size_t hash() const {
                if (
                        _childType == NodeType::SkeletonHkxNode ||
                        _childType == NodeType::RagdollHkxNode ||
                        _childType == NodeType::SkeletonBones ||
                        _childType == NodeType::RagdollBones
                    )
                {
                    return std::hash<int>()(_project) ^ 
                        (( std::hash<int>()((int)_childType) << 1) >> 1);
                }
                if (
                        _childType == NodeType::behaviorEventNames ||
                        _childType == NodeType::behaviorVariableNames ||
                        _childType == NodeType::behaviorCharacterPropertyNames
                    )
                {
                    return std::hash<int>()(_project) ^
                        ((std::hash<int>()((int)_file) << 1) >> 1) ^
                        ((std::hash<int>()((int)_childType) << 1));
                }
                if (
                        _childType == NodeType::behaviorEventName ||
                        _childType == NodeType::behaviorVariable ||
                        _childType == NodeType::behaviorCharacterProperty
                    )
                {
                    return std::hash<int>()(_project) ^
                        ((std::hash<int>()((int)_file) << 1) >> 1) ^
                        ((std::hash<int>()((int)_row) << 1) >> 1) ^
                        ((std::hash<int>()((int)_childType) << 1));
                }
                size_t seed = (size_t)_parentType;
                seed = hash_combine(seed, (size_t)_parent.row());
                seed = hash_combine(seed, (size_t)_parent.column());
                seed = hash_combine(seed, (size_t)_parent.internalPointer());
                seed = hash_combine(seed, (size_t)_project);
                seed = hash_combine(seed, (size_t)_file);
                seed = hash_combine(seed, (size_t)_row);
                seed = hash_combine(seed, (size_t)_column);
                seed = hash_combine(seed, (size_t)_childItem);
                seed = hash_combine(seed, (size_t)_child.row());
                seed = hash_combine(seed, (size_t)_child.column());
                seed = hash_combine(seed, (size_t)_child.internalPointer());
                seed = hash_combine(seed, (size_t)_subindex);
                seed = hash_combine(seed, (size_t)_childType);
                return seed;

                //return std::hash<int>()((int)_parentType) ^
                //    ((std::hash<qintptr>()((qintptr)_parentItem) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_parent.row()) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_parent.column()) << 1) >> 1) ^
                //    ((std::hash<qintptr>()((qintptr)_parent.internalPointer()) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_project) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_file) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_row) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_column) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_subindex) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_childType) << 1) >> 1) ^
                //    ((std::hash<qintptr>()((qintptr)_childItem) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_child.row()) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_child.column()) << 1) >> 1) ^
                //    ((std::hash<qintptr>()((qintptr)_child.internalPointer()) << 1) >> 1) ^
                //    ((std::hash<int>()((int)_childType) << 1));
            }
        };

        struct ModelEdgeComparator {
            bool operator()(const ModelEdge* a, const ModelEdge* b) const {
                return *a < *b;
            }
        };

        class ModelEdgeHasher
        {
        public:
            size_t operator() (const ModelEdge* a) const
            {
                return a->hash();
            }
        };

        class ModelEdgeEqualFn
        {
        public:
            bool operator() (const ModelEdge* a, const ModelEdge* b) const
            {
                return *a == *b;
            }
        };


    }
}