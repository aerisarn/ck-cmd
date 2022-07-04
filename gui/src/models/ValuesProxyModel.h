#pragma once

#include <QAbstractProxyModel>

#include <src/models/ProjectModel.h>

namespace ckcmd {
    namespace HKX {

        class ValuesProxyModel : public QAbstractProxyModel {
            Q_OBJECT

            QModelIndex _sourceRoot; // Persistent?
            int _row;
            int _firstColumn;

        public:

            ValuesProxyModel(ProjectModel* sourceModel, int row, int firstColumn, const QModelIndex root, QObject* parent = nullptr);
            ~ValuesProxyModel()
            {
            }

            //Proxy
            //virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

            virtual void setSourceModel(ProjectModel* sourceModel) { QAbstractProxyModel::setSourceModel(sourceModel); }
            ProjectModel* sourceModel() const { return static_cast<ProjectModel*>(QAbstractProxyModel::sourceModel()); }

            virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
            virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

            virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
            virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

            virtual QModelIndex parent(const QModelIndex& child) const override;
            virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;

            virtual QVariant data(const QModelIndex& proxyIndex, int role) const override;
            virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

            virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
            //virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
        };
    }
}