#include "skyrimanimdata.h"
#include "src/utility.h"
#include "projectanimdata.h"
#include <QTextStream>

using namespace UI;

#define MIN_NUM_LINES 3

SkyrimAnimData::~SkyrimAnimData()
{
    for (auto i = 0; i < animData.size(); i++){
        if (animData.at(i)){
            delete animData.at(i);
        }
    }
}

bool SkyrimAnimData::parse(QFile *file, const QString & projectToIgnore, const QStringList & behaviorfilenames){
    if (!file || (!file->isOpen() && !file->open(QIODevice::ReadOnly | QIODevice::Text))){
        return false;
    }
    QByteArray line;
    auto ok = false;
    auto size = 0;
    int indexToIgnore;
    int blocksize;
    QString name = projectToIgnore;
    //Get project names...
    if (file->fileName().contains("animationdatasinglefile.txt")){
        if (!file->atEnd()){
            line = file->readLine();
            line.chop(1);
            size = line.toUInt(&ok);
            if (ok){
                for (auto i = 0; i < size; i++){
                    if (!file->atEnd()){
                        line = file->readLine();
                        line.chop(1);
                        if (line.contains(".txt")){
                            projectNames.append(line);
                        }else{
                            LogFile::writeToLog("SkyrimAnimData::parse(): Corrupted project filename does not have 'txt' extension!");
                            return false;
                        }
                    }else{
                        LogFile::writeToLog("SkyrimAnimData::parse(): Unexpected EOF!");
                        return false;
                    }
                }
            }else{
                LogFile::writeToLog("SkyrimAnimData::parse(): Corrupted length of current block!");
                return false;
            }
        }else{
            LogFile::writeToLog("SkyrimAnimData::parse(): Unexpected EOF!");
            return false;
        }
    }
    if (projectToIgnore == ""){
        indexToIgnore = projectNames.size();
    }else{
        if (name.contains(".hkx")){
            name.replace(".hkx", ".txt");
        }
        if (!name.contains(".txt")){
            name.append(".txt");
        }
        for (auto i = 0; i < projectNames.size(); i++){
            if (!QString::compare(projectNames.at(i), name, Qt::CaseInsensitive)){
                indexToIgnore = i;
                break;
            }
        }
        if (indexToIgnore == -1){
            indexToIgnore = projectNames.size();
        }
    }
    for (auto i = 0; i < size && i < indexToIgnore; i++){
        animData.append(new ProjectAnimData());
        if (!animData.last()->read(file)){
            LogFile::writeToLog("SkyrimAnimData::parse(): ProjectAnimData read failed!");
            return false;
        }
    }
    if (indexToIgnore != projectNames.size()){
        indexToIgnore++;
        line = file->readLine();
        line.chop(1);
        blocksize = line.toUInt(&ok);
        if (!ok || file->atEnd()){
            return false;
        }
        animData.append(new ProjectAnimData());
        line = file->readLine();
        line.chop(1);
        int numFiles = line.toUInt(&ok);
        if (!ok || numFiles != 1 || file->atEnd()){
            return false;
        }
        line = file->readLine();
        line.chop(1);
        numFiles = line.toUInt(&ok);
        if (!ok || numFiles <= 2 || file->atEnd()){
            return false;
        }
        animData.last()->projectFiles = behaviorfilenames;
        for (auto i = 0; i < numFiles && !file->atEnd(); i++){
            line = file->readLine();
            if ((!line.contains(".hkx") && !line.contains(".HKX"))){
                return false;
            }
            line.chop(1);
            ok = false;
            for (auto k = 0; k < behaviorfilenames.size(); k++){
                if (!QString::compare(behaviorfilenames.at(k), line, Qt::CaseInsensitive)){
                    ok = true;
                }
            }
            if (!ok){
                animData.last()->projectFiles.append(line);
            }
        }
        animData.last()->animationDataLines = numFiles + MIN_NUM_LINES;
        for (auto i = numFiles + MIN_NUM_LINES - 1; i < blocksize && !file->atEnd(); i++){
            line = file->readLine();
        }
        if (line != "0\n"){
            if (!animData.last()->readMotionOnly(file)){
                LogFile::writeToLog("SkyrimAnimData::parse(): readMotionOnly read failed!");
                return false;
            }
        }
        for (auto i = indexToIgnore; i < size; i++){
            animData.append(new ProjectAnimData());
            if (!animData.last()->read(file)){
                LogFile::writeToLog("SkyrimAnimData::parse(): ProjectAnimData read failed!");
                return false;
            }
        }
    }
    return true;
}

