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
#include "customsettings.h"
#include "GUI/simplehprogressbar.h"
#include "flasher/stlinkflasher.h"

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

private:
    Ui::SettingsDialog *ui;

    WidgetButtons *buttonsRestoreSession;
    WidgetButtons *buttonsSmartSessionGeometry;
    WidgetButtons *buttonsSession;
    WidgetSelection * selTheme;

    // Flasher widgets
    WidgetButtons *buttonsFlash;
    SimpleHProgressBar *flashProgressBar;
    WidgetLabel *flashStatusLabel;
    StLinkFlasher *m_flasher;
    QThread *m_flasherThread;

    WidgetLabel *infoLabel;
    WidgetButtons *buttonsDone;

    bool restartNeededWarning = false;

public slots:
    void closeDialog(int isCanceled);

private slots:
    void restartWarning();
    void sessionButtonClicked(int index, int optionalEmitParam = 0);
    void onFlashButtonClicked(int index, int optionalEmitParam = 0);
    void onFlashProgress(int value, int total);
    void onFlashLog(const QString &msg);
    void onFlashFinished(bool success, const QString &msg);
    void onDeviceConnected(const QString &info);

signals:
    void saveSessionRequested();
    void loadSessionRequested();
};

#endif // SETTINGSDIALOG_H
