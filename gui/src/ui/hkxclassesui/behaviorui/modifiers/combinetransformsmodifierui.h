#ifndef COMBINETRANSFORMSMODIFIERUI_H
#define COMBINETRANSFORMSMODIFIERUI_H

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
class hkbCombineTransformsModifier;
class hkbVariableBindingSet;

class CombineTransformsModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    CombineTransformsModifierUI();
    CombineTransformsModifierUI& operator=(const CombineTransformsModifierUI&) = delete;
    CombineTransformsModifierUI(const CombineTransformsModifierUI &) = delete;
    ~CombineTransformsModifierUI() = default;
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
    void setLeftTranslation();
    void setLeftRotation();
    void setRightTranslation();
    void setRightRotation();
    void setInvertLeftTransform();
    void setInvertRightTransform();
    void setInvertResult();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbCombineTransformsModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *translationOut;
    QuadVariableWidget *rotationOut;
    QuadVariableWidget *leftTranslation;
    QuadVariableWidget *leftRotation;
    QuadVariableWidget *rightTranslation;
    QuadVariableWidget *rightRotation;
    CheckBox *invertLeftTransform;
    CheckBox *invertRightTransform;
    CheckBox *invertResult;
};
}
#endif // COMBINETRANSFORMSMODIFIERUI_H
