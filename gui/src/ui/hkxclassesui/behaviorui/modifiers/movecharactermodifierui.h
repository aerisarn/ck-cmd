#ifndef MOVECHARACTERMODIFIERUI_H
#define MOVECHARACTERMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class QuadVariableWidget;
class CheckBox;
class GenericTableWidget;

namespace UI {

class HkxObject;
class hkbMoveCharacterModifier;
class hkbVariableBindingSet;

class MoveCharacterModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    MoveCharacterModifierUI();
    MoveCharacterModifierUI& operator=(const MoveCharacterModifierUI&) = delete;
    MoveCharacterModifierUI(const MoveCharacterModifierUI &) = delete;
    ~MoveCharacterModifierUI() = default;
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
    void setOffsetPerSecondMS();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbMoveCharacterModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *offsetPerSecondMS;
};
}
#endif // MOVECHARACTERMODIFIERUI_H
