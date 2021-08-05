#include "animcacheanimationinfo.h"
#include "hkcrc.h"

const QString AnimCacheAnimationInfo::XKH = "7891816";

AnimCacheAnimationInfo::AnimCacheAnimationInfo(const QString & path, const QString & name, bool compute){
    HkCRC crcGen;
    if (compute){
        crcPath = crcGen.compute(QString(path).replace("/", "\\").toLower().toLocal8Bit());
        crcPath = QString::number(crcPath.toULong(nullptr, 16));
        crcAnimationName = crcGen.compute(name.toLower().toLocal8Bit());
        crcAnimationName = QString::number(crcAnimationName.toULong(nullptr, 16));
    }else{
        crcPath = path;
        crcAnimationName = name;
    }
}

bool AnimCacheAnimationInfo::operator ==(const AnimCacheAnimationInfo & other) const{
    if (crcPath != other.crcPath || crcAnimationName != other.crcAnimationName){
        return false;
    }
    return true;
}

bool AnimCacheAnimationInfo::read(QFile *file){
    if (!file || !file->isOpen()){
        return false;
    }
    QByteArray line;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    crcPath = line;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    crcAnimationName = line;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    if (line != XKH){
        return false;
    }
    return true;
}

bool AnimCacheAnimationInfo::write(QFile *file, QTextStream &out) const{
    if (!file || !file->isOpen()){
        return false;
    }
    out << crcPath << "\n";
    out << crcAnimationName << "\n";
    out << XKH << "\n";
    return true;
}

void AnimCacheAnimationInfo::setAnimationData(const QString &path, const QString &name, bool compute){
    if (compute){
        HkCRC crcGen;
        crcPath = crcGen.compute(QString(path).replace("/", "\\").toLower().toLocal8Bit());
        crcPath = QString::number(crcPath.toULong(nullptr, 16));
        crcAnimationName = crcGen.compute(name.toLower().toLocal8Bit());
        crcAnimationName = QString::number(crcAnimationName.toULong(nullptr, 16));
    }else{
        crcPath = path;
        crcAnimationName = name;
    }
}

QString AnimCacheAnimationInfo::getCrcAnimationName() const{
    return crcAnimationName;
}
