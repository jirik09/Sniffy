#include "agentbridge.h"
#include "devicemediator.h"
#include "modules/abstractmodule.h"
#include "communication/comms.h"
#include "communication/commands.h"
#include "GUI/widgetcontrolmodule.h"
#include "GUI/widgetdisplay.h"

// GUI-synced RPCs — module-specific headers
#include "modules/syncpwm/syncpwmwindow.h"
#include "modules/syncpwm/syncpwmsettings.h"
#include "modules/counter/counterwindow.h"
#include "modules/counter/countertabhighfreq.h"
#include "modules/counter/countertablowfreq.h"
#include "modules/counter/countertabratio.h"
#include "modules/counter/countertabintervals.h"
#include "modules/scope/scope.h"
#include "modules/scope/scopewindow.h"
#include "modules/scope/panelsettings.h"
#include "modules/voltagesource/voltagesourcewindow.h"
#include "modules/arbgenerator/arbgeneratorwindow.h"
#include "modules/arbgenerator/arbgenpanelsettings.h"
#include "modules/patterngenerator/patterngeneratorwindow.h"
#include "modules/patterngenerator/patterngeneratorsettings.h"
#include "modules/voltmeter/voltmeterwindow.h"
#include "GUI/widgetselection.h"
#include <QTabWidget>

#include <QDataStream>
#include <QDateTime>
#include <QJsonValue>
#include <QMutexLocker>
#include <QtEndian>
#include <cmath>

// ──────────────────────────────────────────────────────────
// Helper: build a JSON snapshot of module state
// ──────────────────────────────────────────────────────────

static QJsonObject buildModuleState(QWidget *widget, const QString &moduleName)
{
    QJsonObject st;
    st[QStringLiteral("module")] = moduleName;

    // ── Counter ──
    if (auto *w = qobject_cast<CounterWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Counter");
        auto *tw = w->tabs->findChild<QTabWidget *>(QStringLiteral("tabWidget"));
        int mode = tw ? tw->currentIndex() : -1;
        static const char *modeNames[] = {"HF", "LF", "Ratio", "Intervals"};
        st[QStringLiteral("mode")] = mode;
        st[QStringLiteral("mode_name")] = QString::fromLatin1(mode >= 0 && mode <= 3 ? modeNames[mode] : "unknown");

        // HF tab
        QJsonObject hf;
        hf[QStringLiteral("quantity")]   = w->tabHighFreq->buttonsQuantitySwitch->getSelectedIndex();
        hf[QStringLiteral("error_mode")] = w->tabHighFreq->buttonsErrorSwitch->getSelectedIndex();
        hf[QStringLiteral("gate_time")]  = w->tabHighFreq->buttonsGateTime->getSelectedIndex();
        hf[QStringLiteral("averaging")]  = w->tabHighFreq->dialAveraging->getRealValue();
        st[QStringLiteral("hf")] = hf;

        // LF tab
        QJsonObject lf;
        lf[QStringLiteral("channel")]    = w->tabLowFreq->buttonsChannelSwitch->getSelectedIndex();
        lf[QStringLiteral("quantity")]   = w->tabLowFreq->buttonsQuantitySwitch->getSelectedIndex();
        lf[QStringLiteral("duty_cycle")] = w->tabLowFreq->buttonsDutyCycleSwitch->getSelectedIndex();
        lf[QStringLiteral("multiplier")] = w->tabLowFreq->buttonsMultiplierSwitch->getSelectedIndex();
        lf[QStringLiteral("sample_count_ch1")] = w->tabLowFreq->dialSampleCountCh1->getRealValue();
        lf[QStringLiteral("sample_count_ch2")] = w->tabLowFreq->dialSampleCountCh2->getRealValue();
        st[QStringLiteral("lf")] = lf;

        // Ratio tab
        QJsonObject rat;
        rat[QStringLiteral("sample_count")] = w->tabRatio->dialSampleCount->getRealValue();
        st[QStringLiteral("ratio")] = rat;

        // Intervals tab
        QJsonObject intv;
        intv[QStringLiteral("event_sequence")] = w->tabInter->buttonsEventsSeq->getSelectedIndex();
        intv[QStringLiteral("edge_a")] = w->tabInter->switchEdgeEventA->isCheckedLeft()
                                             ? QStringLiteral("rising") : QStringLiteral("falling");
        intv[QStringLiteral("edge_b")] = w->tabInter->switchEdgeEventB->isCheckedLeft()
                                             ? QStringLiteral("rising") : QStringLiteral("falling");
        intv[QStringLiteral("timeout")] = w->tabInter->dialTimeout->getRealValue();
        st[QStringLiteral("intervals")] = intv;
        return st;
    }

    // ── Scope ──
    if (qobject_cast<ScopeWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Oscilloscope");
        auto *ps = widget->findChild<PanelSettings *>(QStringLiteral("panelSet"));
        if (ps) {
            st[QStringLiteral("timebase")]        = ps->dialTimeBase->getRealValue();
            st[QStringLiteral("channels")]         = ps->buttonsChannelEnable->getStatus();
            st[QStringLiteral("trigger_mode")]     = ps->buttonsTriggerMode->getSelectedIndex();
            st[QStringLiteral("trigger_edge")]     = ps->buttonsTriggerEdge->getSelectedIndex();
            st[QStringLiteral("trigger_channel")]  = ps->buttonsTriggerChannel->getSelectedIndex();
            st[QStringLiteral("pretrigger")]       = ps->dialPretrigger->getRealValue();
            st[QStringLiteral("trigger_level")]    = ps->dialTriggerValue->getRealValue();
            st[QStringLiteral("memory")]           = ps->buttonsMemorySet->getSelectedIndex();
        }
        return st;
    }

    // ── Sync PWM ──
    if (auto *w = qobject_cast<SyncPwmWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Sync PWM");
        QJsonArray chArr;
        for (int i = 0; i < CHANNELS_NUM; ++i) {
            QJsonObject ch;
            ch[QStringLiteral("enabled")]   = w->settings->onOffCh[i]->isCheckedLeft();
            ch[QStringLiteral("frequency")] = w->settings->dialFreqCh[i]->getRealValue();
            ch[QStringLiteral("duty")]      = w->settings->dialDutyCh[i]->getRealValue();
            ch[QStringLiteral("phase")]     = w->settings->dialPhaseCh[i]->getRealValue();
            chArr.append(ch);
        }
        st[QStringLiteral("channels")] = chArr;
        return st;
    }

    // ── Arb / PWM Generator ──
    if (auto *w = qobject_cast<ArbGeneratorWindow *>(widget)) {
        auto *s = w->findChild<ArbGenPanelSettings *>();
        st[QStringLiteral("type")] = s && s->isPWMbased
                                         ? QStringLiteral("PWM generator")
                                         : QStringLiteral("Arbitrary generator");
        if (s) {
            st[QStringLiteral("num_channels")] = s->numChannelsEnabled;
            QJsonArray chArr;
            for (int i = 0; i < MAX_ARB_CHANNELS_NUM; ++i) {
                QJsonObject ch;
                ch[QStringLiteral("shape")]     = s->buttonsShape[i]->getSelectedIndex();
                ch[QStringLiteral("frequency")] = s->dialFreqCh[i]->getRealValue();
                ch[QStringLiteral("amplitude")] = s->dialAmplitudeCh[i]->getRealValue();
                ch[QStringLiteral("offset")]    = s->dialOffsetCh[i]->getRealValue();
                ch[QStringLiteral("duty")]      = s->dialDutyCh[i]->getRealValue();
                ch[QStringLiteral("phase")]     = s->dialPhaseCh[i]->getRealValue();
                chArr.append(ch);
            }
            st[QStringLiteral("channels")] = chArr;
        }
        return st;
    }

    // ── Pattern Generator ──
    if (auto *w = qobject_cast<PatternGeneratorWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Pattern generator");
        int idx = w->settings->comboPatternSelection->getSelectedIndex();
        st[QStringLiteral("pattern_index")] = idx;

        WidgetDialRange *freqDial = nullptr;
        switch (idx) {
        case 0:  freqDial = w->settings->dialUserDefFreq;    break;
        case 1:  freqDial = w->settings->dialCounterFreq;    break;
        case 2:  freqDial = w->settings->dialBinaryCodeFreq; break;
        case 3:  freqDial = w->settings->dialGrayCodeFreq;   break;
        case 4:  freqDial = w->settings->dialQuadratureFreq; break;
        case 5:  freqDial = w->settings->dialPrbsFreq;       break;
        case 6:  freqDial = w->settings->dialPwmFreq;        break;
        case 7:  freqDial = w->settings->dialLineCodeFreq;   break;
        case 8:  freqDial = w->settings->dial4b5bFreq;       break;
        case 9:  freqDial = w->settings->dialJohnsonFreq;    break;
        case 10: freqDial = w->settings->dialPdmFreq;        break;
        case 11: freqDial = w->settings->dialParBusFreq;     break;
        default: break;
        }
        if (freqDial)
            st[QStringLiteral("frequency")] = freqDial->getRealValue();
        return st;
    }

    // ── Voltmeter ──
    if (qobject_cast<VoltmeterWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Voltmeter");
        auto *mode = widget->findChild<WidgetButtons *>(QStringLiteral("ChannelVoltmode"));
        auto *avg  = widget->findChild<WidgetDialRange *>(QStringLiteral("voltAvgSamples"));
        auto *calc = widget->findChild<WidgetButtons *>(QStringLiteral("buttonscalc"));
        if (mode) st[QStringLiteral("mode")]      = mode->getSelectedIndex();
        if (avg)  st[QStringLiteral("averaging")]  = avg->getRealValue();
        if (calc) st[QStringLiteral("calc_mode")]  = calc->getSelectedIndex();
        return st;
    }

    // ── Voltage Source ── (minimal — dials only exist when running)
    if (qobject_cast<VoltageSourceWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Voltage source");
        return st;
    }

    // Unknown module type — return bare object
    return st;
}

