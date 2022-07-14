#pragma once

#include <QAbstractItemModel>
#include <src/hkx/CommandManager.h>
#include <src/hkx/ResourceManager.h>
#include <src/models/ModelEdge.h>

#include <src/utility/bimap.h>

struct hkVariant;

namespace ckcmd {
    namespace HKX {

        class ProjectTreeModel;

        class ProjectModel : public QAbstractItemModel {
            friend class ProjectTreeModel;

            Q_OBJECT

            std::unordered_map<ModelEdge*, qintptr, ModelEdgeHasher, ModelEdgeEqualFn> _reverse_find;
            std::unordered_map<qintptr, ModelEdge> _direct_find;

            qintptr modelEdgeIndex(const ModelEdge& edge) const;
            const ModelEdge& modelEdge(const QModelIndex& index) const;
            ModelEdge& modelEdge(const QModelIndex& index);

            bool hasModelEdgeIndex(const ModelEdge& edge) const;
            qintptr createModelEdgeIndex(const ModelEdge& edge);
            void deleteAllModelEdgeIndexesForFile(int project_file);
            QModelIndex getChildAssetProxy(const QModelIndex& index, NodeType proxy_type);

            CommandManager& _commandManager;
            ResourceManager& _resourceManager;

            QModelIndex _copyPointer;

        public:

            ProjectModel(CommandManager& commandManager, ResourceManager& resourceManager, QObject* parent = 0);
            ~ProjectModel() 
            {
            }

            void select(const QModelIndex& index);
            void activate(const QModelIndex& index);

            NodeType nodeType(const QModelIndex& index) const;

            hkVariant* variant(const QModelIndex& index);
            bool isVariant(const QModelIndex& index);
            bool isAssetsNode(const QModelIndex& index);
            bool isAssetNameValid(const QModelIndex& index, const QString& name, AssetType type);
            int getProjectIndex(const QModelIndex& index);
            int getRigIndex(const QModelIndex& index);
            int getFileIndex(const QModelIndex& index);
            std::pair<int, int> dataStart(const QModelIndex& index);
            ResourceManager& getResourceManager() { return _resourceManager; }

            QModelIndex variablesIndex(const QModelIndex& index);
            QAbstractItemModel* editModel(const QModelIndex& index, AssetType type, const QString& nullValue = "<Not Set>", int role = Qt::DisplayRole);
            void SetCopyPointer(const QModelIndex& index);
            bool PasteEnabled(const QModelIndex& index);
            TypeInfo rowType(const QModelIndex& index);
            std::vector<TypeInfo> rowTypes(const QModelIndex& index);
            bool isArray(const QModelIndex& index);
            QStringList rowNames(const QModelIndex& index, const QString& prefix_filter = QString(""));

            /*
            ** AbstractItemModel(required methods)
            */
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
            QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const override;
            QModelIndex parent(const QModelIndex& index) const override;

            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            int columnCount(const QModelIndex& parent = QModelIndex()) const override;

            bool hasIndex(int row, int column, const QModelIndex& parent = QModelIndex()) const;

            //int childCount(const QModelIndex& parent = QModelIndex())  const;
            //virtual bool hasChild(int row, int column, const QModelIndex& parent = QModelIndex()) const;

            bool hasChildren(const QModelIndex& parent) const override;
            QVariant headerData(int section, Qt::Orientation orientation,
                int role = Qt::DisplayRole) const override;

            Qt::ItemFlags flags(const QModelIndex& index) const override;
            bool setData(const QModelIndex& index, const QVariant& value,
                int role = Qt::EditRole) override;

            virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
            virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
            bool ProjectModel::remove(const QModelIndex& index);
            virtual bool insertColumns(int row, int column, int count, const QModelIndex& parent = QModelIndex());
            virtual bool removeColumns(int row, int column, int count, const QModelIndex& parent = QModelIndex());
            //virtual bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
            //virtual bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;

            /*
            ** Hierarchical model
            */
            int childCount(const QModelIndex& parent = QModelIndex()) const;
            QModelIndex child(int child_index, const QModelIndex& parent = QModelIndex()) const;
            int childIndex(const QModelIndex& child) const;

        public slots:
            void refreshAssetList(const QModelIndex& parent, AssetType type);

        Q_SIGNALS:
            void beginInsertChildren(const QModelIndex& parent, int first, int last);
            void endInsertChildren();

            void beginRemoveChildren(const QModelIndex& parent, int first, int last);
            void endRemoveChildren();

        };
    }
}