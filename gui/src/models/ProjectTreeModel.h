#pragma once

#include <QAbstractProxyModel>

#include <src/models/ProjectModel.h>

namespace ckcmd {
    namespace HKX {

        class ProjectTreeModel : public QAbstractProxyModel {
            Q_OBJECT


        public:

            ProjectTreeModel(QObject* parent = nullptr) : QAbstractProxyModel(parent){}
            ~ProjectTreeModel() 
            {
            }

            void select(const QModelIndex& index);
            void activate(const QModelIndex& index);

            //Proxy
            //virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

            virtual void setSourceModel(ProjectModel* sourceModel);
            ProjectModel* sourceModel() const { return static_cast<ProjectModel*>(QAbstractProxyModel::sourceModel()); }

            virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
            virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

            virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
            virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

            virtual QModelIndex parent(const QModelIndex& child) const override;
            virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;

            virtual QVariant data(const QModelIndex& proxyIndex, int role) const override;

            //virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
            //virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;


        private slots:
            /**
             * QSortFilterProxyModel does it for us but QAbstractProxyModel does not!
             * So we have to map the source indices and reemit the dataChanged signal. */
            void sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

            void sourceBeginInsertChildren(const QModelIndex& parent, int first, int last);
            void sourceEndInsertChildren();

            void sourceBeginRemoveChildren(const QModelIndex& parent, int first, int last);
            void sourceEndRemoveChildren();
        };
    }
}