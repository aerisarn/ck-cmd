#pragma once

#include <QVariant>
#include <QVector>

namespace ckcmd {
    namespace HKX {

        class ProjectNode
        {
        public:

            enum class NodeType {
                support,
                project_node,
                character_node,
                behavior_node,
                skeleton_node,
                animation_node,
                misc_node,
                hkx_project_node,
                hkx_character_node,
                hkx_field_node,
                hkx_node,
            } m_type;

            explicit ProjectNode(NodeType type, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            ~ProjectNode();

            static ProjectNode* createSupport(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createProject(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createCharacter(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createBehavior(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createSkeleton(const QVector<QVariant>& data, ProjectNode* parentItemm = nullptr);
            static ProjectNode* createAnimation(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createMisc(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createHkxProject(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createHkxCharacter(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createHkxNode(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
            static ProjectNode* createHkxFieldNode(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);

            bool isProjectRoot();
            bool isVariant();

            ProjectNode* appendChild(ProjectNode* child);

            ProjectNode* child(int row);
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