bool SkyrimAnimData::write(const QString &filename){
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return false;
    }
    QTextStream out(&file);
    out << QString::number(projectNames.size()) << "\n";
    for (auto i = 0; i < projectNames.size(); i++){
        out << projectNames.at(i) << "\n";
    }
    for (auto i = 0; i < animData.size(); i++){
        if (!animData.at(i)->write(file, out)){
            return false;
        }
    }
    return true;
}

int SkyrimAnimData::getProjectIndex(const QString &projectname) const{
    QString name = projectname;
    if (name.contains(".hkx")){
        name.replace(".hkx", ".txt");
    }
    if (!name.contains(".txt")){
        name.append(".txt");
    }
    for (auto i = 0; i < projectNames.size(); i++){
        if (!name.compare(projectNames.at(i), Qt::CaseInsensitive)){
            return i;
        }
    }
    return -1;
}

ProjectAnimData *SkyrimAnimData::getProjectAnimData(const QString &projectname) const{
    auto index = getProjectIndex(projectname);
    if (index > -1 && index < animData.size()){
        return animData.at(index);
    }
    return nullptr;
}

int SkyrimAnimData::addNewProject(const QString &projectname, const QStringList &projectfilerelativepaths){
    for (auto i = 0; i < projectNames.size(); i++){
        if (!QString::compare(projectNames.at(i), projectname, Qt::CaseInsensitive)){
            return -1;
        }
    }
    projectNames.append(projectname);
    animData.append(new ProjectAnimData());
    for (auto i = 0; i < projectfilerelativepaths.size(); i++){
        animData.last()->projectFiles.append(projectfilerelativepaths.at(i));
        animData.last()->animationDataLines++;
    }
    return animData.size() - 1;
}

bool SkyrimAnimData::appendClipGenerator(const QString & projectname, SkyrimClipGeneratoData * animdata){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::appendClipGenerator(): The project was not found!");
        return false;
    }
    return animData.at(index)->appendClipGenerator(animdata);
}

bool SkyrimAnimData::removeClipGenerator(const QString &projectname, const QString &name){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::removeClipGenerator(): The project was not found!");
        return false;
    }
    return animData.at(index)->removeClipGenerator(name);
}

bool SkyrimAnimData::appendAnimation(const QString &projectname, SkyrimAnimationMotionData * motiondata){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::appendAnimation(): The project was not found!");
        return false;
    }
    return animData.at(index)->appendAnimation(motiondata);
}

bool SkyrimAnimData::removeAnimation(const QString &projectname, int animationindex){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::removeAnimation(): The project was not found!");
        return false;
    }
    return animData.at(index)->removeAnimation(animationindex);
}

void SkyrimAnimData::setLocalTimeForClipGenAnimData(const QString &projectname, const QString &clipname, int triggerindex, qreal time){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::setLocalTimeForClipGenAnimData(): The project was not found!");
        return;
    }
    animData.at(index)->setLocalTimeForClipGenAnimData(clipname, triggerindex, time);
}

void SkyrimAnimData::setEventNameForClipGenAnimData(const QString &projectname, const QString &clipname, int triggerindex, const QString &eventname){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::setEventNameForClipGenAnimData(): The project was not found!");
        return;
    }
    animData.at(index)->setEventNameForClipGenAnimData(clipname, triggerindex, eventname);
}

void SkyrimAnimData::setClipNameAnimData(const QString &projectname, const QString &oldclipname, const QString &newclipname){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::setClipNameAnimData(): The project was not found!");
        return;
    }
    animData.at(index)->setClipNameAnimData(oldclipname, newclipname);
}


void SkyrimAnimData::setAnimationIndexForClipGen(const QString &projectname, const QString &clipGenName, int animationindex){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::setAnimationIndexForClipGen(): The project was not found!");
        return;
    }
    animData.at(index)->setAnimationIndexForClipGen(clipGenName, animationindex);
}

void SkyrimAnimData::setPlaybackSpeedAnimData(const QString &projectname, const QString &clipGenName, qreal speed){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::setPlaybackSpeedAnimData(): Project was not found!");
        return;
    }
    animData.at(index)->setPlaybackSpeedForClipGen(clipGenName, speed);
}

