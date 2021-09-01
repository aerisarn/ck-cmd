#pragma once

#include <QAbstractTableModel>
#include <QUndoStack>
#include <core/HKXWrangler.h>
#include <src/hkx/ISpecialFieldsHandler.h>

namespace ckcmd {
    namespace HKX {

        class HkxItemTableModel : public QAbstractTableModel, public SpecialFieldsListener {
            Q_OBJECT

            hkVariant* _variant;
            hkVariant* _parent;
            int _file;
            hkVariant* getObject(const QModelIndex& index) const;

            bool indexValid(const QModelIndex& index) const;

        public:

            HkxItemTableModel(hkVariant* variant, int file, hkVariant* variant_parent, QObject* parent = 0);
            ~HkxItemTableModel() {}

            QVariant headerData(int section, Qt::Orientation orientation,
                int role = Qt::DisplayRole) const override;


            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
            bool setData(const QModelIndex& index, const QVariant& value,
                int role = Qt::EditRole) override;

            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            int columnCount(const QModelIndex& parent = QModelIndex()) const override;

            Qt::ItemFlags flags(const QModelIndex& index) const override;
        };
    }
}