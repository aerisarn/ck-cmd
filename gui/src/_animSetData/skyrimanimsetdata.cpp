#include "skyrimanimsetdata.h"
#include "src/utility.h"
#include <QTextStream>

using namespace UI;

SkyrimAnimSetData::~SkyrimAnimSetData(){
    for (auto i = 0; i < projects.size(); i++){
        if (projects.at(i)){
            delete projects.at(i);
        }
    }
}

bool SkyrimAnimSetData::parse(QFile *file){
    if (!file || (!file->isOpen() && !file->open(QIODevice::ReadOnly | QIODevice::Text))){
        return false;
    }
    QByteArray line;
    auto ok = false;
    auto size = 0U;
    while (!file->atEnd()){ //Get project names...
        line = file->readLine();
        line.chop(1);
        size = line.toUInt(&ok);
        if (ok){
            for (auto i = 0U; i < size; i++){
                if (!file->atEnd()){
                    line = file->readLine();
                    line.chop(1);
                    if (line.contains(".txt")){
                        projectNames.append(line);
                    }else{
                        LogFile::writeToLog("SkyrimAnimSetData::parse(): Corrupted project filename does not have 'txt' extension!");
                        return false;
                    }
                }else{
                    LogFile::writeToLog("SkyrimAnimSetData::parse(): Unexpected EOF!");
                    return false;
                }
            }
            for (auto i = 0; i < projectNames.size(); i++){
                projects.append(new AnimCacheProjectData());
                if (!projects.last()->read(file)){
                    LogFile::writeToLog("SkyrimAnimSetData::parse(): ProjectAnimSetData read failed!");
                    return false;
                }
            }
        }else{
            LogFile::writeToLog("SkyrimAnimSetData::parse(): Corrupted length of current block!");
            return false;
        }
    }
    return true;
}

bool SkyrimAnimSetData::write(const QString &filename){
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return false;
    }
    QTextStream out(&file);
    out << QString::number(projectNames.size()) << "\n";
    for (auto i = 0; i < projectNames.size(); i++){
        out << projectNames.at(i) << "\n";
    }
    for (auto i = 0; i < projects.size(); i++){
        projects.at(i)->write(&file, out);
    }
    return true;
}

bool SkyrimAnimSetData::addNewProject(const QString &projectname){
    for (auto i = 0; i < projectNames.size(); i++){
        if (!QString::compare(projectNames.at(i), projectname, Qt::CaseInsensitive)){
            return false;
        }
    }
    projectNames.append(projectname);
    QVector <AnimCacheAnimSetData *> vec;
    vec.append(new AnimCacheAnimSetData());
    projects.append(new AnimCacheProjectData(QStringList("FullBody.txt"), vec));
    return true;
}

bool SkyrimAnimSetData::addAnimationToCache(const QString &projectname, const QString & eventname, const QVector<AnimCacheAnimationInfo *> &animations, const QVector<AnimCacheVariable *> &vars, const QVector<AnimCacheClipInfo *> &clips){
    auto count = 0;
    auto index = projectNames.indexOf(projectname);
    if (index < 0 || index >= projects.size()){
        return false;
    }
    for (auto i = 0; i < projects.at(index)->animSetData.size(); i++){
        if (projects.at(index)->animSetData.at(i)->cacheEvents.contains(eventname)){
            count++;
            if (!projects[index]->animSetData[i]->addAnimationToCache(eventname, animations, vars, clips)){
                return false;
            }
        }
    }
    /*if (!count){
        createNewCacheBlock(event, animations, vars, clips);
    }*/
    return true;
}

bool SkyrimAnimSetData::removeAnimationFromCache(const QString & projectname, const QString &animationname, const QString &variablename, const QString &clipname){
    auto index = projectNames.indexOf(projectname);
    if (index < 0 || index >= projects.size()){
        return false;
    }
    for (auto i = 0; i < projects.at(index)->animSetData.size(); i++){
        projects[index]->animSetData[i]->removeAnimationFromCache(animationname, clipname, variablename);
    }
    return true;
}

AnimCacheProjectData *SkyrimAnimSetData::getProjectCacheData(const QString & name){
    for (auto i = 0; i < projectNames.size(); i++){
        if (projectNames.at(i).contains(name, Qt::CaseInsensitive)){
            return projects.at(i);
        }
    }
    LogFile::writeToLog("SkyrimAnimSetData::getProjectCacheData(): getProjectCacheData() failed!");
    return nullptr;
}

bool SkyrimAnimSetData::mergeAnimationCaches(const QString &projectname, const QString &recessiveprojectname, SkyrimAnimSetData *recessiveprojectdata){
    AnimCacheProjectData *thisdata = nullptr;
    AnimCacheProjectData *recessivedata = nullptr;
    if (recessiveprojectdata){
        for (auto i = 0; i < projectNames.size(); i++){
            if (projectNames.at(i).contains(projectname, Qt::CaseInsensitive)){
                thisdata = projects.at(i);
            }
        }
        for (auto i = 0; i < recessiveprojectdata->projectNames.size(); i++){
            if (recessiveprojectdata->projectNames.at(i).contains(recessiveprojectname, Qt::CaseInsensitive)){
                recessivedata = recessiveprojectdata->projects.at(i);
            }
        }
        if (thisdata && recessivedata && thisdata->merge(recessivedata)){
            return true;
        }
    }
    return false;
}

bool SkyrimAnimSetData::isEmpty() const{
    if (projectNames.isEmpty()){
        return true;
    }
    return false;
}
