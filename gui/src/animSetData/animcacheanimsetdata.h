#ifndef ANIMCACHEANIMSETDATA_H
#define ANIMCACHEANIMSETDATA_H

#include <QtCore>

#include "animcachevariable.h"
#include "animcacheanimationinfo.h"
#include "animcacheclipinfo.h"

class AnimCacheAnimSetData final
{
    friend class CacheWidget;
    friend class SkyrimAnimSetData;
public:
    AnimCacheAnimSetData(const QStringList & events = QStringList(), const QVector<AnimCacheVariable *> &vars = QVector <AnimCacheVariable *>(),
                const QVector<AnimCacheClipInfo *> &clips = QVector <AnimCacheClipInfo *>(), const QVector<AnimCacheAnimationInfo *> &anims = QVector <AnimCacheAnimationInfo *>());
    AnimCacheAnimSetData& operator=(const AnimCacheAnimSetData&) = delete;
    AnimCacheAnimSetData(const AnimCacheAnimSetData &) = delete;
    ~AnimCacheAnimSetData() = default;
public:
    bool read(QFile * file);
    bool write(QFile *file, QTextStream &out) const;
    bool merge(AnimCacheAnimSetData *recessiveproject);
private:
    bool addAnimationToCache(const QString & event, const QVector<AnimCacheAnimationInfo *> &anims, const QVector<AnimCacheVariable *> &vars = QVector <AnimCacheVariable *> (), const QVector<AnimCacheClipInfo *> &clips = QVector <AnimCacheClipInfo *> ());
    void removeAnimationFromCache(const QString & animationname, const QString & clipname = "", const QString & variablename = "");
private:
    QStringList cacheEvents;
    QVector <AnimCacheVariable *> behaviorVariables;
    QVector <AnimCacheClipInfo *> clipGenerators;
    QVector <AnimCacheAnimationInfo *> animations;
};

#endif // ANIMCACHEANIMSETDATA_H
