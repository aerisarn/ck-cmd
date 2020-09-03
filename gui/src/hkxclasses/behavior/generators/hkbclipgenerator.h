#ifndef HKBCLIPGENERATOR_H
#define HKBCLIPGENERATOR_H

#include "hkbgenerator.h"
#include "src/animData/skyrimclipgeneratodata.h"
namespace UI {
class hkbClipTriggerArray;

class hkbClipGenerator final: public hkbGenerator
{
    friend class ClipGeneratorUI;
public:
    hkbClipGenerator(HkxFile *parent, long ref = 0, bool addToAnimData = false, const QString & animationname = "");
    hkbClipGenerator& operator=(const hkbClipGenerator&) = delete;
    hkbClipGenerator(const hkbClipGenerator &) = delete;
    ~hkbClipGenerator();
public:
    QString getName() const;
    static const QString getClassname();
    SkyrimClipGeneratoData getClipGeneratorAnimData(ProjectAnimData *parent, uint animationIndex) const;
    QString getAnimationName() const;
    enum ClipFlag{
        FLAG_NONE = 0,
        FLAG_CONTINUE_MOTION_AT_END = 1,
        FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE = 2,
        FLAG_MIRROR = 4,
        FLAG_FORCE_DENSE_POSE = 8,
        FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS = 16,
        FLAG_IGNORE_MOTION = 32,
        INVALID_FLAG = 128
    };
    Q_DECLARE_FLAGS(ClipFlags, ClipFlag)
private:
    hkbClipTriggerArray* getTriggers() const;
    qreal getCropStartAmountLocalTime() const;
    qreal getCropEndAmountLocalTime() const;
    qreal getStartTime() const;
    void setStartTime(const qreal &value);
    qreal getPlaybackSpeed() const;
    qreal getEnforcedDuration() const;
    void setEnforcedDuration(const qreal &value);
    qreal getUserControlledTimeFraction() const;
    void setUserControlledTimeFraction(const qreal &value);
    int getAnimationBindingIndex() const;
    void setAnimationBindingIndex(int value);
    QString getMode() const;
    void setMode(int index);
    QString getFlags() const;
    void setFlags(const QString &value);
    void setTriggers(hkbClipTriggerArray *value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfTriggers() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    bool merge(HkxObject *recessiveObject);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    void setName(const QString & oldclipname, const QString & newclipname);
    void setAnimationName(int index, const QString & animationname);
    void setPlaybackSpeed(qreal speed);
    void setCropStartAmountLocalTime(qreal time);
    void setCropEndAmountLocalTime(qreal time);
private:
    static uint refCount;
    static const QStringList PlaybackMode;
    static const QString classname;
    ulong userData;
    QString name;
    QString animationName;
    HkxSharedPtr triggers;
    qreal cropStartAmountLocalTime;
    qreal cropEndAmountLocalTime;
    qreal startTime;
    qreal playbackSpeed;
    qreal enforcedDuration;
    qreal userControlledTimeFraction;
    int animationBindingIndex;
    QString mode;
    QString flags;
    mutable std::mutex mutex;
};
}
Q_DECLARE_OPERATORS_FOR_FLAGS(UI::hkbClipGenerator::ClipFlags)

#endif // HKBCLIPGENERATOR_H
