#ifndef SKYRIMCLIPGENERATODATA_H
#define SKYRIMCLIPGENERATODATA_H

#include <QTextStream>

#include "skyrimcliptrigger.h"

class ProjectAnimData;

class SkyrimClipGeneratoData final
{
    friend class ProjectAnimData;
public:
    SkyrimClipGeneratoData(ProjectAnimData *par, const QString & name, uint ind = 0, qreal speed = 0, qreal startcrop = 0, qreal endcrop = 0, const QVector <SkyrimClipTrigger> & trigs = QVector <SkyrimClipTrigger>());
    SkyrimClipGeneratoData& operator=(const SkyrimClipGeneratoData&) = default;
    SkyrimClipGeneratoData(const SkyrimClipGeneratoData &) = default;
    ~SkyrimClipGeneratoData() = default;
public:
    uint lineCount() const;
    QString getClipGeneratorName() const;
private:
    bool removeTrigger(int index);
    void addTrigger(const SkyrimClipTrigger &trig = SkyrimClipTrigger());
    void rearrangeTriggers();
    bool read(QFile * file, ulong &lineCount);
    bool write(QFile * file, QTextStream & out);
private:
    ProjectAnimData *parent;
    QString clipGeneratorName;
    uint animationIndex;
    qreal playbackSpeed;
    qreal cropStartTime;
    qreal cropEndTime;
    QVector <SkyrimClipTrigger> triggers;
};

#endif // SKYRIMCLIPGENERATODATA_H
