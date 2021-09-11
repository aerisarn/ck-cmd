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
                property_node,
                clip_event_node
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
            void appendData(const QVariant& value);

            virtual ProjectNode* child(int row);
            ProjectNode* setChild(int row, ProjectNode* new_child);
            virtual int childCount() const;
            virtual int columnCount() const;
            virtual QVariant data(int column) const;
            virtual int row() const;
            virtual ProjectNode* parentItem();
            virtual NodeType type() const;

        protected:
            QVector<ProjectNode*> m_childItems;
            QVector<QVariant> m_itemData;
            ProjectNode* m_parentItem;
        };
    }
}