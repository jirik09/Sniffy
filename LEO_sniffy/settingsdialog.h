#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

#include "GUI/widgetbuttons.h"
#include "GUI/widgetselection.h"
#include "GUI/widgetseparator.h"
#include "GUI/widgetlabel.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
    void open();

    int getThemeIndex() const;
    bool IsSessionRestoreRequest() const;
    void askForSessionRestore (QString device);
    bool askToSaveSession ();
    void setNoSessionFound();

private:
    Ui::SettingsDialog *ui;

    WidgetButtons *buttonsRestoreSession;
    WidgetSelection * selTheme;
    WidgetButtons *buttonsDone;

    QString settingFile;

    int restoreSession = 1; //0 no, 1 ask, 2 yes
    int sessionRestoreAnswer = -1; //-1 not answered
    int themeIndex;

    void saveSettings();
    void loadSettings();

private slots:
    void closeDialog(int isCanceled);
    void restartWarning();
};

#endif // SETTINGSDIALOG_H
