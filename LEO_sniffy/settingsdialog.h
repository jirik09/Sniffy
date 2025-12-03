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
#include "GUI/simplehprogressbar.h"
#include <QPlainTextEdit>
#include "customsettings.h"
#include "firmwaremanager.h"

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
    WidgetSelection * selFlashSource; // Local vs Remote firmware source

    // Flasher widgets
    WidgetButtons *buttonsFlash;
    SimpleHProgressBar *flashProgressBar;
    QPlainTextEdit *flashLogWindow;
    
    FirmwareManager *m_firmwareManager;

    WidgetLabel *infoLabel;
    WidgetButtons *buttonsDone;

    bool restartNeededWarning = false;

public slots:
    void closeDialog(int isCanceled);
    void onUserLoginChanged();

private:
    int m_lastStatusType = 0;

private slots:
    void restartWarning();
    void sessionButtonClicked(int index, int optionalEmitParam = 0);
    void onFlashButtonClicked(int index, int optionalEmitParam = 0);
    
    // Firmware Manager slots
    void onFirmwareProgress(int value, int total);
    void onFirmwareStatusMessage(const QString &msg, const QColor &color, int msgType);
    void onFirmwareLogMessage(const QString &message);
    void onFirmwareOperationStarted();
    void onFirmwareOperationFinished(bool success);
    void onFirmwareFlashed();

signals:
    void saveSessionRequested();
    void loadSessionRequested();
    void firmwareFlashed();
};

#endif // SETTINGSDIALOG_H
