#ifndef BSDIRECTATMODIFIERUI_H
#define BSDIRECTATMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class QuadVariableWidget;
class SpinBox;

namespace UI {
	
class HkxObject;
class BSDirectAtModifier;
class hkbVariableBindingSet;

class BSDirectAtModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSDirectAtModifierUI();
    BSDirectAtModifierUI& operator=(const BSDirectAtModifierUI&) = delete;
    BSDirectAtModifierUI(const BSDirectAtModifierUI &) = delete;
    ~BSDirectAtModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties);
    void variableRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setDirectAtTarget();
    void setSourceBoneIndex(int index);
    void setStartBoneIndex(int index);
    void setEndBoneIndex(int index);
    void setLimitHeadingDegrees();
    void setLimitPitchDegrees();
    void setOffsetHeadingDegrees();
    void setOffsetPitchDegrees();
    void setOnGain();
    void setOffGain();
    void setTargetLocation();
    void setUserInfo();
    void setDirectAtCamera();
    void setDirectAtCameraX();
    void setDirectAtCameraY();
    void setDirectAtCameraZ();
    void setActive();
    void setCurrentHeadingOffset();
    void setCurrentPitchOffset();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSDirectAtModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    CheckBox *directAtTarget;
    ComboBox *sourceBoneIndex;
    ComboBox *startBoneIndex;
    ComboBox *endBoneIndex;
    DoubleSpinBox *limitHeadingDegrees;
    DoubleSpinBox *limitPitchDegrees;
    DoubleSpinBox *offsetHeadingDegrees;
    DoubleSpinBox *offsetPitchDegrees;
    DoubleSpinBox *onGain;
    DoubleSpinBox *offGain;
    QuadVariableWidget *targetLocation;
    SpinBox *userInfo;
    CheckBox *directAtCamera;
    DoubleSpinBox *directAtCameraX;
    DoubleSpinBox *directAtCameraY;
    DoubleSpinBox *directAtCameraZ;
    CheckBox *active;
    DoubleSpinBox *currentHeadingOffset;
    DoubleSpinBox *currentPitchOffset;
};
}
#endif // BSDIRECTATMODIFIERUI_H
