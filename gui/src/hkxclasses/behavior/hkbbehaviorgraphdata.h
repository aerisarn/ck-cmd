#ifndef HKBBEHAVIORGRAPHDATA_H
#define HKBBEHAVIORGRAPHDATA_H

#include <QVector>

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbBehaviorGraphStringData;
class hkbVariableValueSet;

class hkbBehaviorGraphData final: public HkxObject
{
    friend class BehaviorVariablesUI;
    friend class EventsUI;
public:
    hkbBehaviorGraphData(HkxFile *parent, long ref = 0, hkbBehaviorGraphStringData *strings = nullptr, hkbVariableValueSet *values = nullptr);
    hkbBehaviorGraphData& operator=(const hkbBehaviorGraphData&) = delete;
    hkbBehaviorGraphData(const hkbBehaviorGraphData &) = delete;
    ~hkbBehaviorGraphData();
public:
    int addVariable(hkVariableType type, const QString & name, bool isProperty = false);
    void addVariable(hkVariableType type);
    void addEvent(const QString &name);
    HkxObject * getVariantVariable(int index) const;
    static const QString getClassname();
    QStringList getVariableNames() const;
    QStringList getEventNames() const;
    int getNumberOfEvents() const;
    int getNumberOfVariables() const;
    QStringList getVariableTypeNames() const;
    int getIndexOfVariable(const QString & name) const;
    int getIndexOfEvent(const QString & name) const;
    hkQuadVariable getQuadVariableValueAt(int index, bool *ok = nullptr) const;
    hkVariableType getCharacterPropertyTypeAt(int index) const;
    QString getCharacterPropertyNameAt(int index) const;
    hkVariableType getVariableTypeAt(int index) const;
    QString getVariableNameAt(int index) const;
    QString getEventNameAt(int index) const;
    QString getEventInfoAt(int index) const;
    int getWordVariableValueAt(int index) const;
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool merge(HkxObject *recessiveobj);
    void removeVariable(int index);
    void setVariableNameAt(int index, const QString & name);
    void setWordVariableValueAt(int index, int value);
    void setQuadVariableValueAt(int index, hkQuadVariable value);
    void addEvent();
    void removeEvent(int index);
    void setEventNameAt(int index, const QString & name);
    void setEventFlagAt(int index, bool state);
private:
    struct hkVariableInfo
    {
        hkVariableInfo(){}
        struct hkRole
        {
            hkRole(): role("ROLE_DEFAULT"), flags("0"){}
            QString role;
            QString flags;
        };

        hkRole role;
        QString type;    //See hkVariableType...
    };
private:
    static uint refCount;
    static const QStringList Type;
    static const QString classname;
    //QVector <??> attributeDefaults;
    QVector <hkVariableInfo> variableInfos;
    QVector <hkVariableInfo> characterPropertyInfos;
    QStringList eventInfos;
    QVector <int> wordMinVariableValues;
    QVector <int> wordMaxVariableValues;
    HkxSharedPtr variableInitialValues;
    HkxSharedPtr stringData;
    mutable std::mutex mutex;
};
}
#endif // HKBBEHAVIORGRAPHDATA_H
