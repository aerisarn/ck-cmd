#ifndef GETWORLDFROMMODELMODIFIERUI_H
#define GETWORLDFROMMODELMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"

class QGridLayout;
class TableWidget;
class LineEdit;
class QuadVariableWidget;
class CheckBox;
class GenericTableWidget;
class DoubleSpinBox;
class QuadVariableWidget;

namespace UI {

class HkxObject;
class hkbGetWorldFromModelModifier;

class GetWorldFromModelModifierUI final: public ::QGroupBox
{
    Q_OBJECT
public:
    GetWorldFromModelModifierUI();
    GetWorldFromModelModifierUI& operator=(const GetWorldFromModelModifierUI&) = delete;
    GetWorldFromModelModifierUI(const GetWorldFromModelModifierUI &) = delete;
    ~GetWorldFromModelModifierUI() = default;
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
    void setTranslationOut();
    void setRotationOut();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbGetWorldFromModelModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *translationOut;
    QuadVariableWidget *rotationOut;
};
}
#endif // GETWORLDFROMMODELMODIFIERUI_H
