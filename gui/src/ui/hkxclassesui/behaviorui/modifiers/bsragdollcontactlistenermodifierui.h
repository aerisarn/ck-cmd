#ifndef BSRAGDOLLCONTACTLISTENERMODIFIERUI_H
#define BSRAGDOLLCONTACTLISTENERMODIFIERUI_H

#include <QStackedWidget>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class QLineEdit;
class QGroupBox;
class CheckBox;
class GenericTableWidget;
class CheckButtonCombo;

namespace UI {
	
class HkxObject;
class BSRagdollContactListenerModifier;
class BoneIndexArrayUI;
class hkbVariableBindingSet;

class BSRagdollContactListenerModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    BSRagdollContactListenerModifierUI();
    BSRagdollContactListenerModifierUI& operator=(const BSRagdollContactListenerModifierUI&) = delete;
    BSRagdollContactListenerModifierUI(const BSRagdollContactListenerModifierUI &) = delete;
    ~BSRagdollContactListenerModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events, GenericTableWidget *ragdollBones);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewRagdollBones(int index);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setContactEventId(int index, const QString & name);
    void setContactEventPayload();
    void toggleBones(bool enable);
    void viewBones();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        BONE_INDEX_WIDGET
    };
private:
    static const QStringList headerLabels;
    BSRagdollContactListenerModifier *bsData;
    QGridLayout *topLyt;
    QGroupBox *groupBox;
    TableWidget *table;
    BoneIndexArrayUI *boneIndexUI;
    LineEdit *name;
    CheckBox *enable;
    QLineEdit *contactEventPayload;
    CheckButtonCombo *bones;
};
}

#endif // BSRAGDOLLCONTACTLISTENERMODIFIERUI_H
