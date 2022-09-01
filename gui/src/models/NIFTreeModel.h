#pragma once

#include <QAbstractItemModel>

#include <filesystem>
#include <core/NifFile.h>

namespace fs = std::filesystem;

namespace ckcmd {
    namespace HKX {

        Niflib::BSLightingShaderPropertyRef getProperty(Niflib::NiAVObjectRef mesh);

        class NIFTreeModel : public QAbstractItemModel {
            
            fs::path _folder;
            std::vector<NIF::NifFile> _files;

        public:

            NIFTreeModel(const fs::path& directory);
            ~NIFTreeModel();

            NIF::NifFile& file(QModelIndex index);
            Niflib::NiAVObjectRef block(QModelIndex index);

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