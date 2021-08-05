#ifndef SKYRIMANIMSETDATA_H
#define SKYRIMANIMSETDATA_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QFile>
#include <QTextStream>

#include "animcacheprojectdata.h"

class SkyrimAnimSetData final
{
public:
    SkyrimAnimSetData() = default;
    SkyrimAnimSetData& operator=(const SkyrimAnimSetData&) = default;
    SkyrimAnimSetData(const SkyrimAnimSetData &) = default;
    ~SkyrimAnimSetData();
public:
    bool parse(QFile * file);
    bool write(const QString & filename);
    bool addNewProject(const QString & projectname);
    bool addAnimationToCache(const QString & projectname, const QString & eventname, const QVector<AnimCacheAnimationInfo *> &animations, const QVector<AnimCacheVariable *> &vars = QVector <AnimCacheVariable *> (), const QVector<AnimCacheClipInfo *> &clips = QVector <AnimCacheClipInfo *> ());
    bool removeAnimationFromCache(const QString & projectname, const QString & animationname, const QString & variablename = "", const QString & clipname = "");
    AnimCacheProjectData *getProjectCacheData(const QString &name);
    bool mergeAnimationCaches(const QString & projectname, const QString &recessiveprojectname, SkyrimAnimSetData *recessiveprojectdata);
    bool isEmpty() const;
private:
    QStringList projectNames;
    QVector <AnimCacheProjectData *> projects;
};

#endif // SKYRIMANIMSETDATA_H
