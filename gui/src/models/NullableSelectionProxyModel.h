#pragma once

#include <QAbstractProxyModel>

#include <src/models/ProjectModel.h>

namespace ckcmd {
    namespace HKX {

        class NullableSelectionProxyModel : public QAbstractProxyModel {
            Q_OBJECT

            QModelIndex _sourceRoot; // Persistent?

        public:

            NullableSelectionProxyModel(ProjectModel* sourceModel, const QModelIndex root, QObject* parent = nullptr);
            ~NullableSelectionProxyModel()
            {
            }

            virtual void setSourceModel(ProjectModel* sourceModel) { QAbstractProxyModel::setSourceModel(sourceModel); }
            ProjectModel* sourceModel() const { return static_cast<ProjectModel*>(QAbstractProxyModel::sourceModel()); }

            virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
            virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

            virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
            virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

            virtual QModelIndex parent(const QModelIndex& child) const override;
            virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;

            virtual QVariant data(const QModelIndex& proxyIndex, int role) const override;

            bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

            QModelIndex buddy(const QModelIndex& index) const override;
            Qt::ItemFlags flags(const QModelIndex& index) const override;

        };
    }
}