void SkyrimAnimData::setCropStartAmountLocalTimeAnimData(const QString &projectname, const QString &clipGenName, qreal time){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::setCropStartAmountLocalTimeAnimData(): Project was not found!");
        return;
    }
    animData.at(index)->setCropStartAmountLocalTimeForClipGen(clipGenName, time);
}

void SkyrimAnimData::setCropEndAmountLocalTimeAnimData(const QString &projectname, const QString &clipGenName, qreal time){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::setCropEndAmountLocalTimeAnimData(): Project was not found!");
        return;
    }
    animData.at(index)->setCropEndAmountLocalTimeForClipGen(clipGenName, time);
}

void SkyrimAnimData::appendClipTriggerToAnimData(const QString &projectname, const QString &clipGenName, const QString & eventname){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::appendClipTriggerToAnimData(): Project was not found!");
        return;
    }
    animData.at(index)->appendClipTriggerToAnimData(clipGenName, eventname);
}

void SkyrimAnimData::removeClipTriggerToAnimDataAt(const QString &projectname, const QString &clipGenName, int index){
    int indexi = getProjectIndex(projectname);
    if (indexi < 0 || indexi >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::removeClipTriggerToAnimDataAt(): Project was not found!");
        return;
    }
    animData.at(indexi)->removeClipTriggerToAnimDataAt(clipGenName, index);
}

bool SkyrimAnimData::isProjectNameTaken(const QString & name) const{
    for (auto i = 0; i < projectNames.size(); i++){
        if (projectNames.at(i) == name){
            return true;
        }
    }
    return false;
}

bool SkyrimAnimData::removeBehaviorFromProject(const QString &projectname, const QString &behaviorfilename){
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::removeBehaviorFromProject(): Project was not found!");
        return false;
    }
    animData.at(index)->removeBehaviorFromProject(behaviorfilename);
    return true;
}

qreal SkyrimAnimData::getAnimationDurationFromAnimData(const QString &projectname, int animationindex) const{
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::getAnimationDurationFromAnimData(): Project was not found!");
        return 0;
    }
    return animData.at(index)->getAnimationDuration(animationindex);
}

SkyrimAnimationMotionData SkyrimAnimData::getAnimationMotionData(const QString &projectname, int animationindex) const{
    auto index = getProjectIndex(projectname);
    if (index < 0 || index >= animData.size()){
        LogFile::writeToLog("SkyrimAnimData::getAnimationMotionData(): Project was not found!");
        return SkyrimAnimationMotionData(nullptr);
    }
    return animData.at(index)->getAnimationMotionData(animationindex);
}

void SkyrimAnimData::merge(SkyrimAnimData *recessiveanimdata, const QString & projecttomerge){
    int domindex;
    int recindex;
    if (recessiveanimdata){
        if (projecttomerge == ""){
            for (auto i = 0; i < recessiveanimdata->projectNames.size(); i++){
                if (!projectNames.contains(recessiveanimdata->projectNames.at(i), Qt::CaseInsensitive)){
                    projectNames.append(recessiveanimdata->projectNames.at(i));
                    if (recessiveanimdata->animData.size() < i){
                        animData.append(recessiveanimdata->animData.at(i));
                    }else{
                        LogFile::writeToLog("SkyrimAnimData::merge(): recessiveanimdata is corrupt! Mismatch between project names and animdata!");
                    }
                }
            }
        }else{
            domindex = projectNames.indexOf(projecttomerge, Qt::CaseInsensitive);
            recindex = recessiveanimdata->projectNames.indexOf(projecttomerge, Qt::CaseInsensitive);
            if (domindex > -1 && recindex > -1){
                if (domindex < animData.size() && recindex < recessiveanimdata->animData.size()){
                    animData.at(domindex)->merge(recessiveanimdata->animData.at(recindex));
                }
            }else{
                LogFile::writeToLog("SkyrimAnimData::merge(): Project does not exist in either the dominant or recessive objects!");
            }
        }
    }else{
        LogFile::writeToLog("SkyrimAnimData::merge(): recessiveanimdata is nullptr!");
    }
}

bool SkyrimAnimData::isEmpty() const{
    if (projectNames.isEmpty()){
        return true;
    }
    return false;
}

