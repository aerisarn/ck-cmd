#include "animcachevariable.h"

AnimCacheVariable::AnimCacheVariable(const QString & name, uint val1, uint val2)
    : name(name), minValue(val1), maxValue(val2)
{
    //
}

AnimCacheVariable::AnimCacheVariable(const AnimCacheVariable & other){
    name = other.name;
    minValue = other.minValue;
    maxValue = other.maxValue;
}

bool AnimCacheVariable::operator ==(const AnimCacheVariable & other) const{
    if (name != other.name || minValue != other.minValue || maxValue != other.maxValue){
        return false;
    }
    return true;
}

bool AnimCacheVariable::read(QFile *file){
    if (!file || !file->isOpen()){
        return false;
    }
    QByteArray line;
    auto ok = false;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    name = line;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    minValue = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    maxValue = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    return true;
}

bool AnimCacheVariable::write(QFile *file, QTextStream &out) const{
    if (!file || !file->isOpen()){
        return false;
    }
    out << name << "\n";
    out << QString::number(minValue) << "\n";
    out << QString::number(maxValue) << "\n";
    return true;
}

QString AnimCacheVariable::getName() const{
    return name;
}
