#pragma once

#include <QAbstractTableModel>
#include <core/HKXWrangler.h>

namespace ckcmd {
    namespace HKX {

        class HkxItemTableModel : public QAbstractTableModel {
            Q_OBJECT

            hkVariant* _variant;

            hkVariant* getObject(const QModelIndex& index) const;

        public:

            struct HkxItemLink {
                size_t column;
                size_t row;
                hkVariant* parent;
                hkVariant* child;

                bool operator<(const HkxItemLink& rhs) const
                {
                    return column < rhs.column&&
                        row < rhs.row&&
                        parent < rhs.parent&&
                        child < rhs.child;
                }

            };

            HkxItemTableModel(hkVariant* variant, QObject* parent = 0);
            ~HkxItemTableModel() {}

            QVariant headerData(int section, Qt::Orientation orientation,
                int role = Qt::DisplayRole) const override;


            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
            bool setData(const QModelIndex& index, const QVariant& value,
                int role = Qt::EditRole) override;

            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        private:
            
        };
    }
}