#include "animcacheclipinfo.h"

AnimCacheClipInfo::AnimCacheClipInfo(const QString & eventname, const QStringList & clipgens, uint unk)
    : eventName(eventname), clipGenerators(clipgens), unknown(unk)
{
    //
}

AnimCacheClipInfo::AnimCacheClipInfo(const AnimCacheClipInfo & other){
    eventName = other.eventName;
    unknown = other.unknown;
    clipGenerators = other.clipGenerators;
}

bool AnimCacheClipInfo::operator ==(const AnimCacheClipInfo & other) const{
    if (eventName != other.eventName || unknown != other.unknown || clipGenerators != other.clipGenerators){
        return false;
    }
    return true;
}

bool AnimCacheClipInfo::read(QFile *file){
    if (!file || !file->isOpen()){
        return false;
    }
    QByteArray line;
    auto numclips = 0U;
    auto ok = false;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    eventName = line;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    unknown = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    numclips = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    for (auto i = 0U; i < numclips; i++){
        if (file->atEnd()){
            return false;
        }
        line = file->readLine();
        line.chop(1);
        clipGenerators.append(line);
    }
    return true;
}

bool AnimCacheClipInfo::write(QFile *file, QTextStream &out) const{
    if (!file || !file->isOpen()){
        return false;
    }
    out << eventName << "\n";
    out << QString::number(unknown) << "\n";
    out << QString::number(clipGenerators.size()) << "\n";
    for (auto i = 0; i < clipGenerators.size(); i++){
        out << clipGenerators.at(i) << "\n";
    }
    return true;
}

void AnimCacheClipInfo::removeClipGenerator(const QString &clipname){
    clipGenerators.removeAll(clipname);
}

QString AnimCacheClipInfo::getEventName() const{
    return eventName;
}
