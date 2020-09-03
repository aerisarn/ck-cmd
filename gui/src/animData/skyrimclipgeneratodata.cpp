#include "skyrimclipgeneratodata.h"
#include "projectanimdata.h"
#include "src/utility.h"

using namespace UI;

SkyrimClipGeneratoData::SkyrimClipGeneratoData(ProjectAnimData *par, const QString & name, uint ind, qreal speed, qreal startcrop, qreal endcrop, const QVector <SkyrimClipTrigger> & trigs)
    : parent(par), clipGeneratorName(name), animationIndex(ind), playbackSpeed(speed), cropStartTime(startcrop), cropEndTime(endcrop), triggers(trigs)
{
    //
}

void SkyrimClipGeneratoData::addTrigger(const SkyrimClipTrigger & trig){
    triggers.append(trig);
    //parent->animationDataLines++;
}

bool SkyrimClipGeneratoData::removeTrigger(int index){
    if (index >= 0 && index < triggers.size()){
        triggers.removeAt(index);
        //parent->animationDataLines--;
        return true;
    }
    return false;
}

QString SkyrimClipGeneratoData::getClipGeneratorName() const{
    return clipGeneratorName;
}

void SkyrimClipGeneratoData::rearrangeTriggers(){
    qreal temp;
    for (auto i = triggers.size() - 1; i > 0; i--){
        if (triggers.at(i).time < triggers.at(i - 1).time){
            temp = triggers.at(i).time;
            triggers[i].time = triggers.at(i - 1).time;
            triggers[i - 1].time = temp;
            for (auto j = i + 1; j < triggers.size(); j++){
                if (triggers.at(j).time < triggers.at(j - 1).time){
                    temp = triggers.at(j).time;
                    triggers[j].time = triggers.at(j - 1).time;
                    triggers[j - 1].time = temp;
                }
            }
        }
    }
}

bool SkyrimClipGeneratoData::read(QFile *file, ulong &lineCount){
    if (!file || !file->isOpen()){
        return false;
    }
    QByteArray line;
    QList <QByteArray> list;
    auto ok = false;
    auto index = 0U;
    auto size = 0U;
    auto value = 0.0;
    if (!chopLine(file, line, lineCount)){
        return false;
    }
    clipGeneratorName = line;
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
    value = line.toDouble(&ok);
    if (!ok){
        return false;
    }
    playbackSpeed = value;
    if (!chopLine(file, line, lineCount)){
        return false;
    }
    value = line.toDouble(&ok);
    if (!ok){
        return false;
    }
    cropStartTime = value;
    if (!chopLine(file, line, lineCount)){
        return false;
    }
    value = line.toDouble(&ok);
    if (!ok){
        return false;
    }
    cropEndTime = value;
    if (!chopLine(file, line, lineCount)){
        return false;
    }
    size = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    for (uint i = 0; i < size; i++){
        if (!chopLine(file, line, lineCount)){
            return false;
        }
        list = line.split(':');
        if (list.size() != 2){
            return false;
        }
        triggers.append(SkyrimClipTrigger());
        triggers.last().name = list.first();
        triggers.last().time = list[1].toDouble(&ok);
        if (!ok){
            return false;
        }
    }
    if (!chopLine(file, line, lineCount) || line != ""){
        return false;
    }
    return true;
}

bool SkyrimClipGeneratoData::write(QFile *file, QTextStream & out){
    if (!file || !file->isOpen()){
        return false;
    }
    out << clipGeneratorName << "\n";
    out << QString::number(animationIndex) << "\n";
    out << trimFloat(QString::number(playbackSpeed, char('f'), 6)) << "\n";
    out << trimFloat(QString::number(cropStartTime, char('f'), 6)) << "\n";
    out << trimFloat(QString::number(cropEndTime, char('f'), 6)) << "\n";
    out << QString::number(triggers.size()) << "\n";
    rearrangeTriggers();
    for (auto i = 0; i < triggers.size(); i++){
        out << triggers.at(i).name+":"+trimFloat(QString::number(triggers.at(i).time, char('f'), 6)) << "\n";
    }
    out << "\n";
    return true;
}

uint SkyrimClipGeneratoData::lineCount() const{
    return 6 + triggers.size() + 1;
}
