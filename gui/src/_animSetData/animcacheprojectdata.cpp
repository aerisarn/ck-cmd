#include "animcacheprojectdata.h"
#include <QStringList>

AnimCacheProjectData::AnimCacheProjectData(const QStringList & files, const QVector<AnimCacheAnimSetData *> &animdata)
    : fileNames(files), animSetData(animdata)
{
    //
}

AnimCacheProjectData::~AnimCacheProjectData(){
    for (auto i = 0; i < animSetData.size(); i++){
        if (animSetData.at(i)){
            delete animSetData.at(i);
        }
    }
}

bool AnimCacheProjectData::read(QFile *file){
    if (!file || !file->isOpen()){
        return false;
    }
    QByteArray line;
    auto ok = false;
    auto numFiles = 0U;
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    numFiles = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    for (auto i = 0U; i < numFiles; i++){
        if (file->atEnd()){
            return false;
        }
        line = file->readLine();
        line.chop(1);
        if (!line.contains(".txt")){
            return false;
        }
        fileNames.append(line);
    }
    //Read individual project's animsetdata...
    for (auto i = 0U; i < numFiles; i++){
        animSetData.append(new AnimCacheAnimSetData());
        if (!animSetData.last()->read(file)){
            return false;
        }
    }
    return true;
}

bool AnimCacheProjectData::write(QFile *file, QTextStream & out) const{
    if (!file || !file->isOpen()){
        return false;
    }
    out << QString::number(fileNames.size()) << "\n";
    for (auto i = 0; i < fileNames.size(); i++){
        out << fileNames.at(i) << "\n";
    }
    for (auto i = 0; i < animSetData.size(); i++){
        if (!animSetData.at(i)->write(file, out)){
            return false;
        }
    }
    return true;
}

bool AnimCacheProjectData::merge(AnimCacheProjectData *recessiveproject){
    auto size = fileNames.size();
    bool found;
    if (recessiveproject){
        for (auto i = 0; i < size; i++){
            found = false;
            for (auto j = 0; j < recessiveproject->fileNames.size(); j++){
                if (!QString::compare(fileNames.at(i), recessiveproject->fileNames.at(j), Qt::CaseInsensitive)){
                    if (i >= animSetData.size() && j >= recessiveproject->animSetData.size() && !animSetData.at(i)->merge(recessiveproject->animSetData.at(j))){
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
