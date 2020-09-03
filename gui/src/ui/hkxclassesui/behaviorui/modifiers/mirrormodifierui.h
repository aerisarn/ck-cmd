#ifndef MIRRORMODIFIERUI_H
#define MIRRORMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class CheckBox;
class GenericTableWidget;

namespace UI {

class HkxObject;
class hkbMirrorModifier;
class hkbVariableBindingSet;

class MirrorModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    MirrorModifierUI();
    MirrorModifierUI& operator=(const MirrorModifierUI&) = delete;
    MirrorModifierUI(const MirrorModifierUI &) = delete;
    ~MirrorModifierUI() = default;
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
    void setIsAdditive();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbMirrorModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    CheckBox *isAdditive;
};
}
#endif // MIRRORMODIFIERUI_H
