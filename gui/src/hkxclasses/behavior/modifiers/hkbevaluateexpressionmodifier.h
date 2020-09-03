#ifndef HKBEVALUATEEXPRESSIONMODIFIER_H
#define HKBEVALUATEEXPRESSIONMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbExpressionDataArray;

class hkbEvaluateExpressionModifier final: public hkbModifier
{
    friend class EvaluateExpressionModifierUI;
public:
    hkbEvaluateExpressionModifier(HkxFile *parent, long ref = 0);
    hkbEvaluateExpressionModifier& operator=(const hkbEvaluateExpressionModifier&) = delete;
    hkbEvaluateExpressionModifier(const hkbEvaluateExpressionModifier &) = delete;
    ~hkbEvaluateExpressionModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkbExpressionDataArray * getExpressions() const;
    void setExpressions(hkbExpressionDataArray *value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfExpressions() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    bool merge(HkxObject *recessiveObject);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    HkxSharedPtr expressions;
    mutable std::mutex mutex;
};
}
#endif // HKBEVALUATEEXPRESSIONMODIFIER_H
