#ifndef ANIMATIONFILE_H
#define ANIMATIONFILE_H

#include "src/filetypes/hkxfile.h"


class AnimationFile final: public HkxFile
{
public:
    AnimationFile(MainWindow *window, const QString & name);
    AnimationFile& operator=(const AnimationFile&) = delete;
    AnimationFile(const AnimationFile &) = delete;
    ~AnimationFile() = default;
public:
    qreal getDuration() const;
    bool parse();
protected:
    bool addObjectToFile(UI::HkxObject *, long);
private:
    qreal duration;
    //QVector <SkyrimAnimationTranslation> translations;
    //QVector <SkyrimAnimationRotation> rotations;
    mutable std::mutex mutex;
};

#endif // ANIMATIONFILE_H
