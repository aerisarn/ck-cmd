#ifndef BSISTATEMANAGERMODIFIER_H
#define BSISTATEMANAGERMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSIStateManagerModifier final: public hkbModifier
{
public:
    BSIStateManagerModifier(HkxFile *parent, long ref = 0);
    BSIStateManagerModifier& operator=(const BSIStateManagerModifier&) = delete;
    BSIStateManagerModifier(const BSIStateManagerModifier &) = delete;
    ~BSIStateManagerModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    struct BSiStateData{
        BSiStateData()
            : stateID(-1),
              iStateToSetAs(-1)
        {
            //
        }

        HkxSharedPtr pStateMachine;    //This may preserve state machines unexpectedly...
        int stateID;
        int iStateToSetAs;
    };

    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    int iStateVar;
    QVector <BSiStateData> stateData;
    mutable std::mutex mutex;
};
}
#endif // BSISTATEMANAGERMODIFIER_H
