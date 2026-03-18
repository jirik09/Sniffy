#ifndef AGENTBRIDGE_H
#define AGENTBRIDGE_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <QPointF>
#include <QByteArray>
#include <QMutex>
#include <QSharedPointer>
#include <functional>

class DeviceMediator;
class AbstractModule;
class Comms;
class WidgetDisplay;

// Lightweight JSON-RPC 2.0 server over QLocalServer (named pipe).
// Auto-discovers all modules via DeviceMediator — zero per-module maintenance.
//
// Protocol:  Each message is a length-prefixed JSON-RPC 2.0 frame:
//   [4 bytes big-endian length][JSON payload]
//
// Pipe name:  "sniffy-agent" (platform-specific path resolved by Qt)
class AgentBridge : public QObject
{
    Q_OBJECT

public:
    explicit AgentBridge(DeviceMediator *mediator, QObject *parent = nullptr);
    ~AgentBridge() override;

    bool start(const QString &pipeName = QStringLiteral("sniffy-agent"));
    void stop();
    bool isListening() const;

signals:
    void clientConnected();
    void clientDisconnected();

private slots:
    void onNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();
    void onRawData(QByteArray data);

private:
    // JSON-RPC dispatch
    using Handler = std::function<QJsonObject(const QJsonObject &params)>;
    QHash<QString, Handler> m_handlers;
    void registerHandlers();

    QJsonObject dispatch(const QByteArray &raw);
    static QJsonObject makeResult(const QJsonValue &id, const QJsonValue &result);
    static QJsonObject makeError(const QJsonValue &id, int code, const QString &msg);

    void sendResponse(QLocalSocket *sock, const QJsonObject &response);

    // Module helpers
    QSharedPointer<AbstractModule> findModule(const QString &name) const;
    QJsonArray enumerateModules() const;
    static QString statusToString(int status);
    WidgetDisplay *findDisplay(const QString &moduleName, const QString &displayName) const;

    // Scope data snapshot captured from raw serial frames
    struct ScopeSnapshot {
        QVector<QVector<QPointF>> traces;
        int samplingRate = 0;
        int resolution   = 0;
        qint64 timestamp = 0;
    };
    ScopeSnapshot m_lastScope;
    QMutex m_scopeMutex;

    DeviceMediator *m_mediator = nullptr;
    QLocalServer   *m_server   = nullptr;
    QList<QLocalSocket *> m_clients;

    // Per-client receive buffer for length-prefixed framing
    QHash<QLocalSocket *, QByteArray> m_rxBuffers;
};

#endif // AGENTBRIDGE_H
