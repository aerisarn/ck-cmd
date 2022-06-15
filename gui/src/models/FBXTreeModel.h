#pragma once

#include <QAbstractItemModel>

#include <filesystem>
#include <fbxsdk.h>

namespace fs = std::filesystem;

namespace ckcmd {
    namespace HKX {

        struct FBXImportOptions {
            bool InvertU = false;
            bool InvertV = true; //DAMN openGL
        };

        enum class FBXTreeColumns
        {
            name = 0,
            transform,
            boneIndex,
            columnMax
        };

        class FBXTreeModel : public QAbstractItemModel {
            
            FbxScene* _scene;

            void ImportScene(const fs::path& fileName, const FBXImportOptions& options = FBXImportOptions());
            void CloseScene();

        public:

            FBXTreeModel(FbxNode* root);
            FBXTreeModel(const fs::path& file);


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