#ifndef FOOTIKCONTROLSMODIFIERUI_H
#define FOOTIKCONTROLSMODIFIERUI_H

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
class hkbFootIkControlsModifier;
class hkbVariableBindingSet;
class ExpressionDataArrayUI;
class LegUI;

class FootIkControlsModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    FootIkControlsModifierUI();
    FootIkControlsModifierUI& operator=(const FootIkControlsModifierUI&) = delete;
    FootIkControlsModifierUI(const FootIkControlsModifierUI &) = delete;
    ~FootIkControlsModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void variableRenamed(const QString & name, int index);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setOnOffGain();
    void setGroundAscendingGain();
    void setGroundDescendingGain();
    void setFootPlantedGain();
    void setFootRaisedGain();
    void setFootUnlockGain();
    void setWorldFromModelFeedbackGain();
    void setErrorUpDownBias();
    void setAlignWorldFromModelGain();
    void setHipOrientationGain();
    void setMaxKneeAngleDifference();
    void setAnkleOrientationGain();
    void setErrorOutTranslation();
    void setAlignWithGroundRotation();
    void viewSelectedChild(int row, int column);
    void variableTableElementSelected(int index, const QString &name);
    void returnToWidget();
private:
    void addLeg();
    void removeLeg(int index);
    void loadDynamicTableRows();
    void toggleSignals(bool toggleconnections);
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        CHILD_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbFootIkControlsModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QGroupBox *groupBox;
    LegUI *legUI;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *onOffGain;
    DoubleSpinBox *groundAscendingGain;
    DoubleSpinBox *groundDescendingGain;
    DoubleSpinBox *footPlantedGain;
    DoubleSpinBox *footRaisedGain;
    DoubleSpinBox *footUnlockGain;
    DoubleSpinBox *worldFromModelFeedbackGain;
    DoubleSpinBox *errorUpDownBias;
    DoubleSpinBox *alignWorldFromModelGain;
    DoubleSpinBox *hipOrientationGain;
    DoubleSpinBox *maxKneeAngleDifference;
    DoubleSpinBox *ankleOrientationGain;
    QuadVariableWidget *errorOutTranslation;
    QuadVariableWidget *alignWithGroundRotation;
};
}
#endif // FOOTIKCONTROLSMODIFIERUI_H
