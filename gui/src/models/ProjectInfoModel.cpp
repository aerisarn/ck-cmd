#include "ProjectInfoModel.h"
#include "HkxItemTableModel.h"
#include "HkxItemTreeModel.h"
#include "src/config.h"

#include <QTableView>
#include <QTreeView>

using namespace ckcmd::HKX;

ProjectInfoModel::ProjectInfoModel(ProjectEntry entry, QObject* parent) :
    _entry(entry),
    QAbstractTableModel(parent)
{
//    readObjects();
}

void ProjectInfoModel::readObjects() {
    HKXWrapper temp;
    std::string workspace_folder = Settings.get<std::string>("global\workspace_folder");
    for (auto& p : fs::recursive_directory_iterator(workspace_folder))
    {
        if (p.is_regular_file() && AnimationCache::iequals(p.path().filename().string(),_entry.first + ".hkx"))
        {
            //found
            //HkxItemModel* m = new HkxItemModel(p.path());
            //temp.read(p.path(), _project_objects);

            //HKXWrapper temp;
            //temp.read(p.path(), _project_objects);
            //for (int i = 0; i < _project_objects.getSize(); i++) {

            //        QTableView* table_view = new QTableView;
            //        table_view->setWindowTitle("Data View");
            //        HkxItemTableModel* m = new HkxItemTableModel(&_project_objects[i]);
            //        table_view->setModel(m);
            //        table_view->show();
            //}

            //HkxItemTreeModel* model = new HkxItemTreeModel(p.path());

            //QTableView* table_view = new QTableView;
            //table_view->setWindowTitle("Data View");
            //table_view->setModel(m);
            //table_view->show();

            //QTreeView* tree_view = new QTreeView;
            //tree_view->setWindowTitle("Hierarchy View");
            //tree_view->setModel(model);
            //tree_view->show();
            //
            //_project_folder = p.path().parent_path();
            return;
        }
    }
}

void ProjectInfoModel::setData(ProjectEntry entry) {
    _entry = entry; 
    readObjects();
    
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1); 
    
    emit dataChanged(topLeft, bottomRight);
}

int ProjectInfoModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int ProjectInfoModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant ProjectInfoModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return "files";
    }
    else if (index.column() == 1) {
        auto files = _entry.second->block.getProjectFiles();

        return QString::fromStdString(files.getBlock());
    }
    return QVariant();
}

QVariant ProjectInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Field");
        }
        else if (section == 1) {
            return QString("Value");
        }
    }
    return QVariant();
}