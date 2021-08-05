#ifndef PROJECTANIMDATA_H
#define PROJECTANIMDATA_H

#include "skyrimclipgeneratodata.h"
#include "skyrimanimationmotiondata.h"

class ProjectAnimData final
{
    friend class ProjectFile;
    friend class SkyrimAnimData;
public:
    ProjectAnimData();
    ProjectAnimData& operator=(const ProjectAnimData&) = delete;
    ProjectAnimData(const ProjectAnimData &) = delete;
    ~ProjectAnimData();
public:
    qreal getAnimationDuration(int animationindex) const;
    SkyrimAnimationMotionData getAnimationMotionData(int animationindex) const;
    void merge(ProjectAnimData *recessiveanimdata);
    SkyrimAnimationMotionData * findMotionData(int animationindex);
    bool appendAnimation(SkyrimAnimationMotionData *motiondata);
    bool removeAnimation(int animationindex);
private:
    bool read(QFile * file);
    bool readMotionOnly(QFile *file);
    bool write(QFile &file, QTextStream &output);
    bool appendClipGenerator(SkyrimClipGeneratoData *animData);
    bool removeClipGenerator(const QString & clipname);
    void setLocalTimeForClipGenAnimData(const QString &clipname, int triggerindex, qreal time);
    void setEventNameForClipGenAnimData(const QString &clipname, int triggerindex, const QString &eventname);
    void setClipNameAnimData(const QString &oldclipname, const QString &newclipname);
    void setAnimationIndexForClipGen(const QString &clipGenName, int index);
    void setPlaybackSpeedForClipGen(const QString & clipGenName, qreal speed);
    void setCropStartAmountLocalTimeForClipGen(const QString & clipGenName, qreal time);
    void setCropEndAmountLocalTimeForClipGen(const QString & clipGenName, qreal time);
    void appendClipTriggerToAnimData(const QString & clipGenName, const QString & eventname);
    void removeClipTriggerToAnimDataAt(const QString & clipGenName, int index);
    bool removeBehaviorFromProject(const QString &behaviorname);
    void fixNumberAnimationLines();
private:
    ulong animationDataLines;
    ulong animationMotionDataLines;
    QStringList projectFiles;
    QVector <SkyrimClipGeneratoData *> animationData;
    QVector <SkyrimAnimationMotionData *> animationMotionData;
};

#endif // PROJECTANIMDATA_H