// ──────────────────────────────────────────────────────────
// Construction / destruction
// ──────────────────────────────────────────────────────────

AgentBridge::AgentBridge(DeviceMediator *mediator, QObject *parent)
    : QObject(parent), m_mediator(mediator)
{
    registerHandlers();
}

AgentBridge::~AgentBridge() { stop(); }

// ──────────────────────────────────────────────────────────
// Server lifecycle
// ──────────────────────────────────────────────────────────

bool AgentBridge::start(const QString &pipeName)
{
    if (m_server) return m_server->isListening();

    // Remove stale socket from a previous unclean shutdown
    QLocalServer::removeServer(pipeName);

    m_server = new QLocalServer(this);
    m_server->setMaxPendingConnections(4);
    connect(m_server, &QLocalServer::newConnection, this, &AgentBridge::onNewConnection);

    if (!m_server->listen(pipeName)) {
        qWarning() << "[AgentBridge] Cannot listen on" << pipeName
                    << m_server->errorString();
        delete m_server;
        m_server = nullptr;
        return false;
    }

    // Tap into raw serial data so we can capture scope frames passively
    Comms *comms = m_mediator->getComms();
    if (comms)
        connect(comms, &Comms::newData, this, &AgentBridge::onRawData, Qt::UniqueConnection);

    qDebug() << "[AgentBridge] Listening on" << m_server->fullServerName();
    return true;
}

void AgentBridge::stop()
{
    if (!m_server) return;
    for (QLocalSocket *c : m_clients)
        c->disconnectFromServer();
    m_clients.clear();
    m_rxBuffers.clear();
    m_server->close();
    delete m_server;
    m_server = nullptr;
}

bool AgentBridge::isListening() const
{
    return m_server && m_server->isListening();
}

// ──────────────────────────────────────────────────────────
// Connection handling
// ──────────────────────────────────────────────────────────

