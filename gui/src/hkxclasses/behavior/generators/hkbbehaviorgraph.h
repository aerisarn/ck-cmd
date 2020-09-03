#ifndef HKBBEHAVIORGRAPH_H
#define HKBBEHAVIORGRAPH_H

#include "hkbgenerator.h"
namespace UI {
class hkbStateMachine;
class hkbBehaviorGraphData;

class hkbBehaviorGraph final: public hkbGenerator
{
    friend class BehaviorGraphUI;
public:
    hkbBehaviorGraph(HkxFile *parent, long ref = 0);
    hkbBehaviorGraph& operator=(const hkbBehaviorGraph&) = delete;
    hkbBehaviorGraph(const hkbBehaviorGraph &) = delete;
    ~hkbBehaviorGraph();
public:
    QString getName() const;
    static const QString getClassname();
    void setData(hkbBehaviorGraphData *graphdata);
    hkbStateMachine * getRootGenerator() const;
    void setName(const QString & newname);
    void setVariableMode(const QString &value);
private:
    QString getRootGeneratorName() const;
    QString getVariableMode() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    static const QStringList VariableMode;
    qint64 userData;
    QString name;
    QString variableMode;
    HkxSharedPtr rootGenerator;
    HkxSharedPtr data;
    mutable std::mutex mutex;
};
}
#endif // HKBBEHAVIORGRAPH_H
