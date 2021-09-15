#pragma once

#include <QAbstractItemModel>
#include <src/models/ProjectNode.h>
#include <src/hkx/CommandManager.h>
#include <src/hkx/ResourceManager.h>

//Proof of concept, but unuseful. look for table/tree models instead

namespace ckcmd {
    namespace HKX {

        class ProjectTreeModel : public QAbstractItemModel {
            Q_OBJECT
                friend class TreeBuilder;

            CommandManager& _commandManager;
            ResourceManager& _resourceManager;

        public:

            ProjectTreeModel(CommandManager& commandManager, ResourceManager& resourceManager, QObject* parent = 0);
            ~ProjectTreeModel() {
            
            }


            ProjectNode* getNode(const QModelIndex& index) const;
            void notifyBeginInsertRows(const QModelIndex& index, int first, int last) {
                emit beginInsertRows(index, first, last);
            }

            void notifyEndInsertRows() {
                emit endInsertRows();
            }

            void notifyBeginRemoveRows(const QModelIndex& index, int first, int last) {
                emit beginRemoveRows(index, first, last);
            }

            void notifyEndRemoveRows() {
                emit endRemoveRows();
            }

            void notifyBeginMoveRows(const QModelIndex& sourceParent, int sourceFirst, int sourceLast, const QModelIndex& destinationParent, int destinationRow) {
                emit beginMoveRows(sourceParent, sourceFirst, sourceLast, destinationParent, destinationRow);
            }

            void notifyEndMoveRows() {
                emit endMoveRows();
            }

            void notifyElementChanged(const QModelIndex& index) {
                emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
            }


            /*
            ** AbstractItemModel(required methods)
            */
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
            QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const override;
            QModelIndex parent(const QModelIndex& index) const override;
            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            int columnCount(const QModelIndex& parent = QModelIndex()) const override;
            QVariant headerData(int section, Qt::Orientation orientation,
                int role = Qt::DisplayRole) const override;

            Qt::ItemFlags flags(const QModelIndex& index) const override;
            bool setData(const QModelIndex& index, const QVariant& value,
                int role = Qt::EditRole) override;

            QModelIndex getIndex(ProjectNode* node) const;

            ProjectNode* _rootNode = NULL;

        };
    }
}