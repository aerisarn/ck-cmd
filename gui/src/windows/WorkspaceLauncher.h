#ifndef WORKSPACELAUNCHER_H
#define WORKSPACELAUNCHER_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class WorkspaceLauncher;
}
QT_END_NAMESPACE

/**
 * This example shows, how to place a dock widget container and a static
 * sidebar into a QMainWindow
 */
    class WorkspaceLauncher : public QDialog
{
    Q_OBJECT

public:
    explicit WorkspaceLauncher(QWidget* parent = 0);
    ~WorkspaceLauncher();

    QString _workspace_folder;

private slots:
    void onSelectFolderButtonPressed();

private:
    Ui::WorkspaceLauncher* ui;

};

#endif // MAINWINDOW_H