void AgentBridge::onNewConnection()
{
    while (QLocalSocket *sock = m_server->nextPendingConnection()) {
        m_clients.append(sock);
        m_rxBuffers[sock] = QByteArray();
        connect(sock, &QLocalSocket::readyRead, this, &AgentBridge::onClientReadyRead);
        connect(sock, &QLocalSocket::disconnected, this, &AgentBridge::onClientDisconnected);
        emit clientConnected();
    }
}

void AgentBridge::onClientDisconnected()
{
    auto *sock = qobject_cast<QLocalSocket *>(sender());
    if (!sock) return;
    m_clients.removeAll(sock);
    m_rxBuffers.remove(sock);
    sock->deleteLater();
    emit clientDisconnected();
}

// Length-prefixed framing: [4B big-endian length][JSON]
void AgentBridge::onClientReadyRead()
{
    auto *sock = qobject_cast<QLocalSocket *>(sender());
    if (!sock) return;

    QByteArray &buf = m_rxBuffers[sock];
    buf.append(sock->readAll());

    while (buf.size() >= 4) {
        quint32 len = qFromBigEndian<quint32>(reinterpret_cast<const uchar *>(buf.constData()));
        if (len > 4 * 1024 * 1024) { // 4 MB sanity limit
            sock->disconnectFromServer();
            return;
        }
        if (static_cast<quint32>(buf.size()) < 4 + len) break; // incomplete

        QByteArray payload = buf.mid(4, static_cast<int>(len));
        buf.remove(0, 4 + static_cast<int>(len));

        QJsonObject resp = dispatch(payload);
        sendResponse(sock, resp);
    }
}

void AgentBridge::sendResponse(QLocalSocket *sock, const QJsonObject &response)
{
    QByteArray json = QJsonDocument(response).toJson(QJsonDocument::Compact);
    quint32 len = static_cast<quint32>(json.size());
    QByteArray frame(4, '\0');
    qToBigEndian(len, reinterpret_cast<uchar *>(frame.data()));
    frame.append(json);
    sock->write(frame);
    sock->flush();
}

// ──────────────────────────────────────────────────────────
// JSON-RPC 2.0 dispatch
// ──────────────────────────────────────────────────────────

QJsonObject AgentBridge::dispatch(const QByteArray &raw)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error != QJsonParseError::NoError)
        return makeError(QJsonValue::Null, -32700, QStringLiteral("Parse error: ") + err.errorString());

    QJsonObject req = doc.object();
    QJsonValue id     = req.value(QStringLiteral("id"));
    QString method    = req.value(QStringLiteral("method")).toString();
    QJsonObject params = req.value(QStringLiteral("params")).toObject();

    if (method.isEmpty())
        return makeError(id, -32600, QStringLiteral("Missing 'method'"));

    auto it = m_handlers.constFind(method);
    if (it == m_handlers.constEnd())
        return makeError(id, -32601, QStringLiteral("Unknown method: ") + method);

    QJsonObject result = it.value()(params);

    // If the handler itself returned an error object, pass it through
    if (result.contains(QStringLiteral("error")))
        return makeError(id, result.value(QStringLiteral("code")).toInt(-32000),
                         result.value(QStringLiteral("error")).toString());

    return makeResult(id, result);
}

QJsonObject AgentBridge::makeResult(const QJsonValue &id, const QJsonValue &result)
{
    return QJsonObject{
        {QStringLiteral("jsonrpc"), QStringLiteral("2.0")},
        {QStringLiteral("id"), id},
        {QStringLiteral("result"), result}
    };
}

QJsonObject AgentBridge::makeError(const QJsonValue &id, int code, const QString &msg)
{
    return QJsonObject{
        {QStringLiteral("jsonrpc"), QStringLiteral("2.0")},
        {QStringLiteral("id"), id},
        {QStringLiteral("error"), QJsonObject{
             {QStringLiteral("code"), code},
             {QStringLiteral("message"), msg}
         }}
    };
}

// ──────────────────────────────────────────────────────────
// Module helpers
// ──────────────────────────────────────────────────────────

QSharedPointer<AbstractModule> AgentBridge::findModule(const QString &name) const
{
    const QString lower = name.toLower();
    for (const auto &m : m_mediator->getModulesList()) {
        if (m->getModuleName().toLower() == lower)
            return m;
    }
    return nullptr;
}

QString AgentBridge::statusToString(int status)
{
    switch (static_cast<ModuleStatus>(status)) {
    case ModuleStatus::STOP:         return QStringLiteral("stopped");
    case ModuleStatus::PLAY:         return QStringLiteral("running");
    case ModuleStatus::PAUSE:        return QStringLiteral("paused");
    case ModuleStatus::HIDDEN_PLAY:  return QStringLiteral("running_hidden");
    case ModuleStatus::HIDDEN_PAUSE: return QStringLiteral("paused_hidden");
    case ModuleStatus::LOCKED:       return QStringLiteral("locked");
    }
    return QStringLiteral("unknown");
}

QJsonArray AgentBridge::enumerateModules() const
{
    QJsonArray arr;
    for (const auto &m : m_mediator->getModulesList()) {
        QJsonObject obj;
        obj[QStringLiteral("name")]      = m->getModuleName();
        obj[QStringLiteral("prefix")]    = QString::fromLatin1(m->getCommandPrefix());
        obj[QStringLiteral("status")]    = statusToString(static_cast<int>(m->getModuleStatus()));
        obj[QStringLiteral("available")] = m->isAvailable();
        obj[QStringLiteral("active")]    = m->isActive();
        arr.append(obj);
    }
    return arr;
}

WidgetDisplay *AgentBridge::findDisplay(const QString &moduleName, const QString &displayName) const
{
    auto mod = findModule(moduleName);
    if (!mod) return nullptr;
    QWidget *w = mod->getWidget();
    if (!w) return nullptr;
    return w->findChild<WidgetDisplay *>(displayName);
}

// ──────────────────────────────────────────────────────────
// Raw serial data tap — capture scope frames passively
// ──────────────────────────────────────────────────────────

