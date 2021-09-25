#pragma once

#include <QVariant>
#include <QVector>

namespace ckcmd {
    namespace HKX {

        class ProjectNode
        {

        public:

            enum class NodeType {
                fixed,
                support,
                project_node,
                character_node,
                behavior_node,
                skeleton_node,
                animation_node,
                misc_node,
                creature_node,
                hkx_project_node,
                hkx_character_node,
                hkx_node,
                event_node,
                weapon_set_node,
                variable_node,
                property_node,
                clip_event_node,
                events_node,
                variables_node,
                animation_styles_node,
                animation_style_node
            } m_type;

            explicit ProjectNode(
                NodeType type, 
                const QVector<QVariant>& data, 
                ProjectNode* parentItem = nullptr
            );
            ~ProjectNode();



            bool isProjectRoot() const;
            bool isSkeleton() const;
            bool isVariant() const;
            bool isCharacter() const;

            bool canSaveOrExport() const;

            ProjectNode* appendChild(ProjectNode* child);
            void appendData(const QVariant& value);

            virtual ProjectNode* child(int row);
            ProjectNode* setChild(int row, ProjectNode* new_child);
            void addParent(ProjectNode* new_parent);
            void setParent(ProjectNode* new_parent);
            ProjectNode* removeChild(int row);
            void removeParent(ProjectNode* parent);
            void insertChild(int row, ProjectNode* new_child);
            virtual int childCount() const;
            virtual int columnCount() const;
            virtual QVariant data(int column) const;
            virtual int row() const;
            virtual ProjectNode* parentItem();
            ProjectNode* parentItem(int row);
            virtual int parentCount() const;
            virtual NodeType type() const;

            template<typename Visitor>
            void accept(Visitor& v) {
                switch (m_type) {
                case NodeType::fixed:
                    v.visit<NodeType::fixed>(*this);
                    break;
                case NodeType::support:
                    v.visit<NodeType::support>(*this);
                    break;
                case NodeType::project_node:
                    v.visit<NodeType::project_node>(*this);
                    break;
                case NodeType::character_node:
                    v.visit<NodeType::character_node>(*this);
                    break;
                case NodeType::behavior_node:
                    v.visit<NodeType::behavior_node>(*this);
                    break;
                case NodeType::skeleton_node:
                    v.visit<NodeType::skeleton_node>(*this);
                    break;
                case NodeType::animation_node:
                    v.visit<NodeType::animation_node>(*this);
                    break;
                case NodeType::misc_node:
                    v.visit<NodeType::misc_node>(*this);
                    break;
                case NodeType::hkx_project_node:
                    v.visit<NodeType::hkx_project_node>(*this);
                    break;
                case NodeType::hkx_character_node:
                    v.visit<NodeType::hkx_character_node>(*this);
                    break;
                case NodeType::hkx_node:
                    v.visit<NodeType::hkx_node>(*this);
                    break;
                case NodeType::event_node:
                    v.visit<NodeType::event_node>(*this);
                    break;
                case NodeType::weapon_set_node:
                    v.visit<NodeType::weapon_set_node>(*this);
                    break;
                case NodeType::variable_node:
                    v.visit<NodeType::variable_node>(*this);
                    break;
                case NodeType::property_node:
                    v.visit<NodeType::property_node>(*this);
                    break;
                case NodeType::clip_event_node:
                    v.visit<NodeType::clip_event_node>(*this);
                    break;
                case NodeType::events_node:
                    v.visit<NodeType::events_node>(*this);
                    break;
                case NodeType::variables_node:
                    v.visit<NodeType::variables_node>(*this);
                    break;
                case NodeType::animation_styles_node:
                    v.visit<NodeType::animation_styles_node>(*this);
                    break;
                case NodeType::animation_style_node:
                    v.visit<NodeType::animation_style_node>(*this);
                    break;
                default:
                    break;
                }
            }

        protected:
            QVector<ProjectNode*> m_childItems;
            QVector<QVariant> m_itemData;
            QVector<ProjectNode*> m_parentItems;
        };
    }
}