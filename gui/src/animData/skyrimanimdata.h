#ifndef SKYRIMANIMDATA_H
#define SKYRIMANIMDATA_H

#include <QString>
#include <QFile>
#include <QVector>

#include "projectanimdata.h"

class QTextStream;

class SkyrimAnimData final
{
    friend class ProjectFile;
public:
    SkyrimAnimData() = default;
    SkyrimAnimData& operator=(const SkyrimAnimData&) = default;
    SkyrimAnimData(const SkyrimAnimData &) = default;
    ~SkyrimAnimData();
public:
    int getProjectIndex(const QString & projectname) const;
    ProjectAnimData * getProjectAnimData(const QString & projectname) const;
    bool isProjectNameTaken(const QString &name) const;
    qreal getAnimationDurationFromAnimData(const QString &projectname, int animationindex) const;
    SkyrimAnimationMotionData getAnimationMotionData(const QString &projectname, int animationindex) const;
    void merge(SkyrimAnimData *recessiveanimdata, const QString &projecttomerge);
    bool isEmpty() const;
    bool write(const QString & filename);
private:
    int addNewProject(const QString &projectname, const QStringList &projectfilerelativepaths);
    bool removeBehaviorFromProject(const QString &projectname,  const QString & behaviorfilename);
    bool parse(QFile * file, const QString &projectToIgnore = "", const QStringList & behaviorfilenames = QStringList());
    bool appendClipGenerator(const QString &projectname, SkyrimClipGeneratoData *animdata);
    bool removeClipGenerator(const QString & projectname, const QString & clipname);
    bool appendAnimation(const QString & projectname, SkyrimAnimationMotionData *motiondata);
    bool removeAnimation(const QString & projectname, int animationindex);
    void setLocalTimeForClipGenAnimData(const QString &projectname, const QString &clipname, int triggerindex, qreal time);
    void setEventNameForClipGenAnimData(const QString &projectname, const QString &clipname, int triggerindex, const QString &eventname);
    void setClipNameAnimData(const QString &projectname, const QString &oldclipname, const QString &newclipname);
    void setAnimationIndexForClipGen(const QString & projectname, const QString &clipGenName, int index);
    void setPlaybackSpeedAnimData(const QString & projectname, const QString & clipGenName, qreal speed);
    void setCropStartAmountLocalTimeAnimData(const QString & projectname, const QString & clipGenName, qreal time);
    void setCropEndAmountLocalTimeAnimData(const QString & projectname, const QString & clipGenName, qreal time);
    void appendClipTriggerToAnimData(const QString & projectname, const QString & clipGenName, const QString &eventname);
    void removeClipTriggerToAnimDataAt(const QString & projectname, const QString & clipGenName, int index);
private:
    QStringList projectNames;
    QVector <ProjectAnimData *> animData;
};

#endif // SKYRIMANIMDATA_H
