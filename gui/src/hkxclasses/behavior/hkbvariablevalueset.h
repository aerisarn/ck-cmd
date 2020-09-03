#ifndef HKBVARIABLEVALUESET_H
#define HKBVARIABLEVALUESET_H

#include <QVector>

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbVariableValueSet final: public HkxObject
{
    friend class hkbBehaviorGraphData;
    friend class hkbCharacterData;
    friend class BehaviorVariablesUI;
    friend class CharacterPropertiesUI;
public:
    hkbVariableValueSet(HkxFile *parent, long ref = 0);
    hkbVariableValueSet& operator=(const hkbVariableValueSet&) = delete;
    hkbVariableValueSet(const hkbVariableValueSet &) = delete;
    ~hkbVariableValueSet();
public:
    static const QString getClassname();
private:
    int getWordVariableAt(int index) const;
    HkxObject * getVariantVariableValueAt(int index);
    hkQuadVariable getQuadVariableValueAt(int index, bool *ok = nullptr);
    void setQuadVariableValueAt(int index, const hkQuadVariable & value);
    void setWordVariableAt(int index, int value);
    void removeWordVariableValueAt(int index);
    void removeQuadVariableValueAt(int index);
    void removeVariantVariableValueAt(int index);
    void addQuadVariableValue(const hkQuadVariable & value);
    void addWordVariableValue(int value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool merge(HkxObject *recessiveobj);
private:
    static uint refCount;
    static const QString classname;
    QVector <int> wordVariableValues;
    QVector <hkQuadVariable> quadVariableValues;
    QVector <HkxSharedPtr> variantVariableValues;
    mutable std::mutex mutex;
};
}
#endif // HKBVARIABLEVALUESET_H
