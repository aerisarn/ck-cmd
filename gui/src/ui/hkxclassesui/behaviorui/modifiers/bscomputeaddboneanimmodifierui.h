#ifndef BSCOMPUTEADDBONEANIMMODIFIERUI_H
#define BSCOMPUTEADDBONEANIMMODIFIERUI_H

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

namespace UI {

class HkxObject;
class BSComputeAddBoneAnimModifier;
class hkbVariableBindingSet;

class BSComputeAddBoneAnimModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSComputeAddBoneAnimModifierUI();
    BSComputeAddBoneAnimModifierUI& operator=(const BSComputeAddBoneAnimModifierUI&) = delete;
    BSComputeAddBoneAnimModifierUI(const BSComputeAddBoneAnimModifierUI &) = delete;
    ~BSComputeAddBoneAnimModifierUI() = default;
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
    void setBoneIndex(int index);
    void setTranslationLSOut();
    void setRotationLSOut();
    void setScaleLSOut();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSComputeAddBoneAnimModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    ComboBox *boneIndex;
    QuadVariableWidget *translationLSOut;
    QuadVariableWidget *rotationLSOut;
    QuadVariableWidget *scaleLSOut;
};
}
#endif // BSCOMPUTEADDBONEANIMMODIFIERUI_H
