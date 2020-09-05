#ifndef SKELETONFILE_H
#define SKELETONFILE_H

#include "src/filetypes/hkxfile.h"

namespace UI {
	class hkaSkeleton;
}

class SkeletonFile final: public HkxFile
{
    friend class UI::hkaSkeleton;
public:
    SkeletonFile(MainWindow *window, const QString & name);
    SkeletonFile& operator=(const SkeletonFile&) = delete;
    SkeletonFile(const SkeletonFile &) = delete;
    ~SkeletonFile() = default;
public:
    bool addObjectToFile(UI::HkxObject *obj, long ref = -1);
    void write();
    QString getRootObjectReferenceString();
    QStringList getBonesFromSkeletonAt(int index) const;
    int getNumberOfBones(bool ragdoll = false) const;
    QStringList getLocalFrameNames() const;
    UI::hkaSkeleton *getSkeleton(bool isragdoll) const;
    bool parse();
	virtual bool parseBinary();
	UI::HkxSharedPtr * findSkeleton(long ref);
protected:
    bool link();
private:
	UI::HkxSharedPtr * findLocalFrame(long ref);
private:
    QVector <UI::HkxSharedPtr> skeletons;
    QVector <UI::HkxSharedPtr> localFrames;
    long largestRef;
    mutable std::mutex mutex;
};

#endif // SKELETONFILE_H
