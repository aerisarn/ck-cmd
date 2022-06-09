#pragma once

#include <QVariant>
#include <QVector>
#include <QColor>

#include <unordered_map>

struct hkVariant;

namespace ckcmd {
    namespace HKX {

        class ProjectNode
        {

        public:

            enum class NodeType {
                fixed,
                support,
                characters_node,
                miscs_node,
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
                animation_style_node,
                action_node,
                idle_node
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
            virtual hkVariant* variant() const;
            virtual int file() const;
            virtual int project() const;

            bool isCharacter() const;

            bool canSaveOrExport() const;

            bool hasChild(ProjectNode* child);
            ProjectNode* appendChild(ProjectNode* child);
            void appendIndex(ProjectNode* item);
            void appendData(const QVariant& value);

            virtual ProjectNode* child(int row);
            ProjectNode* setChild(int row, ProjectNode* new_child);
            ProjectNode* removeChild(int row);
            void insertChild(int row, ProjectNode* new_child);
            virtual int childCount() const;
            virtual int columnCount() const;
            virtual QVariant data(int column) const;
            virtual NodeType type() const;

            const std::vector<int>& indicesOf(ProjectNode* child) const;

            virtual void setColor(QColor color);
            virtual QColor color() const;

            virtual std::string name();


            template<typename Visitor, typename ...Ts>
            void accept(Visitor& v, Ts... arg) {
                switch (m_type) {
                case NodeType::fixed:
                    v.visit<NodeType::fixed>(*this, arg...);
                    break;
                case NodeType::support:
                    v.visit<NodeType::support>(*this, arg...);
                    break;
                case NodeType::project_node:
                    v.visit<NodeType::project_node>(*this, arg...);
                    break;
                case NodeType::character_node:
                    v.visit<NodeType::character_node>(*this, arg...);
                    break;
                case NodeType::behavior_node:
                    v.visit<NodeType::behavior_node>(*this, arg...);
                    break;
                case NodeType::skeleton_node:
                    v.visit<NodeType::skeleton_node>(*this, arg...);
                    break;
                case NodeType::animation_node:
                    v.visit<NodeType::animation_node>(*this, arg...);
                    break;
                case NodeType::misc_node:
                    v.visit<NodeType::misc_node>(*this, arg...);
                    break;
                case NodeType::hkx_project_node:
                    v.visit<NodeType::hkx_project_node>(*this, arg...);
                    break;
                case NodeType::hkx_character_node:
                    v.visit<NodeType::hkx_character_node>(*this, arg...);
                    break;
                case NodeType::hkx_node:
                    v.visit<NodeType::hkx_node>(*this, arg...);
                    break;
                case NodeType::event_node:
                    v.visit<NodeType::event_node>(*this, arg...);
                    break;
                case NodeType::weapon_set_node:
                    v.visit<NodeType::weapon_set_node>(*this, arg...);
                    break;
                case NodeType::variable_node:
                    v.visit<NodeType::variable_node>(*this, arg...);
                    break;
                case NodeType::property_node:
                    v.visit<NodeType::property_node>(*this, arg...);
                    break;
                case NodeType::clip_event_node:
                    v.visit<NodeType::clip_event_node>(*this, arg...);
                    break;
                case NodeType::events_node:
                    v.visit<NodeType::events_node>(*this, arg...);
                    break;
                case NodeType::variables_node:
                    v.visit<NodeType::variables_node>(*this, arg...);
                    break;
                case NodeType::animation_styles_node:
                    v.visit<NodeType::animation_styles_node>(*this, arg...);
                    break;
                case NodeType::animation_style_node:
                    v.visit<NodeType::animation_style_node>(*this, arg...);
                    break;
                case NodeType::action_node:
                    v.visit<NodeType::action_node>(*this, arg...);
                    break;
                case NodeType::idle_node:
                    v.visit<NodeType::idle_node>(*this, arg...);
                    break;
                default:
                    break;
                }
            }

        protected:
            QVector<ProjectNode*> m_childItems;
            std::unordered_map<ProjectNode*, std::vector<int>> m_childIndices;
            QVector<QVariant> m_itemData;
            QVector<ProjectNode*> m_parentItems;
            QColor m_color;
        };
    }
}