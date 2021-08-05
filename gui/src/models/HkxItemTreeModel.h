//#pragma once
//
//#include <QAbstractItemModel>
//#include <core/HKXWrangler.h>
//
//#include <src/models/ProjectNode.h>
//
//#include <hkbBehaviorReferenceGenerator_0.h>
//#include <hkbClipGenerator_2.h>
//#include <hkbProjectData_2.h>
//#include <hkbCharacterData_7.h>
//#include <hkbBehaviorGraph_1.h>
//#include <hkbExpressionDataArray_0.h>
//#include <hkbExpressionDataArray_0.h>
//#include <Animation/Animation/hkaAnimationContainer.h>
//#include <BSSynchronizedClipGenerator_1.h>
//
//namespace ckcmd {
//    namespace HKX {
//
//        class TreeBuilder;
//
//
//        class HkxItemTreeModel : public QAbstractItemModel {
//            Q_OBJECT
//            friend class TreeBuilder;
//        private:
//            const hkVariant* find(const void* object) const;
//            hkVariant* find(void* object);
//            int findIndex(const void* object) const;
//            int findIndex(void* object);
//
//            hkVariant* getObject(const QModelIndex& index) const;
//
//        public:
//
//            HkxItemTreeModel(const fs::path& file, QObject* parent = 0);
//            ~HkxItemTreeModel() {}
//
//            const hkVariant& at(size_t index) const {
//                return _objects[index];
//            }
//
//            hkVariant& at(size_t index)  {
//                return const_cast<hkVariant&>(const_cast<const HkxItemTreeModel*>(this)->at(index));
//            }
//
//            /*
//            ** AbstractItemModel(required methods)
//            */
//            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
//            QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const override;
//            QModelIndex parent(const QModelIndex& index) const override;
//            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
//            int columnCount(const QModelIndex& parent = QModelIndex()) const override;
//            QVariant headerData(int section, Qt::Orientation orientation,
//                int role = Qt::DisplayRole) const override;
//
//            Qt::ItemFlags flags(const QModelIndex& index) const override;
//            bool setData(const QModelIndex& index, const QVariant& value,
//                int role = Qt::EditRole) override;
//        private:
//            ProjectNode* _rootNode = NULL;
//            mutable hkVariant* _root = NULL;
//            hkArray<hkVariant> _objects;
//            fs::path _file;
//        };
//    }
//}