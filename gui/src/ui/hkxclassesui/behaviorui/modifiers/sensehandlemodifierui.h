#ifndef SENSEHANDLEMODIFIERUI_H
#define SENSEHANDLEMODIFIERUI_H

#include <QStackedWidget>

#include "src/utility.h"

class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class SpinBox;
class QGroupBox;
class QuadVariableWidget;

namespace UI {

class HkxObject;
class hkbSenseHandleModifier;
class hkbVariableBindingSet;

class RangesUI;
class ExpressionDataArrayUI;

class SenseHandleModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    SenseHandleModifierUI();
    SenseHandleModifierUI& operator=(const SenseHandleModifierUI&) = delete;
    SenseHandleModifierUI(const SenseHandleModifierUI &) = delete;
    ~SenseHandleModifierUI() = default;
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
    void setSensorLocalOffset();
    void setLocalFrameName(int index);
    void setSensorLocalFrameName(int index);
    void setMinDistance();
    void setMaxDistance();
    void setDistanceOut();
    void setCollisionFilterInfo(int index);
    void setSensorRagdollBoneIndex(int index);
    void setSensorAnimationBoneIndex(int index);
    void setSensingMode(int index);
    void setExtrapolateSensorPosition();
    void setKeepFirstSensedHandle();
    void setFoundHandleOut();
    void viewSelectedChild(int row, int column);
    void variableTableElementSelected(int index, const QString &name);
    void returnToWidget();
private:
    void toggleSignals(bool toggleconnections);
    void addRange();
    void removeRange(int index);
    void setBindingVariable(int index, const QString & name);
    void loadDynamicTableRows();
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        CHILD_WIDGET
    };
private:
    static const QStringList collisionLayers;
    static const QStringList headerLabels;
    hkbSenseHandleModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QGroupBox *groupBox;
    RangesUI *rangeUI;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *sensorLocalOffset;
    ComboBox *localFrameName;
    ComboBox *sensorLocalFrameName;
    DoubleSpinBox *minDistance;
    DoubleSpinBox *maxDistance;
    DoubleSpinBox *distanceOut;
    ComboBox *collisionFilterInfo;
    ComboBox *sensorRagdollBoneIndex;
    ComboBox *sensorAnimationBoneIndex;
    ComboBox *sensingMode;
    CheckBox *extrapolateSensorPosition;
    CheckBox *keepFirstSensedHandle;
    CheckBox *foundHandleOut;
};
}
#endif // SENSEHANDLEMODIFIERUI_H