void AgentBridge::onRawData(QByteArray data)
{
    // Only capture scope data frames (prefix "OSCP", sub-header "OSC_")
    static const QByteArray SCOPE_PREFIX = QByteArrayLiteral("OSCP");
    static const QByteArray SCOPE_DATA   = QByteArrayLiteral("OSC_");

    if (data.size() < 8) return;
    if (data.left(4) != SCOPE_PREFIX) return;

    QByteArray subHeader = data.mid(4, 4);
    if (subHeader != SCOPE_DATA) return;

    // Parse the binary scope frame (same format as Scope::parseData)
    QByteArray payload = data.mid(4); // skip module prefix
    QDataStream stream(payload);

    if (payload.size() < 32) return;

    QVector<QVector<QPointF>> allTraces;
    int currentChannel = 0;
    int numChannels = 1;
    int capturedSamplingRate = 0;
    int capturedResolution = 0;

    while (numChannels > currentChannel) {
        int samplingFreq;
        stream >> samplingFreq;
        stream >> samplingFreq;
        capturedSamplingRate = samplingFreq;

        quint8 tmpByte;
        stream >> tmpByte;
        int resolution = tmpByte;
        capturedResolution = resolution;

        int length;
        stream >> length;
        int samples = resolution > 8 ? length / 2 : length;

        quint16 tmpShort;
        stream >> tmpShort;
        qint16 rangeMin = static_cast<qint16>(tmpShort);
        stream >> tmpShort;
        qint16 rangeMax = static_cast<qint16>(tmpShort);

        stream >> tmpByte; // skip
        stream >> tmpByte;
        currentChannel = tmpByte;
        stream >> tmpByte;
        numChannels = tmpByte;

        if (length <= 0 || length > 500000) return;

        QVector<QPointF> points;
        points.reserve(samples);

        qreal minX = static_cast<qreal>(samples) / samplingFreq * (-1.0); // approximate
        qreal x, y;

        if (resolution > 8) {
            for (int j = 0; j < length / 2; ++j) {
                if (stream.atEnd()) break;
                quint8 lo, hi;
                stream >> lo;
                stream >> hi;
                quint16 raw = lo + hi * 256;
                x = static_cast<qreal>(j) / samplingFreq + minX;
                y = (raw * static_cast<qreal>(rangeMax - rangeMin) / (std::pow(2, resolution) - 1) + rangeMin) / 1000.0;
                points.append(QPointF(x, y));
            }
        } else {
            for (int j = 0; j < length; ++j) {
                if (stream.atEnd()) break;
                stream >> tmpByte;
                x = static_cast<qreal>(j) / samplingFreq + minX;
                y = (tmpByte * static_cast<qreal>(rangeMax - rangeMin) / (std::pow(2, resolution) - 1) + rangeMin) / 1000.0;
                points.append(QPointF(x, y));
            }
        }

        while (allTraces.size() < currentChannel) allTraces.append(QVector<QPointF>());
        if (allTraces.size() >= currentChannel)
            allTraces.replace(currentChannel - 1, points);
        else
            allTraces.append(points);
    }

    QMutexLocker lock(&m_scopeMutex);
    m_lastScope.traces       = allTraces;
    m_lastScope.samplingRate = capturedSamplingRate;
    m_lastScope.resolution   = capturedResolution;
    m_lastScope.timestamp    = QDateTime::currentMSecsSinceEpoch();
}

// ──────────────────────────────────────────────────────────
// Handler registration — the single place defining the API
// ──────────────────────────────────────────────────────────

