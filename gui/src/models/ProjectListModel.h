#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>

#include <core/AnimationCache.h>



class ProjectListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ProjectListModel(AnimationCache& cache, QObject* parent = 0)
        : QAbstractListModel(parent), _cache(cache) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());



private:
    AnimationCache& _cache;
};