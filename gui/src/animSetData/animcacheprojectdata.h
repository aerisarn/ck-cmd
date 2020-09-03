#ifndef ANIMCACHEPROJECTDATA_H
#define ANIMCACHEPROJECTDATA_H

#include <QtCore>

#include "animcacheanimsetdata.h"

namespace UI {
	class AnimationCacheUI;
}

class AnimCacheProjectData final
{
    friend class UI::AnimationCacheUI;
    friend class SkyrimAnimSetData;
public:
    AnimCacheProjectData(const QStringList & files = QStringList(), const QVector<AnimCacheAnimSetData *> &animdata = QVector <AnimCacheAnimSetData *>());
    AnimCacheProjectData& operator=(const AnimCacheProjectData&) = delete;
    AnimCacheProjectData(const AnimCacheProjectData &) = delete;
    ~AnimCacheProjectData();
public:
    bool read(QFile * file);
    bool write(QFile *file, QTextStream &out) const;
    bool merge(AnimCacheProjectData *recessiveproject);
private:
    QStringList fileNames;
    QVector <AnimCacheAnimSetData *> animSetData;
};

#endif // ANIMCACHEPROJECTDATA_H
