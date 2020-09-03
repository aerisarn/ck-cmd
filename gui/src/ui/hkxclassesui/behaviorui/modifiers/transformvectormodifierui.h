#ifndef TRANSFORMVECTORMODIFIERUI_H
#define TRANSFORMVECTORMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class QuadVariableWidget;
class CheckBox;
class GenericTableWidget;
class DoubleSpinBox;

namespace UI {

class HkxObject;
class hkbTransformVectorModifier;
class hkbVariableBindingSet;

class TransformVectorModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    TransformVectorModifierUI();
    TransformVectorModifierUI& operator=(const TransformVectorModifierUI&) = delete;
    TransformVectorModifierUI(const TransformVectorModifierUI &) = delete;
    ~TransformVectorModifierUI() = default;
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
    void setRotation();
    void setTranslation();
    void setVectorIn();
    void setVectorOut();
    void setRotateOnly();
    void setInverse();
    void setComputeOnActivate();
    void setComputeOnModify();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbTransformVectorModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *rotation;
    QuadVariableWidget *translation;
    QuadVariableWidget *vectorIn;
    QuadVariableWidget *vectorOut;
    CheckBox *rotateOnly;
    CheckBox *inverse;
    CheckBox *computeOnActivate;
    CheckBox *computeOnModify;
};
}
#endif // TRANSFORMVECTORMODIFIERUI_H
