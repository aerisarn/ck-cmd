#include "skyrimanimationmotiondata.h"
#include "projectanimdata.h"
#include "src/utility.h"

using namespace UI;

SkyrimAnimationMotionData::SkyrimAnimationMotionData(ProjectAnimData *par, uint ind, qreal dur, const QVector <SkyrimAnimationTranslation> & trans, const QVector <SkyrimAnimationRotation> & rots)
    : parent(par), animationIndex(ind), duration(dur), translations(trans), rotations(rots)
{
    //
}

bool SkyrimAnimationMotionData::read(QFile *file, ulong &lineCount){
    SkyrimAnimationTranslation translation;
    SkyrimAnimationRotation rotation;
    QByteArray line;
    QList <QByteArray> list = {"0.000000", "0.000000", "0.000000", "0.000000"};
    auto ok = false;
    auto index = 0U;
    if (file && file->isOpen()){
        if (!chopLine(file, line, lineCount)){
            return false;
        }
        index = line.toUInt(&ok);
        if (!ok){
            return false;
        }
        animationIndex = index;
        if (!chopLine(file, line, lineCount)){
            return false;
        }
        auto value = line.toDouble(&ok);
        if (!ok){
            return false;
        }
        duration = value;
        if (!chopLine(file, line, lineCount)){
            return false;
        }
        auto count = line.toUInt(&ok);
        if (!ok){
            return false;
        }
        for (auto i = 0U; i < count; i++){
            if (!chopLine(file, line, lineCount)){
                return false;
            }
            list = line.split(' ');
            if (list.size() != 4){
                return false;
            }
            translation.localTime = list[0].toDouble(&ok);
            if (!ok){
                return false;
            }
            translation.x = list[1].toDouble(&ok);
            if (!ok){
                return false;
            }
            translation.y = list[2].toDouble(&ok);
            if (!ok){
                return false;
            }
            translation.z = list[3].toDouble(&ok);
            if (!ok){
                return false;
            }
            translations.append(translation);
        }
        if (!chopLine(file, line, lineCount)){
            return false;
        }
        count = line.toUInt(&ok);
        if (!ok){
            return false;
        }
        for (auto i = 0U; i < count; i++){
            if (!chopLine(file, line, lineCount)){
                return false;
            }
            list = line.split(' ');
            if (list.size() != 5){
                return false;
            }
            rotation.localTime = list[0].toDouble(&ok);
            if (!ok){
                return false;
            }
            rotation.x = list[1].toDouble(&ok);
            if (!ok){
                return false;
            }
            rotation.y = list[2].toDouble(&ok);
            if (!ok){
                return false;
            }
            rotation.z = list[3].toDouble(&ok);
            if (!ok){
                return false;
            }
            rotation.w = list[4].toDouble(&ok);
            if (!ok){
                return false;
            }
            rotations.append(rotation);
        }
        if (!chopLine(file, line, lineCount) || line != ""){
            return false;
        }
    }else{
        return false;
    }
    return true;
}

bool SkyrimAnimationMotionData::operator==(const SkyrimAnimationMotionData &other) const{
    if (animationIndex != other.animationIndex/* || duration != other.duration || translations.size() != other.translations.size() ||
            rotations.size() != other.rotations.size()*/)
    {
        return false;
    }
    return true;
}

bool SkyrimAnimationMotionData::write(QFile * file, QTextStream & out) const{
    if (!file || !file->isOpen()){
        return false;
    }
    out << QString::number(animationIndex) << "\n";
    out << trimFloat(QString::number(duration, char('f'), 6)) << "\n";
    out << QString::number(translations.size()) << "\n";
    for (auto i = 0; i < translations.size(); i++){
        out << trimFloat(QString::number(translations.at(i).localTime, char('f'), 6));
        out << " "+trimFloat(QString::number(translations.at(i).x, char('f'), 6));
        out << " "+trimFloat(QString::number(translations.at(i).y, char('f'), 6));
        out << " "+trimFloat(QString::number(translations.at(i).z, char('f'), 6)) << "\n";
    }
    out << QString::number(rotations.size()) << "\n";
    for (auto i = 0; i < rotations.size(); i++){
        out << trimFloat(QString::number(rotations.at(i).localTime, char('f'), 6));
        out << " "+trimFloat(QString::number(rotations.at(i).x, char('f'), 6));
        out << " "+trimFloat(QString::number(rotations.at(i).y, char('f'), 6));
        out << " "+trimFloat(QString::number(rotations.at(i).z, char('f'), 6));
        out << " "+trimFloat(QString::number(rotations.at(i).w, char('f'), 6)) << "\n";
    }
    out << "\n";
    return true;
}

void SkyrimAnimationMotionData::addTranslation(const SkyrimAnimationTranslation & trans){
    translations.append(trans);
    //parent->animationMotionDataLines++;
}

bool SkyrimAnimationMotionData::removeTranslation(int index){
    if (index >= 0 && index < translations.size()){
        translations.removeAt(index);
        //parent->animationMotionDataLines--;
        return true;
    }
    return false;
}

void SkyrimAnimationMotionData::addRotation(const SkyrimAnimationRotation & rot){
    rotations.append(rot);
    //parent->animationMotionDataLines++;
}
bool SkyrimAnimationMotionData::removeRotation(int index){
    if (index >= 0 && index < rotations.size()){
        rotations.removeAt(index);
        //parent->animationMotionDataLines--;
        return true;
    }
    return false;
}

void SkyrimAnimationMotionData::setDuration(const qreal &value){
    duration = value;
}

void SkyrimAnimationMotionData::setAnimationIndex(const uint &value){
    animationIndex = value;
}

uint SkyrimAnimationMotionData::lineCount() const{
    return 4 + translations.size() + rotations.size() + 1;
}
