#ifndef DETECTCLOSETOGROUNDMODIFIERUI_H
#define DETECTCLOSETOGROUNDMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"

class QGridLayout;
class TableWidget;
class LineEdit;
class QLineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class QuadVariableWidget;

namespace UI {
	
class HkxObject;
class hkbDetectCloseToGroundModifier;
class hkbVariableBindingSet;

class DetectCloseToGroundModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    DetectCloseToGroundModifierUI();
    DetectCloseToGroundModifierUI& operator=(const DetectCloseToGroundModifierUI&) = delete;
    DetectCloseToGroundModifierUI(const DetectCloseToGroundModifierUI &) = delete;
    ~DetectCloseToGroundModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setCloseToGroundEventId(int index, const QString & name);
    void setCloseToGroundEventPayload();
    void setCloseToGroundHeight();
    void setRaycastDistanceDown();
    void setCollisionFilterInfo(int index);
    void setBoneIndex(int index);
    void setAnimBoneIndex(int index);
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList collisionLayers;
    static const QStringList headerLabels;
    hkbDetectCloseToGroundModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QLineEdit *closeToGroundEventPayload;
    DoubleSpinBox *closeToGroundHeight;
    DoubleSpinBox *raycastDistanceDown;
    ComboBox *collisionFilterInfo;
    ComboBox *boneIndex;
    ComboBox *animBoneIndex;
};
}
#endif // DETECTCLOSETOGROUNDMODIFIERUI_H
