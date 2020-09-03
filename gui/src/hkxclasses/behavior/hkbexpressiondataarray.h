#ifndef HKBEXPRESSIONDATAARRAY_H
#define HKBEXPRESSIONDATAARRAY_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbExpressionDataArray final: public HkxObject
{
    friend class ExpressionDataArrayUI;
    friend class hkbEvaluateExpressionModifier;
    friend class EvaluateExpressionModifierUI;
private:
    enum ExpressionEventMode{
        EVENT_MODE_SEND_ONCE=0,
        EVENT_MODE_SEND_ON_TRUE=1,
        EVENT_MODE_SEND_ON_FALSE_TO_TRUE=2,
        EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE=3
    };
public:
    hkbExpressionDataArray(HkxFile *parent, long ref = -1);
    hkbExpressionDataArray& operator=(const hkbExpressionDataArray&) = delete;
    hkbExpressionDataArray(const hkbExpressionDataArray &) = delete;
    ~hkbExpressionDataArray();
    QString getExpressionAt(int index) const;
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    static QString getClassname();
    void addExpression(const QString & exp = "", ExpressionEventMode mode = EVENT_MODE_SEND_ONCE);
    void removeExpression(const QString & exp);
    void removeExpression(int index);
    bool write(HkxXMLWriter *writer);
    bool isEventReferenced(int eventindex) const;
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    bool merge(HkxObject *recessiveObject);
    void updateEventIndices(int eventindex);
private:
    struct hkExpression
    {
        hkExpression(const QString & exp = "", ExpressionEventMode mode = EVENT_MODE_SEND_ONCE)
            : expression(exp),
              assignmentVariableIndex(-1),
              assignmentEventIndex(-1),
              eventMode(EventMode.at(mode))
        {
            //
        }

        bool operator==(const hkExpression & other) const;

        QString expression;
        int assignmentVariableIndex;
        int assignmentEventIndex;
        QString eventMode;
    };
private:
    static uint refCount;
    static const QString classname;
    static const QStringList EventMode;
    QVector <hkExpression> expressionsData;
    mutable std::mutex mutex;
};
}
#endif // HKBEXPRESSIONDATAARRAY_H
