#include "animcacheanimsetdata.h"
#include "hkcrc.h"
#include "src/utility.h"

AnimCacheAnimSetData::AnimCacheAnimSetData(const QStringList & events, const QVector <AnimCacheVariable *> & vars, const QVector <AnimCacheClipInfo *> & clips, const QVector <AnimCacheAnimationInfo *> & anims)
    : cacheEvents(events), behaviorVariables(vars), clipGenerators(clips), animations(anims)
{
    //
}

bool AnimCacheAnimSetData::read(QFile *file){
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
    if (line != "V3" || file->atEnd()){
        return false;
    }
    //Read cache events...
    line = file->readLine();
    line.chop(1);
    auto numevents = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    for (auto i = 0U; i < numevents; i++){
        if (file->atEnd()){
            return false;
        }
        line = file->readLine();
        line.chop(1);
        cacheEvents.append(line);
    }
    line = file->readLine();
    line.chop(1);
    auto numvars = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    //Get attack animation data, if any...
    for (auto i = 0U; i < numvars; i++){
        behaviorVariables.append(new AnimCacheVariable());
        if (!behaviorVariables.last()->read(file)){
            return false;
        }
    }
    if (file->atEnd()){
        return false;
    }
    line = file->readLine();
    line.chop(1);
    auto numclips = line.toUInt(&ok);
    if (!ok){
        return false;
    }
    for (auto i = 0U; i < numclips; i++){
        clipGenerators.append(new AnimCacheClipInfo());
        if (!clipGenerators.last()->read(file)){
            return false;
        }
    }
    //Read encrypted animation names, paths...
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
        animations.append(new AnimCacheAnimationInfo());
        if (!animations.last()->read(file)){
            return false;
        }
    }
    return true;
}

bool AnimCacheAnimSetData::write(QFile *file, QTextStream & out) const{
    if (!file || !file->isOpen()){
        return false;
    }
    out << "V3\n";
    out << QString::number(cacheEvents.size()) << "\n";
    for (auto i = 0; i < cacheEvents.size(); i++){
        out << cacheEvents.at(i) << "\n";
    }
    out << QString::number(behaviorVariables.size()) << "\n";
    for (auto i = 0; i < behaviorVariables.size(); i++){
        behaviorVariables.at(i)->write(file, out);
    }
    out << QString::number(clipGenerators.size()) << "\n";
    for (auto i = 0; i < clipGenerators.size(); i++){
        clipGenerators.at(i)->write(file, out);
    }
    out << QString::number(animations.size()) << "\n";
    for (auto i = 0; i < animations.size(); i++){
        animations.at(i)->write(file, out);
    }
    return true;
}

bool AnimCacheAnimSetData::addAnimationToCache(const QString & event, const QVector<AnimCacheAnimationInfo *> &anims, const QVector<AnimCacheVariable *> &vars, const QVector <AnimCacheClipInfo *> & clips){
    bool exists;
    if (!cacheEvents.contains(event)){
        cacheEvents.append(event);
    }
    for (auto i = 0; i < anims.size(); i++){
        exists = false;
        for (auto j = 0; j < animations.size(); j++){
            if (animations.at(j) == anims.at(i)){
                exists = true;
            }
        }
        if (!exists){
            animations.append(anims.at(i));
        }
    }
    for (auto i = 0; i < vars.size(); i++){
        exists = false;
        for (auto j = 0; j < behaviorVariables.size(); j++){
            if (behaviorVariables.at(j) == vars.at(i)){
                exists = true;
            }
        }
        if (!exists){
            behaviorVariables.append(vars.at(i));
        }
    }
    for (auto i = 0; i < clips.size(); i++){
        exists = false;
        for (auto j = 0; j < clipGenerators.size(); j++){
            if (clipGenerators.at(j) == clips.at(i)){
                exists = true;
            }
        }
        if (!exists){
            clipGenerators.append(clips.at(i));
        }
    }
    return true;
}


void AnimCacheAnimSetData::removeAnimationFromCache(const QString &animationname, const QString &clipname,  const QString &variablename){
    if (animationname != ""){
        auto temp = animationname;
        if (temp.contains("/")){
            temp = temp.section("/", -1, -1);
        }else if (temp.contains("\\")){
            temp = temp.section("\\", -1, -1);
        }
        if (temp.contains(".")){
            int index = temp.indexOf(".");
            temp.remove(index, temp.size() - index);
        }
        bool ok;
        auto animationhash = QString(HkCRC().compute(temp.toLocal8Bit().toLower()));
        animationhash = QString::number(animationhash.toLong(&ok, 16));
        if (!ok){
            CRITICAL_ERROR_MESSAGE("AnimCacheAnimSetData::removeAnimationFromCache(): animation hash is invalid!!!");
        }
        for (auto i = animations.size() - 1; i >= 0; i--){
            if (animations.at(i)->getCrcAnimationName() == animationhash){
                animations.removeAt(i);
            }
        }
    }
    if (variablename != ""){
        for (auto i = behaviorVariables.size() - 1; i >= 0; i--){
            if (behaviorVariables.at(i)->getName() == variablename){
                behaviorVariables.removeAt(i);
            }
        }
    }
    if (clipname != ""){
        for (auto i = clipGenerators.size() - 1; i >= 0; i--){
            clipGenerators[i]->removeClipGenerator(clipname);
        }
    }
}

bool AnimCacheAnimSetData::merge(AnimCacheAnimSetData *recessiveproject){
    if (recessiveproject){
        for (auto i = 0; i < recessiveproject->cacheEvents.size(); i++){
            if (!cacheEvents.contains(recessiveproject->cacheEvents.at(i))){
                cacheEvents.append(recessiveproject->cacheEvents.at(i));
            }
        }
        for (auto i = 0; i < recessiveproject->behaviorVariables.size(); i++){
            if (!behaviorVariables.contains(recessiveproject->behaviorVariables.at(i))){
                behaviorVariables.append(recessiveproject->behaviorVariables.at(i));
            }
        }
        for (auto i = 0; i < recessiveproject->clipGenerators.size(); i++){
            if (!clipGenerators.contains(recessiveproject->clipGenerators.at(i))){
                clipGenerators.append(recessiveproject->clipGenerators.at(i));
            }
        }
        for (auto i = 0; i < recessiveproject->animations.size(); i++){
            if (!animations.contains(recessiveproject->animations.at(i))){
                animations.append(recessiveproject->animations.at(i));
            }
        }
    }
    return true;
}
