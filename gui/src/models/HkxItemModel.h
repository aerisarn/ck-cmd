#pragma once

#include <QAbstractItemModel>
#include <core/HKXWrangler.h>

#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbClipGenerator_2.h>
#include <hkbProjectData_2.h>
#include <hkbCharacterData_7.h>
#include <hkbBehaviorGraph_1.h>
#include <hkbExpressionDataArray_0.h>
#include <hkbExpressionDataArray_0.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <BSSynchronizedClipGenerator_1.h>

//Proof of concept, but unuseful. look for table/tree models instead

namespace ckcmd {
    namespace HKX {

        class HkxTableVariant;
        class HkxItemVisitor;

        class HkxItemModel : public QAbstractItemModel {
            Q_OBJECT
            friend class HkxTableVariant;
            friend class HkxItemVisitor;
        private:
            const hkVariant* find(const void* object) const;
            hkVariant* find(void* object);
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
                        child < rhs.child;  //assume that you compare the record based on a
                }

            };

            HkxItemModel(const fs::path& file, QObject* parent = 0);
            ~HkxItemModel() {}

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
        private:
            mutable hkVariant* _root;
            hkArray<hkVariant> _objects;
            std::set<HkxItemLink> _linkmap;
            fs::path _file;
        };
    }
}