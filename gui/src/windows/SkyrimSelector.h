#ifndef SKYRIMSELECTOR_H
#define SKYRIMSELECTOR_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class SkyrimSelector;
}
QT_END_NAMESPACE

class SkyrimSelector : public QDialog
{
    Q_OBJECT

public:
    explicit SkyrimSelector(QWidget* parent = 0);
    ~SkyrimSelector();

    QString _skyrim_folder;

private slots:
    void onSelectFolderButtonPressed();

private:
    Ui::SkyrimSelector* ui;

};

#endif // MAINWINDOW_H
