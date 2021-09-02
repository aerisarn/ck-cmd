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
                hkx_project_node,
                hkx_character_node,
                hkx_node,
                event_node,
                variable_node,
                property_node
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

            ProjectNode* appendChild(ProjectNode* child);

            ProjectNode* child(int row);
            ProjectNode* setChild(int row, ProjectNode* new_child);
            int childCount() const;
            int columnCount() const;
            QVariant data(int column) const;
            int row() const;
            ProjectNode* parentItem();
            NodeType type() const;

        private:
            QVector<ProjectNode*> m_childItems;
            QVector<QVariant> m_itemData;
            ProjectNode* m_parentItem;
        };
    }
}