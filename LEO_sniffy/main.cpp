#include "mainwindow.h"

#include <QSerialPort>

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "devicedescriptor.h"
#include "modules/scope/measurement.h"
#include "customsettings.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

#define LOG_OUTPUT

// Global log file path
#ifdef LOG_OUTPUT
static QString g_logFilePath;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    Q_UNUSED(context);

    QString level;
    switch (type) {
    case QtDebugMsg:     level = "Debug"; break;
    case QtInfoMsg:      level = "Info"; break;
    case QtWarningMsg:   level = "Warning"; break;
    case QtCriticalMsg:  level = "Critical"; break;
    case QtFatalMsg:     level = "Fatal"; break;
    }

    QString txt = QString("[%1] %2").arg(level, msg);

    // Print to stderr (keep default console behavior)
    fprintf(stderr, "%s\n", qPrintable(txt));

    if (!g_logFilePath.isEmpty()) {
        QFile outFile(g_logFilePath);
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&outFile);
            ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") << txt << "\n";
        }
    }
}
#endif

int main(int argc, char *argv[])
{
    // Qt6: QTextCodec setCodecForLocale removed (UTF-8 is default). Removed obsolete call.

    QApplication app(argc, argv);
    app.setOrganizationName("LEO Sniffy");
    app.setApplicationName("LEO Sniffy");
    app.setWindowIcon(QIcon(":/graphics/graphics/logo_color.png"));

    qRegisterMetaType<QList<Measurement *>>();
    qRegisterMetaType<QList<DeviceDescriptor>>();
    qRegisterMetaType<QSerialPort::SerialPortError>();

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(configPath);
    }

#ifdef LOG_OUTPUT
    // Initialize logging
    g_logFilePath = dir.filePath("leo_sniffy.log");
    
    // Output the log file location to the console so it's easy to find
    fprintf(stderr, "Log file location: %s\n", qPrintable(g_logFilePath));
    
    qInstallMessageHandler(messageHandler);
#endif

    CustomSettings::loadSettings(configPath + "/settings.ini");
    CustomSettings::setThemesList(Graphics::initThemesList());

    // Set theme explicitly (replaces deprecated getThemeInstance(int) usage)
    Graphics::setTheme(CustomSettings::getThemeIndex());
    if (auto themePtr = Graphics::getThemeInstance()) {
        app.setStyleSheet(themePtr->getAppGlobalStyle());
    }

    MainWindow w;
    w.show();
    return app.exec();
}