void AgentBridge::registerHandlers()
{
    // ── ping ──
    m_handlers[QStringLiteral("ping")] = [](const QJsonObject &) -> QJsonObject {
        return QJsonObject{{QStringLiteral("pong"), true}};
    };

    // ── get_status ──
    m_handlers[QStringLiteral("get_status")] = [this](const QJsonObject &) -> QJsonObject {
        QJsonObject r;
        r[QStringLiteral("connected")]  = m_mediator->getIsConnected();
        r[QStringLiteral("demo_mode")]  = m_mediator->getIsDemoMode();
        r[QStringLiteral("device")]     = m_mediator->getDeviceName();
        r[QStringLiteral("modules")]    = enumerateModules();
        return r;
    };

    // ── list_modules ──
    m_handlers[QStringLiteral("list_modules")] = [this](const QJsonObject &) -> QJsonObject {
        return QJsonObject{{QStringLiteral("modules"), enumerateModules()}};
    };

    // ── module_start ──
    m_handlers[QStringLiteral("module_start")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        if (!mod->isAvailable()) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not configured by firmware")}, {QStringLiteral("code"), -32001}};
        if (mod->isActive()) {
            QJsonObject result{{QStringLiteral("ok"), true}, {QStringLiteral("note"), QStringLiteral("Already running")}};
            QWidget *w = mod->getWidget();
            if (w) result[QStringLiteral("state")] = buildModuleState(w, p.value(QStringLiteral("module")).toString());
            return result;
        }

        // Trigger the same flow as clicking the module button in the GUI.
        // STOP = the current status → this arms, configures and starts the module.
        mod->widgetControlClicked(ModuleStatus::STOP);

        // Return current state so the agent knows the session-restored config.
        QWidget *w = mod->getWidget();
        QJsonObject result{{QStringLiteral("ok"), true}};
        if (w) {
            QJsonObject state = buildModuleState(w, p.value(QStringLiteral("module")).toString());
            result[QStringLiteral("state")] = state;
        }
        return result;
    };

    // ── module_stop ──
    m_handlers[QStringLiteral("module_stop")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        if (!mod->isActive()) return QJsonObject{{QStringLiteral("ok"), true}, {QStringLiteral("note"), QStringLiteral("Already stopped")}};

        // Direct call is safe — bridge and modules share the main GUI thread.
        mod->closeModule();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── module_get_config ──
    m_handlers[QStringLiteral("module_get_config")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};

        QByteArray cfg = mod->getConfiguration();
        QJsonObject r;
        r[QStringLiteral("module")] = mod->getModuleName();
        r[QStringLiteral("config_base64")] = QString::fromLatin1(cfg.toBase64());
        return r;
    };

    // ── write_command — send a raw protocol command to the MCU ──
    m_handlers[QStringLiteral("write_command")] = [this](const QJsonObject &p) -> QJsonObject {
        if (!m_mediator->getIsConnected())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not connected")}, {QStringLiteral("code"), -32002}};

        QString cmdStr = p.value(QStringLiteral("command")).toString();
        if (cmdStr.isEmpty())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Missing 'command'")}, {QStringLiteral("code"), -32602}};

        Comms *comms = m_mediator->getComms();
        if (!comms)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Communication unavailable")}, {QStringLiteral("code"), -32003}};

        // Ensure command ends with semicolon (protocol requirement)
        QByteArray raw = cmdStr.toUtf8();
        if (!raw.endsWith(';')) raw.append(';');
        comms->write(raw);

        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── write_command_int — send MODULE:FEATURE with integer param ──
    m_handlers[QStringLiteral("write_command_int")] = [this](const QJsonObject &p) -> QJsonObject {
        if (!m_mediator->getIsConnected())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not connected")}, {QStringLiteral("code"), -32002}};

        QString module  = p.value(QStringLiteral("prefix")).toString();
        QString feature = p.value(QStringLiteral("feature")).toString();
        int value       = p.value(QStringLiteral("value")).toInt();

        if (module.isEmpty() || feature.isEmpty())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Missing prefix/feature")}, {QStringLiteral("code"), -32602}};

        Comms *comms = m_mediator->getComms();
        if (!comms)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Communication unavailable")}, {QStringLiteral("code"), -32003}};

        comms->write(module.toLatin1(), feature.toLatin1(), value);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── scope_read_data — return the last captured scope snapshot ──
    m_handlers[QStringLiteral("scope_read_data")] = [this](const QJsonObject &) -> QJsonObject {
        QMutexLocker lock(&m_scopeMutex);

        if (m_lastScope.traces.isEmpty())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("No scope data captured yet")}, {QStringLiteral("code"), -32004}};

        QJsonArray channels;
        for (const auto &trace : m_lastScope.traces) {
            QJsonArray xArr, yArr;
            for (const QPointF &pt : trace) {
                xArr.append(pt.x());
                yArr.append(pt.y());
            }
            QJsonObject ch;
            ch[QStringLiteral("x")] = xArr;
            ch[QStringLiteral("y")] = yArr;
            ch[QStringLiteral("num_samples")] = trace.size();
            channels.append(ch);
        }

        QJsonObject r;
        r[QStringLiteral("channels")]      = channels;
        r[QStringLiteral("sampling_rate")]  = m_lastScope.samplingRate;
        r[QStringLiteral("resolution")]     = m_lastScope.resolution;
        r[QStringLiteral("timestamp_ms")]   = m_lastScope.timestamp;
        return r;
    };

    // ── scope_read_data_compact — same data, base64-packed floats ──
    m_handlers[QStringLiteral("scope_read_data_compact")] = [this](const QJsonObject &) -> QJsonObject {
        QMutexLocker lock(&m_scopeMutex);

        if (m_lastScope.traces.isEmpty())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("No scope data captured yet")}, {QStringLiteral("code"), -32004}};

        QJsonArray channels;
        for (const auto &trace : m_lastScope.traces) {
            // Pack as interleaved float32 LE pairs: [x0,y0, x1,y1, ...]
            QByteArray packed;
            packed.reserve(trace.size() * 8);
            for (const QPointF &pt : trace) {
                float fx = static_cast<float>(pt.x());
                float fy = static_cast<float>(pt.y());
                packed.append(reinterpret_cast<const char *>(&fx), 4);
                packed.append(reinterpret_cast<const char *>(&fy), 4);
            }
            QJsonObject ch;
            ch[QStringLiteral("xy_base64")]   = QString::fromLatin1(packed.toBase64());
            ch[QStringLiteral("num_samples")]  = trace.size();
            channels.append(ch);
        }

        QJsonObject r;
        r[QStringLiteral("channels")]     = channels;
        r[QStringLiteral("sampling_rate")] = m_lastScope.samplingRate;
        r[QStringLiteral("resolution")]    = m_lastScope.resolution;
        r[QStringLiteral("timestamp_ms")]  = m_lastScope.timestamp;
        r[QStringLiteral("encoding")]      = QStringLiteral("float32le_interleaved_xy");
        return r;
    };

    // ── list_displays — enumerate WidgetDisplay children of a module ──
    m_handlers[QStringLiteral("list_displays")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};

        QWidget *w = mod->getWidget();
        QJsonArray arr;
        if (w) {
            for (auto *d : w->findChildren<WidgetDisplay *>())
                arr.append(d->getDisplayName());
        }
        return QJsonObject{{QStringLiteral("displays"), arr}};
    };

    // ── display_save_history — save a display's history to file ──
    m_handlers[QStringLiteral("display_save_history")] = [this](const QJsonObject &p) -> QJsonObject {
        auto *d = findDisplay(p.value(QStringLiteral("module")).toString(),
                              p.value(QStringLiteral("display")).toString());
        if (!d) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Display not found")}, {QStringLiteral("code"), -32602}};
        d->saveHistory();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── display_clear_history — clear a display's history ──
    m_handlers[QStringLiteral("display_clear_history")] = [this](const QJsonObject &p) -> QJsonObject {
        auto *d = findDisplay(p.value(QStringLiteral("module")).toString(),
                              p.value(QStringLiteral("display")).toString());
        if (!d) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Display not found")}, {QStringLiteral("code"), -32602}};
        d->clearHistory();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── display_set_history_size — change history sample count (dial range) ──
    m_handlers[QStringLiteral("display_set_history_size")] = [this](const QJsonObject &p) -> QJsonObject {
        auto *d = findDisplay(p.value(QStringLiteral("module")).toString(),
                              p.value(QStringLiteral("display")).toString());
        if (!d) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Display not found")}, {QStringLiteral("code"), -32602}};
        int size = p.value(QStringLiteral("size")).toInt(100);
        if (size < 100) size = 100;
        if (size > 1000) size = 1000;
        d->setHistorySampleCount(size);
        return QJsonObject{{QStringLiteral("ok"), true}, {QStringLiteral("size"), size}};
    };

    // ═══════════════════════════════════════════════════════════
    //  GUI-synced high-level RPCs
    //  These update both the GUI widgets AND send MCU commands,
    //  keeping the desktop app in perfect sync with the agent.
    // ═══════════════════════════════════════════════════════════

    // ── Sync PWM ─────────────────────────────────────────────

    // spwm_set_frequency — set channel frequency (Hz), GUI + MCU
    m_handlers[QStringLiteral("spwm_set_frequency")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<SyncPwmWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Sync PWM module")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        if (ch < 0 || ch >= CHANNELS_NUM)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid channel (0-3)")}, {QStringLiteral("code"), -32602}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        win->settings->dialFreqCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // spwm_set_duty — set channel duty cycle (%), GUI + MCU
    m_handlers[QStringLiteral("spwm_set_duty")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<SyncPwmWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Sync PWM module")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        if (ch < 0 || ch >= CHANNELS_NUM)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid channel (0-3)")}, {QStringLiteral("code"), -32602}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        win->settings->dialDutyCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // spwm_set_phase — set channel phase (degrees 0-360), GUI + MCU
    m_handlers[QStringLiteral("spwm_set_phase")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<SyncPwmWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Sync PWM module")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        if (ch < 0 || ch >= CHANNELS_NUM)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid channel (0-3)")}, {QStringLiteral("code"), -32602}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        win->settings->dialPhaseCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // spwm_set_channel_enable — enable/disable a PWM channel, GUI + MCU
    m_handlers[QStringLiteral("spwm_set_channel_enable")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<SyncPwmWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Sync PWM module")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        if (ch < 0 || ch >= CHANNELS_NUM)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid channel (0-3)")}, {QStringLiteral("code"), -32602}};
        bool enable = p.value(QStringLiteral("enabled")).toBool(true);
        // Left = ON (index 0), Right = OFF (index 1); setLeft/setRight are private slots
        QMetaObject::invokeMethod(win->settings->onOffCh[ch],
                                  enable ? "setLeft" : "setRight", Q_ARG(bool, false));
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── Counter ──────────────────────────────────────────────

    // counter_set_mode — switch counter mode tab, GUI + MCU
    //   0 = High Frequency, 1 = Low Frequency, 2 = Ratio, 3 = Intervals
    m_handlers[QStringLiteral("counter_set_mode")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<CounterWindow *>(mod->getWidget());
        if (!win || !win->tabs)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Counter module")}, {QStringLiteral("code"), -32602}};
        int mode = p.value(QStringLiteral("mode")).toInt(0);
        if (mode < 0 || mode > 3)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid mode (0=HF,1=LF,2=Ratio,3=Intervals)")}, {QStringLiteral("code"), -32602}};
        // Switch the visible tab
        QTabWidget *tw = win->tabs->findChild<QTabWidget *>();
        if (tw) tw->setCurrentIndex(mode);
        // Switch the display panels (show/hide the correct WidgetDisplay for the new mode)
        QMetaObject::invokeMethod(win, "switchCounterModeCallback", Q_ARG(int, mode));
        // Trigger the module's mode-switch callback (sends MCU commands + updates state)
        QMetaObject::invokeMethod(mod.data(), "switchCounterModeCallback", Q_ARG(int, mode));
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // counter_set_gate_time — HF mode gate time selection, GUI + MCU
    //   0=100ms, 1=500ms, 2=1s, 3=5s, 4=10s
    m_handlers[QStringLiteral("counter_set_gate_time")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<CounterWindow *>(mod->getWidget());
        if (!win || !win->tabHighFreq)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Counter module")}, {QStringLiteral("code"), -32602}};
        int idx = p.value(QStringLiteral("index")).toInt(0);
        // clickedInternal updates the button visual AND emits the clicked signal
        win->tabHighFreq->buttonsGateTime->clickedInternal(idx);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── Scope ────────────────────────────────────────────────

    // scope_set_timebase — set time/div (seconds), MCU reconfigured
    m_handlers[QStringLiteral("scope_set_timebase")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        scope->updateTimebase(val);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_mode — auto|auto_fast|normal|single|stop
    m_handlers[QStringLiteral("scope_set_trigger_mode")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        QString ms = p.value(QStringLiteral("mode")).toString().toLower();
        ScopeTriggerMode mode;
        if      (ms == QStringLiteral("auto"))      mode = ScopeTriggerMode::TRIG_AUTO;
        else if (ms == QStringLiteral("auto_fast"))  mode = ScopeTriggerMode::TRIG_AUTO_FAST;
        else if (ms == QStringLiteral("normal"))     mode = ScopeTriggerMode::TRIG_NORMAL;
        else if (ms == QStringLiteral("single"))     mode = ScopeTriggerMode::TRIG_SINGLE;
        else if (ms == QStringLiteral("stop"))       mode = ScopeTriggerMode::TRIG_STOP;
        else return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid mode: auto|auto_fast|normal|single|stop")}, {QStringLiteral("code"), -32602}};
        scope->updateTriggerMode(mode);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_edge — rising|falling
    m_handlers[QStringLiteral("scope_set_trigger_edge")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        QString es = p.value(QStringLiteral("edge")).toString().toLower();
        ScopeTriggerEdge edge;
        if      (es == QStringLiteral("rising"))  edge = ScopeTriggerEdge::EDGE_RISING;
        else if (es == QStringLiteral("falling")) edge = ScopeTriggerEdge::EDGE_FALLING;
        else return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid edge: rising|falling")}, {QStringLiteral("code"), -32602}};
        scope->updateTriggerEdge(edge);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_channel — 0-based channel index
    m_handlers[QStringLiteral("scope_set_trigger_channel")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        scope->updateTriggerChannel(ch);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_channels — enable channels by bitmask (bit 0 = CH1, etc.)
    m_handlers[QStringLiteral("scope_set_channels")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        int mask = p.value(QStringLiteral("channel_mask")).toInt(1);
        scope->updateChannelsEnable(mask);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_pretrigger — pretrigger percentage (0-100)
    m_handlers[QStringLiteral("scope_set_pretrigger")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        float pct = static_cast<float>(p.value(QStringLiteral("percent")).toDouble(50.0));
        scope->updatePretrigger(pct);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_level — trigger level percentage (0-100)
    m_handlers[QStringLiteral("scope_set_trigger_level")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        float pct = static_cast<float>(p.value(QStringLiteral("percent")).toDouble(50.0));
        scope->updateTriggerLevel(pct);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_resolution — ADC resolution (8 or 12 bits)
    m_handlers[QStringLiteral("scope_set_resolution")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        int bits = p.value(QStringLiteral("bits")).toInt(12);
        scope->updateResolution(bits);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── Voltage Source ───────────────────────────────────────

    // vout_set_voltage — set DAC output voltage for a channel, GUI + MCU
    m_handlers[QStringLiteral("vout_set_voltage")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<VoltageSourceWindow *>(mod->getWidget());
        if (!win)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltage Source module")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        qreal voltage = p.value(QStringLiteral("voltage")).toDouble();
        // Invoke the window's internal callback — updates display, bar, and emits
        // voltageChanged signal which triggers VoltageSource::voltageChangedCallback → MCU
        QMetaObject::invokeMethod(win, "dialChangedCallback",
                                  Q_ARG(qreal, voltage), Q_ARG(int, ch));
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── Counter — remaining widgets ──────────────────────

    // counter_hf_config — High Frequency tab: quantity, error_mode, averaging, gate_time
    m_handlers[QStringLiteral("counter_hf_config")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<CounterWindow *>(mod->getWidget());
        if (!win || !win->tabHighFreq)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Counter module")}, {QStringLiteral("code"), -32602}};
        // quantity: 0 = Frequency, 1 = Period
        if (p.contains(QStringLiteral("quantity")))
            win->tabHighFreq->buttonsQuantitySwitch->clickedInternal(p.value(QStringLiteral("quantity")).toInt());
        // error_mode: 0 = Err, 1 = Err AVG
        if (p.contains(QStringLiteral("error_mode")))
            win->tabHighFreq->buttonsErrorSwitch->clickedInternal(p.value(QStringLiteral("error_mode")).toInt());
        // averaging: number of samples (2+)
        if (p.contains(QStringLiteral("averaging")))
            win->tabHighFreq->dialAveraging->setRealValue(static_cast<float>(p.value(QStringLiteral("averaging")).toDouble()), false);
        // gate_time: 0=100ms, 1=500ms, 2=1s, 3=5s, 4=10s
        if (p.contains(QStringLiteral("gate_time")))
            win->tabHighFreq->buttonsGateTime->clickedInternal(p.value(QStringLiteral("gate_time")).toInt());
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // counter_lf_config — Low Frequency tab
    m_handlers[QStringLiteral("counter_lf_config")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<CounterWindow *>(mod->getWidget());
        if (!win || !win->tabLowFreq)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Counter module")}, {QStringLiteral("code"), -32602}};
        // channel: 0 = CH1, 1 = CH2
        if (p.contains(QStringLiteral("channel")))
            win->tabLowFreq->buttonsChannelSwitch->clickedInternal(p.value(QStringLiteral("channel")).toInt());
        // quantity: 0 = Frequency, 1 = Period
        if (p.contains(QStringLiteral("quantity")))
            win->tabLowFreq->buttonsQuantitySwitch->clickedInternal(p.value(QStringLiteral("quantity")).toInt());
        // multiplier: 0=1x, 1=2x, 2=4x, 3=8x
        if (p.contains(QStringLiteral("multiplier")))
            win->tabLowFreq->buttonsMultiplierSwitch->clickedInternal(p.value(QStringLiteral("multiplier")).toInt());
        // duty_cycle: 0 = Disable, 1 = Enable
        if (p.contains(QStringLiteral("duty_cycle")))
            win->tabLowFreq->buttonsDutyCycleSwitch->clickedInternal(p.value(QStringLiteral("duty_cycle")).toInt());
        // sample_count + sample_channel (0=CH1, 1=CH2)
        if (p.contains(QStringLiteral("sample_count"))) {
            float sc = static_cast<float>(p.value(QStringLiteral("sample_count")).toDouble());
            int sch = p.value(QStringLiteral("sample_channel")).toInt(0);
            if (sch == 1)
                win->tabLowFreq->dialSampleCountCh2->setRealValue(sc, false);
            else
                win->tabLowFreq->dialSampleCountCh1->setRealValue(sc, false);
        }
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // counter_rat_set_sample_count — Ratio tab sample count dial
    m_handlers[QStringLiteral("counter_rat_set_sample_count")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<CounterWindow *>(mod->getWidget());
        if (!win || !win->tabRatio)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Counter module")}, {QStringLiteral("code"), -32602}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        win->tabRatio->dialSampleCount->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // counter_int_config — Intervals tab
    m_handlers[QStringLiteral("counter_int_config")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<CounterWindow *>(mod->getWidget());
        if (!win || !win->tabInter)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Counter module")}, {QStringLiteral("code"), -32602}};
        // event_sequence: 0 = A→B, 1 = B→A
        if (p.contains(QStringLiteral("event_sequence")))
            win->tabInter->buttonsEventsSeq->clickedInternal(p.value(QStringLiteral("event_sequence")).toInt());
        // edge_a: "rising" or "falling"
        if (p.contains(QStringLiteral("edge_a"))) {
            bool rising = p.value(QStringLiteral("edge_a")).toString().toLower() == QStringLiteral("rising");
            QMetaObject::invokeMethod(win->tabInter->switchEdgeEventA,
                                      rising ? "setLeft" : "setRight", Q_ARG(bool, false));
        }
        // edge_b: "rising" or "falling"
        if (p.contains(QStringLiteral("edge_b"))) {
            bool rising = p.value(QStringLiteral("edge_b")).toString().toLower() == QStringLiteral("rising");
            QMetaObject::invokeMethod(win->tabInter->switchEdgeEventB,
                                      rising ? "setLeft" : "setRight", Q_ARG(bool, false));
        }
        // timeout: seconds
        if (p.contains(QStringLiteral("timeout")))
            win->tabInter->dialTimeout->setRealValue(static_cast<float>(p.value(QStringLiteral("timeout")).toDouble()), false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── Arbitrary / PWM Generator ────────────────────────

    // arbgen_set_frequency — set signal frequency for a channel
    m_handlers[QStringLiteral("arbgen_set_frequency")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        settings->dialFreqCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_shape — 0=Sine, 1=Saw, 2=Rect, 3=Arb
    m_handlers[QStringLiteral("arbgen_set_shape")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        int idx = p.value(QStringLiteral("index")).toInt(0);
        settings->buttonsShape[ch]->clickedInternal(idx);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_amplitude — set signal amplitude (V) per channel
    m_handlers[QStringLiteral("arbgen_set_amplitude")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        settings->dialAmplitudeCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_offset — set signal offset (V) per channel
    m_handlers[QStringLiteral("arbgen_set_offset")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        settings->dialOffsetCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_duty — set duty cycle (%) per channel
    m_handlers[QStringLiteral("arbgen_set_duty")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        settings->dialDutyCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_phase — set phase (degrees 0-360) per channel
    m_handlers[QStringLiteral("arbgen_set_phase")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        settings->dialPhaseCh[ch]->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_channels — set number of active channels (1-4)
    m_handlers[QStringLiteral("arbgen_set_channels")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int count = p.value(QStringLiteral("count")).toInt(1);
        settings->buttonsEnable->clickedInternal(count - 1); // 0-based index
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── Pattern Generator ────────────────────────────────

    // patgen_set_pattern — select pattern type (0-14)
    m_handlers[QStringLiteral("patgen_set_pattern")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<PatternGeneratorWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Pattern Generator module")}, {QStringLiteral("code"), -32602}};
        int idx = p.value(QStringLiteral("index")).toInt(0);
        win->settings->comboPatternSelection->setSelected(idx, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // patgen_set_frequency — set the frequency dial of the currently selected pattern
    m_handlers[QStringLiteral("patgen_set_frequency")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<PatternGeneratorWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Pattern Generator module")}, {QStringLiteral("code"), -32602}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        // Find the visible frequency dial for the current pattern selection
        int pattern = win->settings->comboPatternSelection->getSelectedIndex();
        WidgetDialRange *freqDial = nullptr;
        switch (pattern) {
        case 0:  freqDial = win->settings->dialUserDefFreq;   break;
        case 1:  freqDial = win->settings->dialCounterFreq;   break;
        case 2:  freqDial = win->settings->dialBinaryCodeFreq; break;
        case 3:  freqDial = win->settings->dialGrayCodeFreq;  break;
        case 4:  freqDial = win->settings->dialQuadratureFreq; break;
        case 5:  freqDial = win->settings->dialPrbsFreq;      break;
        case 6:  freqDial = win->settings->dialPwmFreq;       break;
        case 7:  freqDial = win->settings->dialLineCodeFreq;  break;
        case 8:  freqDial = win->settings->dial4b5bFreq;      break;
        case 9:  freqDial = win->settings->dialJohnsonFreq;   break;
        case 10: freqDial = win->settings->dialPdmFreq;       break;
        case 11: freqDial = win->settings->dialParBusFreq;    break;
        default: break;
        }
        if (!freqDial)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("No frequency dial for this pattern")}, {QStringLiteral("code"), -32602}};
        freqDial->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── Voltmeter ────────────────────────────────────────

    // voltmeter_set_mode — 0 = Normal, 1 = Fast
    m_handlers[QStringLiteral("voltmeter_set_mode")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        QWidget *w = mod->getWidget();
        if (!w) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        auto *btn = w->findChild<WidgetButtons *>(QStringLiteral("ChannelVoltmode"));
        if (!btn) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Mode buttons not found")}, {QStringLiteral("code"), -32603}};
        btn->clickedInternal(p.value(QStringLiteral("mode")).toInt(0));
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // voltmeter_set_averaging — set number of averaging samples
    m_handlers[QStringLiteral("voltmeter_set_averaging")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        QWidget *w = mod->getWidget();
        if (!w) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        auto *dial = w->findChild<WidgetDialRange *>(QStringLiteral("voltAvgSamples"));
        if (!dial) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Averaging dial not found")}, {QStringLiteral("code"), -32603}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        dial->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // voltmeter_set_calc_mode — 0 = Min/Max, 1 = Ripple, 2 = None
    m_handlers[QStringLiteral("voltmeter_set_calc_mode")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        QWidget *w = mod->getWidget();
        if (!w) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        auto *btn = w->findChild<WidgetButtons *>(QStringLiteral("buttonscalc"));
        if (!btn) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Calc mode buttons not found")}, {QStringLiteral("code"), -32603}};
        btn->clickedInternal(p.value(QStringLiteral("mode")).toInt(2));
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── get_module_state — read current GUI state of a module ──
    m_handlers[QStringLiteral("get_module_state")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        QWidget *w = mod->getWidget();
        if (!w) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module has no window (not started?)")}, {QStringLiteral("code"), -32603}};
        QJsonObject state = buildModuleState(w, p.value(QStringLiteral("module")).toString());
        state[QStringLiteral("ok")] = true;
        return state;
    };
}
