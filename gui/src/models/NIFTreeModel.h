#pragma once

#include <QAbstractItemModel>

#include <filesystem>
#include <core/NifFile.h>

namespace fs = std::filesystem;

namespace ckcmd {
    namespace HKX {

        class NIFTreeModel : public QAbstractItemModel {
            
            fs::path _folder;
            std::vector<NIF::NifFile> _files;

        public:

            NIFTreeModel(const fs::path& directory);
            ~NIFTreeModel();

            Niflib::NiTriShapeRef block(QModelIndex index);

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
        };
    }
}