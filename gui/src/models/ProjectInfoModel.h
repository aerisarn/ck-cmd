#pragma once

#include <QAbstractTableModel>
#include <QObject>
#include <QStringList>

#include <utility>

#include <core/AnimationCache.h>
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

class ProjectInfoModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    typedef std::pair<std::string, CacheEntry*> ProjectEntry;

    ProjectInfoModel(ProjectEntry entry, QObject* parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void setData(ProjectEntry entry);

    const std::string& name() { return _entry.first; }


private:

    void readObjects();

    ProjectEntry _entry;
    hkArray<hkVariant> _project_objects;
    hkArray<hkVariant> _character_objects;


    fs::path _project_folder;
    
};