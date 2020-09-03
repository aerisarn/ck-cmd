#ifndef SKYRIMCLIPTRIGGER_H
#define SKYRIMCLIPTRIGGER_H

#include <QtCore>

class SkyrimClipTrigger final
{
public:
    SkyrimClipTrigger(qreal t = 0, const QString & trigname = "");
    SkyrimClipTrigger& operator=(const SkyrimClipTrigger&) = default;
    SkyrimClipTrigger(const SkyrimClipTrigger &) = default;
    ~SkyrimClipTrigger() = default;
public:
    QString name;
    qreal time;
};

#endif // SKYRIMCLIPTRIGGER_H
