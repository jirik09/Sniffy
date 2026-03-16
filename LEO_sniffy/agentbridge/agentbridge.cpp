#include "agentbridge.h"
#include "devicemediator.h"
#include "modules/abstractmodule.h"

#include <cmath>
#include <limits>

#include "modules/device/device.h"
#include "modules/device/devicewindow.h"
#include "communication/comms.h"
#include "communication/commands.h"
#include "communication/serialLine.h"
#include "GUI/widgetcontrolmodule.h"
#include "GUI/widgetbuttons.h"
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
#include "modules/scope/scopeconfig.h"
#include "modules/scope/panelsettings.h"
#include "modules/scope/panelmeasurement.h"
#include "modules/scope/measurement.h"
#include "GUI/widgettextinput.h"
#include "modules/voltagesource/voltagesourcewindow.h"
#include "modules/arbgenerator/arbgeneratorwindow.h"
#include "modules/arbgenerator/arbgenpanelsettings.h"
#include "modules/patterngenerator/patterngeneratorwindow.h"
#include "modules/patterngenerator/patterngeneratorsettings.h"
#include "modules/voltmeter/voltmeterwindow.h"
#include "modules/voltmeter/voltmeter.h"
#include "GUI/widgetselection.h"
#include <QTabWidget>

// Specification headers — needed by get_system_config
#include "modules/device/devicespec.h"
#include "modules/scope/scopespec.h"
#include "modules/counter/counterspec.h"
#include "modules/voltmeter/voltmeterspec.h"
#include "modules/syncpwm/syncpwmspec.h"
#include "modules/arbgenerator/arbgeneratorspec.h"
#include "modules/patterngenerator/patterngeneratorspec.h"
#include "modules/voltagesource/voltagesourcespec.h"
#include "resourcemanager.h"

#include <QDataStream>
#include <QDateTime>
#include <QJsonValue>
#include <QMutexLocker>
#include <QtEndian>
#include <cmath>

// ──────────────────────────────────────────────────────────
// Helper: build a JSON snapshot of module state
// ──────────────────────────────────────────────────────────

static QJsonObject buildModuleState(QWidget *widget, const QString &moduleName, AbstractModule *mod = nullptr)
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
    if (auto *sw = qobject_cast<ScopeWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Oscilloscope");
        auto *ps = sw->findChild<PanelSettings *>(QStringLiteral("panelSet"));
        if (ps) {
            st[QStringLiteral("timebase")]        = ps->dialTimeBase->getRealValue();
            st[QStringLiteral("channels")]         = ps->buttonsChannelEnable->getStatus();
            int tmIdx = ps->buttonsTriggerMode->getSelectedIndex();
            st[QStringLiteral("trigger_mode")]     = tmIdx;
            // Decode effective trigger mode name from button index + toggle text
            QString tmName;
            if (tmIdx == 2) tmName = QStringLiteral("auto");
            else if (tmIdx == 1) tmName = QStringLiteral("normal");
            else {
                // Button 0 is Stop/Single toggle — check its current text
                QString txt = ps->buttonsTriggerMode->getText(0);
                tmName = (txt == QStringLiteral("Single")) ? QStringLiteral("stop") : QStringLiteral("single");
            }
            st[QStringLiteral("trigger_mode_name")] = tmName;
            st[QStringLiteral("trigger_edge")]     = ps->buttonsTriggerEdge->getSelectedIndex();
            st[QStringLiteral("trigger_channel")]  = ps->buttonsTriggerChannel->getSelectedIndex();
            st[QStringLiteral("pretrigger")]       = ps->dialPretrigger->getRealValue();
            st[QStringLiteral("trigger_level")]    = ps->dialTriggerValue->getRealValue();
            st[QStringLiteral("memory")]           = ps->buttonsMemorySet->getSelectedIndex();
        }
        auto *pa = sw->findChild<WidgetButtons *>(QStringLiteral("scoperesolution"));
        if (pa)
            st[QStringLiteral("resolution")]   = pa->getSelectedIndex() == 0 ? 8 : 12;
        // Sampling rate + data length from ScopeConfig
        if (mod) {
            auto *scope = qobject_cast<Scope *>(mod);
            auto *cfg = scope ? scope->getConfig() : nullptr;
            if (cfg) {
                st[QStringLiteral("real_sampling_rate")] = cfg->realSamplingRate;
                st[QStringLiteral("data_length")]        = cfg->dataLength;
                // Measurement results
                QJsonArray measArr;
                for (auto *m : cfg->scopeMeasList) {
                    QJsonObject mObj;
                    mObj[QStringLiteral("label")]   = m->getLabel();
                    mObj[QStringLiteral("value")]   = m->getValue();
                    mObj[QStringLiteral("value_str")] = m->getValueString();
                    mObj[QStringLiteral("channel")] = m->getChannelIndex();
                    measArr.append(mObj);
                }
                st[QStringLiteral("measurements")] = measArr;
            }
        }
        return st;
    }

    // ── Sync PWM ──
    if (auto *w = qobject_cast<SyncPwmWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Sync PWM");
        st[QStringLiteral("step_mode")]   = w->settings->switchStepMode->isCheckedLeft() ? false : true;
        st[QStringLiteral("equidistant")] = w->settings->buttonEquidist->isChecked(0);
        QJsonArray chArr;
        for (int i = 0; i < CHANNELS_NUM; ++i) {
            QJsonObject ch;
            ch[QStringLiteral("enabled")]   = w->settings->onOffCh[i]->isCheckedLeft();
            ch[QStringLiteral("inverted")]  = w->settings->inverCh[i]->isChecked(0);
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
            // Memory mode
            static const char *memNames[] = {"best_fit", "long", "custom"};
            int memIdx = s->buttonsMemory->getSelectedIndex();
            st[QStringLiteral("memory")] = QString::fromLatin1(memNames[qBound(0, memIdx, 2)]);
            if (memIdx == 2)
                st[QStringLiteral("custom_length")] = s->customLength;
            // SW Sweep
            st[QStringLiteral("sweep_enabled")] = s->isSweepEnabled;
            if (s->isSweepEnabled) {
                st[QStringLiteral("sweep_min_freq")] = s->dialFreqSweepMin->getRealValue();
                st[QStringLiteral("sweep_max_freq")] = s->dialFreqSweepMax->getRealValue();
                st[QStringLiteral("sweep_time")]     = s->dialSweepTime->getRealValue();
            }
            QJsonArray chArr;
            for (int i = 0; i < MAX_ARB_CHANNELS_NUM; ++i) {
                QJsonObject ch;
                int si = s->buttonsShape[i]->getSelectedIndex();
                ch[QStringLiteral("shape")]      = si;
                ch[QStringLiteral("shape_name")] = s->buttonsShape[i]->getText(si).trimmed().toLower();
                ch[QStringLiteral("frequency")]  = s->dialFreqCh[i]->getRealValue();
                ch[QStringLiteral("amplitude")]  = s->dialAmplitudeCh[i]->getRealValue();
                ch[QStringLiteral("offset")]     = s->dialOffsetCh[i]->getRealValue();
                ch[QStringLiteral("duty")]       = s->dialDutyCh[i]->getRealValue();
                ch[QStringLiteral("phase")]      = s->dialPhaseCh[i]->getRealValue();
                if (i > 0)
                    ch[QStringLiteral("freq_sync_ch1")] = s->channelSyncWithCH1[i];
                // PWM-specific per-channel fields
                if (s->isPWMbased) {
                    ch[QStringLiteral("pwm_frequency")]       = s->dialPWMFreqCh[i]->getRealValue();
                    if (i > 0)
                        ch[QStringLiteral("pwm_freq_sync_ch1")] = s->channelSyncPWMWithCH1[i];
                }
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

        const auto &pattList = w->settings->getPatternList();
        if (idx >= 0 && idx < pattList.size())
            st[QStringLiteral("pattern_name")] = pattList.at(idx);

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

        // Channel count / data length dial (not all patterns have one)
        WidgetDialRange *chanDial = nullptr;
        switch (idx) {
        case 0:  chanDial = w->settings->dialUserDefLength;   break;
        case 1:  chanDial = w->settings->dialCounterLength;   break;
        case 2:  chanDial = w->settings->dialBinaryChanNum;   break;
        case 3:  chanDial = w->settings->dialGrayCodeChanNum; break;
        case 8:  chanDial = w->settings->dial4b5bGroups;      break;
        case 9:  chanDial = w->settings->dialJohnsonPhases;   break;
        case 11: chanDial = w->settings->dialParBusWidth;     break;
        default: break;
        }
        if (chanDial)
            st[QStringLiteral("channels")] = chanDial->getRealValue();

        // Per-pattern extra settings
        switch (idx) {
        case 4: // Quadrature
            if (w->settings->comboQuadratureSeqAbba)
                st[QStringLiteral("sequence")] = w->settings->comboQuadratureSeqAbba->getSelectedIndex();
            break;
        case 5: // PRBS
            if (w->settings->comboPrbsOrder)
                st[QStringLiteral("prbs_order")] = w->settings->comboPrbsOrder->getSelectedIndex();
            break;
        case 6: // PWM
            if (w->settings->dialPwmDuty)
                st[QStringLiteral("duty")] = w->settings->dialPwmDuty->getRealValue();
            break;
        case 7: // Line code
            if (w->settings->comboLineCodeType)
                st[QStringLiteral("line_code_type")] = w->settings->comboLineCodeType->getSelectedIndex();
            break;
        case 10: // PDM
            if (w->settings->dialPdmLevel)
                st[QStringLiteral("pdm_level")] = w->settings->dialPdmLevel->getRealValue();
            break;
        case 12: { // UART
            if (w->settings->comboUartBaud)
                st[QStringLiteral("baud")] = w->settings->comboUartBaud->getSelectedValue();
            if (w->settings->dialUartDataBits)
                st[QStringLiteral("data_bits")] = w->settings->dialUartDataBits->getRealValue();
            if (w->settings->comboUartParity)
                st[QStringLiteral("parity")] = w->settings->comboUartParity->getSelectedIndex();
            if (w->settings->comboUartStopBits)
                st[QStringLiteral("stop_bits")] = w->settings->comboUartStopBits->getSelectedIndex();
            if (w->settings->comboUartBitOrder)
                st[QStringLiteral("bit_order")] = w->settings->comboUartBitOrder->getSelectedIndex();
            break;
        }
        case 13: { // SPI
            if (w->settings->comboSpiMode)
                st[QStringLiteral("spi_mode")] = w->settings->comboSpiMode->getSelectedIndex();
            if (w->settings->dialSpiWordSize)
                st[QStringLiteral("word_size")] = w->settings->dialSpiWordSize->getRealValue();
            if (w->settings->comboSpiBitOrder)
                st[QStringLiteral("bit_order")] = w->settings->comboSpiBitOrder->getSelectedIndex();
            if (w->settings->comboSpiCsGating)
                st[QStringLiteral("cs_gating")] = w->settings->comboSpiCsGating->getSelectedIndex();
            if (w->settings->dialSpiPauseTicks)
                st[QStringLiteral("pause_ticks")] = w->settings->dialSpiPauseTicks->getRealValue();
            break;
        }
        case 14: { // I2C
            if (w->settings->comboI2cClockFreq)
                st[QStringLiteral("clock_freq")] = w->settings->comboI2cClockFreq->getSelectedValue();
            if (w->settings->comboI2cCommType)
                st[QStringLiteral("comm_type")] = w->settings->comboI2cCommType->getSelectedIndex();
            if (w->settings->comboI2cAddrMode)
                st[QStringLiteral("addr_mode")] = w->settings->comboI2cAddrMode->getSelectedIndex();
            if (w->settings->dialI2cAddress)
                st[QStringLiteral("address")] = w->settings->dialI2cAddress->getRealValue();
            break;
        }
        default: break;
        }

        return st;
    }

    // ── Voltmeter ──
    if (qobject_cast<VoltmeterWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Voltmeter");
        auto *mode = widget->findChild<WidgetButtons *>(QStringLiteral("ChannelVoltmode"));
        auto *avg  = widget->findChild<WidgetDialRange *>(QStringLiteral("voltAvgSamples"));
        auto *calc = widget->findChild<WidgetButtons *>(QStringLiteral("buttonscalc"));
        auto *chEn = widget->findChild<WidgetButtons *>(QStringLiteral("ChannelvoltmeterEnable"));
        if (mode) {
            int mi = mode->getSelectedIndex();
            st[QStringLiteral("mode")] = mi;
            st[QStringLiteral("mode_name")] = (mi == 1) ? QStringLiteral("fast") : QStringLiteral("normal");
        }
        if (avg)  st[QStringLiteral("averaging")]  = avg->getRealValue();
        if (calc) {
            int ci = calc->getSelectedIndex();
            st[QStringLiteral("calc_mode")] = ci;
            static const char *calcNames[] = {"min_max", "ripple", "none"};
            st[QStringLiteral("calc_mode_name")] = QString::fromLatin1(calcNames[qBound(0, ci, 2)]);
        }
        if (chEn) st[QStringLiteral("channels")]   = chEn->getStatus();
        // Data log status
        auto *vw = qobject_cast<VoltmeterWindow *>(widget);
        if (vw) st[QStringLiteral("datalog_running")] = vw->isLogging();
        // Live readings from Voltmeter model
        if (mod) {
            auto *vm = qobject_cast<Voltmeter *>(mod);
            if (vm) {
                st[QStringLiteral("vdd")] = vm->getVdd();
                st[QStringLiteral("num_channels_enabled")] = vm->getNumChannelsEnabled();
                QJsonArray readings;
                for (int i = 0; i < vm->getNumChannelsEnabled(); ++i) {
                    const auto &ch = vm->channelData(i);
                    QJsonObject chObj;
                    chObj[QStringLiteral("voltage")]   = ch.voltage;
                    chObj[QStringLiteral("min")]        = ch.min;
                    chObj[QStringLiteral("max")]        = ch.max;
                    chObj[QStringLiteral("ripple")]     = ch.ripple;
                    chObj[QStringLiteral("frequency")]  = ch.frequency;
                    chObj[QStringLiteral("percent")]    = ch.percent;
                    readings.append(chObj);
                }
                st[QStringLiteral("readings")] = readings;
            }
        }
        return st;
    }

    // ── Voltage Source ──
    if (qobject_cast<VoltageSourceWindow *>(widget)) {
        st[QStringLiteral("type")] = QStringLiteral("Voltage source");
        for (int i = 1; i <= 4; ++i) {
            auto *dial = widget->findChild<WidgetDialRange *>(QStringLiteral("voltSourceDialCH") + QString::number(i));
            if (dial && dial->isVisible())
                st[QStringLiteral("voltage_ch") + QString::number(i)] = dial->getRealValue();
        }
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

    // ── get_system_config — full hardware configuration + conflicts ──
    m_handlers[QStringLiteral("get_system_config")] = [this](const QJsonObject &) -> QJsonObject {
        if (!m_mediator->getIsConnected())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not connected")}};

        QJsonObject result;
        auto modules = m_mediator->getModulesList();

        // ── Device info ──
        for (const auto &m : modules) {
            if (qobject_cast<Device *>(m.data())) {
                auto *spec = dynamic_cast<DeviceSpec *>(m->getSpecification());
                if (spec && spec->isLoaded()) {
                    QJsonObject dev;
                    dev[QStringLiteral("name")]          = spec->device;
                    dev[QStringLiteral("mcu")]           = spec->MCU;
                    dev[QStringLiteral("core_clock")]    = static_cast<qint64>(spec->CoreClock);
                    dev[QStringLiteral("mcu_id")]        = spec->MCU_ID;
                    dev[QStringLiteral("fw_version")]    = spec->FW_Version;
                    dev[QStringLiteral("buffer_length")] = static_cast<qint64>(spec->bufferLength);
                    dev[QStringLiteral("uart_speed")]    = static_cast<qint64>(spec->uartSpeed);
                    dev[QStringLiteral("use_usb")]       = spec->useUsb;
                    result[QStringLiteral("device")] = dev;
                }
                break;
            }
        }

        // ── Per-module configuration ──
        QJsonArray modulesArr;
        for (const auto &m : modules) {
            if (!m->isAvailable()) continue;
            if (qobject_cast<Device *>(m.data())) continue; // skip Device itself

            auto *baseSpec = m->getSpecification();
            QJsonObject obj;
            obj[QStringLiteral("name")]   = m->getModuleName();
            obj[QStringLiteral("status")] = statusToString(static_cast<int>(m->getModuleStatus()));

            if (auto *s = dynamic_cast<ScopeSpec *>(baseSpec)) {
                obj[QStringLiteral("channels")]                = s->maxADCChannels;
                obj[QStringLiteral("max_sampling_rate_12b")]   = s->maxSamplingRate12B;
                obj[QStringLiteral("max_sampling_rate_8b")]    = s->maxSamplingRate8BInterleaved;
                obj[QStringLiteral("memory_size")]             = s->memorySize;
                obj[QStringLiteral("vref")]                    = s->Vref;
                QJsonArray pins;
                for (int i = 0; i < s->maxADCChannels && i < MAX_SCOPE_CHANNELS; ++i)
                    pins.append(s->channelPins[i]);
                obj[QStringLiteral("pins")] = pins;
            } else if (auto *s = dynamic_cast<CounterSpec *>(baseSpec)) {
                obj[QStringLiteral("hf_max")]     = s->hf_max;
                obj[QStringLiteral("lf_max")]     = s->lf_max;
                obj[QStringLiteral("lf_min")]     = s->lf_min;
                obj[QStringLiteral("rat_max_ref")] = s->rat_max_ref;
                obj[QStringLiteral("rat_max_chan")] = s->rat_max_chan;
                QJsonObject pins;
                pins[QStringLiteral("hf_ch1")]  = s->pins.hf_ch1;
                pins[QStringLiteral("lf_ch1")]  = s->pins.lf_ch1;
                pins[QStringLiteral("lf_ch2")]  = s->pins.lf_ch2;
                pins[QStringLiteral("rat_ref")] = s->pins.rat_ref;
                pins[QStringLiteral("rat_ch3")] = s->pins.rat_ch3;
                pins[QStringLiteral("int_ch1")] = s->pins.int_ch1;
                pins[QStringLiteral("int_ch2")] = s->pins.int_ch2;
                obj[QStringLiteral("pins")] = pins;
            } else if (auto *s = dynamic_cast<VoltmeterSpec *>(baseSpec)) {
                obj[QStringLiteral("channels")]                = s->maxADCChannels;
                obj[QStringLiteral("max_sampling_rate_12b")]   = s->maxSamplingRate12B;
                obj[QStringLiteral("max_sampling_rate_8b")]    = s->maxSamplingRate8BInterleaved;
                obj[QStringLiteral("memory_size")]             = s->memorySize;
                obj[QStringLiteral("vref")]                    = s->Vref;
                QJsonArray pins;
                for (int i = 0; i < s->maxADCChannels && i < MAX_VOLTMETER_CHANNELS; ++i)
                    pins.append(s->channelPins[i]);
                obj[QStringLiteral("pins")] = pins;
            } else if (auto *s = dynamic_cast<SyncPwmSpec *>(baseSpec)) {
                obj[QStringLiteral("channels")]      = s->chan_num;
                obj[QStringLiteral("max_freq")]      = s->max_freq;
                obj[QStringLiteral("chans_depend")]   = s->chans_depend;
                QJsonArray pins;
                pins.append(s->pins.chan1);
                pins.append(s->pins.chan2);
                pins.append(s->pins.chan3);
                pins.append(s->pins.chan4);
                obj[QStringLiteral("pins")] = pins;
            } else if (auto *s = dynamic_cast<ArbGeneratorSpec *>(baseSpec)) {
                bool isPWM = m->getModuleName().contains(QStringLiteral("PWM"), Qt::CaseInsensitive);
                if (isPWM) {
                    obj[QStringLiteral("channels")]          = s->maxPWMChannels;
                    obj[QStringLiteral("timer_clock")]       = s->periphPWMClockFrequency;
                } else {
                    obj[QStringLiteral("channels")]          = s->maxDACChannels;
                    obj[QStringLiteral("dac_resolution")]    = s->DACResolution;
                }
                obj[QStringLiteral("max_sampling_rate")]     = s->maxSamplingRate;
                obj[QStringLiteral("buffer_size")]           = s->generatorBufferSize;
                obj[QStringLiteral("range_min")]             = s->rangeMin;
                obj[QStringLiteral("range_max")]             = s->rangeMax;
                QJsonArray pins;
                int nCh = isPWM ? s->maxPWMChannels : s->maxDACChannels;
                for (int i = 0; i < nCh && i < MAX_ARB_CHANNELS_NUM; ++i)
                    pins.append(isPWM ? s->channelPWMPins[i] : s->channelPins[i]);
                obj[QStringLiteral("pins")] = pins;
            } else if (auto *s = dynamic_cast<PatternGeneratorSpec *>(baseSpec)) {
                obj[QStringLiteral("channels")]          = PATT_MAX_CHANNELS_NUM;
                obj[QStringLiteral("max_sampling_rate")]  = s->maxSamplingRate;
                QJsonArray pins;
                for (int i = 0; i < PATT_MAX_CHANNELS_NUM; ++i)
                    pins.append(s->chanPins[i]);
                obj[QStringLiteral("pins")] = pins;
            } else if (auto *s = dynamic_cast<VoltageSourceSpec *>(baseSpec)) {
                obj[QStringLiteral("channels")]       = s->maxDACChannels;
                obj[QStringLiteral("dac_resolution")] = s->DACResolution;
                obj[QStringLiteral("range_min")]      = s->rangeMin;
                obj[QStringLiteral("range_max")]      = s->rangeMax;
                QJsonArray pins;
                for (int i = 0; i < s->maxDACChannels && i < MAX_VOLTAGE_SOURCE_CHANNELS; ++i)
                    pins.append(s->channelPins[i]);
                obj[QStringLiteral("pins")] = pins;
            }

            modulesArr.append(obj);
        }
        result[QStringLiteral("modules")] = modulesArr;

        // ── Resource conflicts — which modules cannot run simultaneously ──
        QJsonArray conflicts;
        for (int i = 0; i < modules.size(); ++i) {
            if (!modules[i]->isAvailable()) continue;
            if (qobject_cast<Device *>(modules[i].data())) continue;
            ResourceSet rs_i = ResourceSet::fromModule(modules[i]);
            for (int j = i + 1; j < modules.size(); ++j) {
                if (!modules[j]->isAvailable()) continue;
                if (qobject_cast<Device *>(modules[j].data())) continue;
                ResourceSet rs_j = ResourceSet::fromModule(modules[j]);
                if (ResourceSet::collide(rs_i, rs_j)) {
                    QJsonArray pair;
                    pair.append(modules[i]->getModuleName());
                    pair.append(modules[j]->getModuleName());
                    conflicts.append(pair);
                }
            }
        }
        result[QStringLiteral("conflicts")] = conflicts;

        return result;
    };

    // ── Device management ────────────────────────────────────

    // scan_devices — probe all COM ports for Sniffy-compatible boards
    m_handlers[QStringLiteral("scan_devices")] = [this](const QJsonObject &) -> QJsonObject {
        if (m_mediator->getIsConnected())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Already connected — disconnect first")}, {QStringLiteral("code"), -32001}};
        QList<DeviceDescriptor> list;
        SerialLine::getAvailableDevices(&list, 0);
        // Update the mediator's cached list and GUI dropdown
        m_mediator->setDeviceList(list);
        Device *dev = m_mediator->getDevice();
        if (dev) dev->updateGUIDeviceList(list, false);
        QJsonArray arr;
        for (int i = 0; i < list.size(); ++i) {
            QJsonObject d;
            d[QStringLiteral("index")] = i;
            d[QStringLiteral("name")]  = list.at(i).deviceName;
            d[QStringLiteral("port")]  = list.at(i).port;
            arr.append(d);
        }
        return QJsonObject{{QStringLiteral("ok"), true}, {QStringLiteral("devices"), arr}};
    };

    // list_devices — return the last scanned device list (no re-scan)
    m_handlers[QStringLiteral("list_devices")] = [this](const QJsonObject &) -> QJsonObject {
        auto list = m_mediator->getDeviceList();
        QJsonArray arr;
        for (int i = 0; i < list.size(); ++i) {
            QJsonObject d;
            d[QStringLiteral("index")] = i;
            d[QStringLiteral("name")]  = list.at(i).deviceName;
            d[QStringLiteral("port")]  = list.at(i).port;
            arr.append(d);
        }
        return QJsonObject{{QStringLiteral("ok"), true},
                           {QStringLiteral("connected"), m_mediator->getIsConnected()},
                           {QStringLiteral("devices"), arr}};
    };

    // connect_device — connect to a device by index or name
    m_handlers[QStringLiteral("connect_device")] = [this](const QJsonObject &p) -> QJsonObject {
        if (m_mediator->getIsConnected())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Already connected — disconnect first")}, {QStringLiteral("code"), -32001}};
        auto list = m_mediator->getDeviceList();
        if (list.isEmpty())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("No devices in list — call scan_devices first")}, {QStringLiteral("code"), -32002}};
        int idx = -1;
        if (p.contains(QStringLiteral("index"))) {
            idx = p.value(QStringLiteral("index")).toInt(-1);
        } else if (p.contains(QStringLiteral("name"))) {
            QString needle = p.value(QStringLiteral("name")).toString();
            for (int i = 0; i < list.size(); ++i) {
                if (list.at(i).deviceName.contains(needle, Qt::CaseInsensitive)) { idx = i; break; }
            }
        }
        if (idx < 0 || idx >= list.size())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Device not found in list")}, {QStringLiteral("code"), -32602}};
        // Use the Device module's GUI path so buttons and dropdown update properly
        Device *dev = m_mediator->getDevice();
        if (dev) {
            auto *win = qobject_cast<DeviceWindow *>(dev->getWidget());
            if (win) {
                win->deviceSelection->setSelected(idx, true);
                win->deviceConnectButton->setText("Disconnect", 0);
                win->deviceConnectButton->setDisabledButton(true, 1);
                win->deviceSelection->setEnabled(false);
            }
        }
        QMetaObject::invokeMethod(m_mediator, "openDevice", Q_ARG(int, idx));
        return QJsonObject{{QStringLiteral("ok"), true},
                           {QStringLiteral("device"), list.at(idx).deviceName},
                           {QStringLiteral("port"), list.at(idx).port}};
    };

    // disconnect_device — close the current connection
    m_handlers[QStringLiteral("disconnect_device")] = [this](const QJsonObject &) -> QJsonObject {
        if (!m_mediator->getIsConnected())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not connected")}, {QStringLiteral("code"), -32001}};
        // Use the Device module's disconnection path to update GUI properly
        Device *dev = m_mediator->getDevice();
        if (dev) {
            auto *win = qobject_cast<DeviceWindow *>(dev->getWidget());
            if (win) {
                win->deviceConnectButton->setText("Connect", 0);
                win->deviceConnectButton->setDisabledButton(false, 1);
                win->deviceSelection->setEnabled(true);
            }
        }
        m_mediator->close();
        return QJsonObject{{QStringLiteral("ok"), true}};
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
            if (w) result[QStringLiteral("state")] = buildModuleState(w, p.value(QStringLiteral("module")).toString(), mod.data());
            return result;
        }

        // Trigger the same flow as clicking the module button in the GUI.
        // STOP = the current status → this arms, configures and starts the module.
        mod->widgetControlClicked(ModuleStatus::STOP);

        // Return current state so the agent knows the session-restored config.
        QWidget *w = mod->getWidget();
        QJsonObject result{{QStringLiteral("ok"), true}};
        if (w) {
            QJsonObject state = buildModuleState(w, p.value(QStringLiteral("module")).toString(), mod.data());
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
        // Accept bool true/false AND int 0/1 (CLI sends int, MCP sends bool)
        QJsonValue ev = p.value(QStringLiteral("enabled"));
        bool enable = ev.isBool() ? ev.toBool() : (ev.toInt(1) != 0);
        // Left = ON (index 0), Right = OFF (index 1); setLeft/setRight are private slots
        QMetaObject::invokeMethod(win->settings->onOffCh[ch],
                                  enable ? "setLeft" : "setRight", Q_ARG(bool, false));
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // spwm_set_step_mode — switch between Continuous (false) and Step (true)
    m_handlers[QStringLiteral("spwm_set_step_mode")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<SyncPwmWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Sync PWM module")}, {QStringLiteral("code"), -32602}};
        QJsonValue sv = p.value(QStringLiteral("step"));
        bool step = sv.isBool() ? sv.toBool() : (sv.toInt(0) != 0);
        // Left = Continuous (index 0), Right = Step (index 1)
        QMetaObject::invokeMethod(win->settings->switchStepMode,
                                  step ? "setRight" : "setLeft", Q_ARG(bool, false));
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // spwm_set_equidistant — toggle equidistant phase distribution
    m_handlers[QStringLiteral("spwm_set_equidistant")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<SyncPwmWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Sync PWM module")}, {QStringLiteral("code"), -32602}};
        QJsonValue ev = p.value(QStringLiteral("enabled"));
        bool want = ev.isBool() ? ev.toBool() : (ev.toInt(0) != 0);
        bool current = win->settings->buttonEquidist->isChecked(0);
        if (want != current) {
            win->settings->buttonEquidist->setChecked(want, 0);
            win->settings->buttonEquidist->clickedInternal(0);
        }
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // spwm_set_invert — toggle invert for a specific channel
    m_handlers[QStringLiteral("spwm_set_invert")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<SyncPwmWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Sync PWM module")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        if (ch < 0 || ch >= CHANNELS_NUM)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid channel (0-3)")}, {QStringLiteral("code"), -32602}};
        QJsonValue iv = p.value(QStringLiteral("enabled"));
        bool want = iv.isBool() ? iv.toBool() : (iv.toInt(0) != 0);
        bool current = win->settings->inverCh[ch]->isChecked(0);
        if (want != current) {
            win->settings->inverCh[ch]->setChecked(want, 0);
            win->settings->inverCh[ch]->clickedInternal(0);
        }
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
    // All scope handlers drive PanelSettings widgets so the GUI stays in sync
    // with the agent. The widget signals trigger ScopeWindow callbacks which
    // call Scope::update*() → MCU commands automatically.

    // Helper: find PanelSettings inside a module's widget tree
    auto findScopePS = [this](const QJsonObject &p, PanelSettings *&ps) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ScopeWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        ps = win->findChild<PanelSettings *>(QStringLiteral("panelSet"));
        if (!ps) return QJsonObject{{QStringLiteral("error"), QStringLiteral("PanelSettings not found")}, {QStringLiteral("code"), -32603}};
        return QJsonObject{};
    };

    // scope_set_timebase — set time/div (seconds), MCU reconfigured
    // WidgetDial is a discrete selector — find the closest matching index
    m_handlers[QStringLiteral("scope_set_timebase")] = [this, findScopePS](const QJsonObject &p) -> QJsonObject {
        PanelSettings *ps = nullptr;
        auto err = findScopePS(p, ps);
        if (!err.isEmpty()) return err;
        float target = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        // Save current state, scan silently, pick closest, then set for real
        int orig = ps->dialTimeBase->getSelectedIndex();
        int bestIdx = orig;
        float bestDist = std::numeric_limits<float>::max();
        for (int i = 0; i < 30; ++i) {  // max 30 options (more than enough)
            ps->dialTimeBase->setSelectedIndex(i, true);
            if (ps->dialTimeBase->getSelectedIndex() != i) break; // past end
            float dist = std::abs(static_cast<float>(ps->dialTimeBase->getRealValue()) - target);
            if (dist < bestDist) { bestDist = dist; bestIdx = i; }
        }
        ps->dialTimeBase->setSelectedIndex(bestIdx, false); // non-silent → emits signal
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_mode — auto|normal|single|stop
    // buttonsTriggerMode has 3 buttons: 0=Stop/Single(toggle), 1=Normal, 2=Auto
    m_handlers[QStringLiteral("scope_set_trigger_mode")] = [this, findScopePS](const QJsonObject &p) -> QJsonObject {
        PanelSettings *ps = nullptr;
        auto err = findScopePS(p, ps);
        if (!err.isEmpty()) return err;
        QString ms = p.value(QStringLiteral("mode")).toString().toLower();
        if (ms == QStringLiteral("auto")) {
            ps->buttonsTriggerMode->clickedInternal(2);
        } else if (ms == QStringLiteral("normal")) {
            ps->buttonsTriggerMode->clickedInternal(1);
        } else if (ms == QStringLiteral("stop")) {
            // Ensure button 0 text reads "Stop" so triggerModeCallback sees TRIG_STOP
            ps->buttonsTriggerMode->setText(QStringLiteral("Stop"), 0);
            ps->buttonsTriggerMode->clickedInternal(0);
        } else if (ms == QStringLiteral("single")) {
            // Ensure button 0 text reads "Single" so triggerModeCallback sees TRIG_SINGLE
            ps->buttonsTriggerMode->setText(QStringLiteral("Single"), 0);
            ps->buttonsTriggerMode->clickedInternal(0);
        } else {
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid mode: auto|normal|single|stop")}, {QStringLiteral("code"), -32602}};
        }
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_edge — rising|falling
    m_handlers[QStringLiteral("scope_set_trigger_edge")] = [this, findScopePS](const QJsonObject &p) -> QJsonObject {
        PanelSettings *ps = nullptr;
        auto err = findScopePS(p, ps);
        if (!err.isEmpty()) return err;
        QString es = p.value(QStringLiteral("edge")).toString().toLower();
        int idx = -1;
        if      (es == QStringLiteral("rising"))  idx = 0;
        else if (es == QStringLiteral("falling")) idx = 1;
        else return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid edge: rising|falling")}, {QStringLiteral("code"), -32602}};
        ps->buttonsTriggerEdge->clickedInternal(idx);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_channel — 0-based channel index
    m_handlers[QStringLiteral("scope_set_trigger_channel")] = [this, findScopePS](const QJsonObject &p) -> QJsonObject {
        PanelSettings *ps = nullptr;
        auto err = findScopePS(p, ps);
        if (!err.isEmpty()) return err;
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        ps->buttonsTriggerChannel->clickedInternal(ch);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_channels — enable channels by bitmask (bit 0 = CH1, etc.)
    m_handlers[QStringLiteral("scope_set_channels")] = [this, findScopePS](const QJsonObject &p) -> QJsonObject {
        PanelSettings *ps = nullptr;
        auto err = findScopePS(p, ps);
        if (!err.isEmpty()) return err;
        int mask = p.value(QStringLiteral("channel_mask")).toInt(1);
        // Set individual CHECKABLE button states, then fire one clickedInternal to emit statusChanged
        for (int i = 0; i < 4; ++i)
            ps->buttonsChannelEnable->setChecked((mask >> i) & 1, i);
        ps->buttonsChannelEnable->clickedInternal(0);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_pretrigger — pretrigger percentage (0-100)
    m_handlers[QStringLiteral("scope_set_pretrigger")] = [this, findScopePS](const QJsonObject &p) -> QJsonObject {
        PanelSettings *ps = nullptr;
        auto err = findScopePS(p, ps);
        if (!err.isEmpty()) return err;
        float pct = static_cast<float>(p.value(QStringLiteral("percent")).toDouble(50.0));
        ps->dialPretrigger->setRealValue(pct, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_trigger_level — trigger level percentage (0-100)
    m_handlers[QStringLiteral("scope_set_trigger_level")] = [this, findScopePS](const QJsonObject &p) -> QJsonObject {
        PanelSettings *ps = nullptr;
        auto err = findScopePS(p, ps);
        if (!err.isEmpty()) return err;
        float pct = static_cast<float>(p.value(QStringLiteral("percent")).toDouble(50.0));
        ps->dialTriggerValue->setRealValue(pct, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_resolution — ADC resolution (8 or 12 bits)
    m_handlers[QStringLiteral("scope_set_resolution")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ScopeWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        auto *resBtn = win->findChild<WidgetButtons *>(QStringLiteral("scoperesolution"));
        if (!resBtn)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Resolution buttons not found")}, {QStringLiteral("code"), -32603}};
        int bits = p.value(QStringLiteral("bits")).toInt(12);
        // buttonIndex: 0=8-bit, 1=12-bit
        resBtn->clickedInternal(bits == 8 ? 0 : 1);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_sampling_freq — set custom sampling frequency (Hz)
    m_handlers[QStringLiteral("scope_set_sampling_freq")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ScopeWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        auto *input = win->findChild<WidgetTextInput *>(QStringLiteral("scopeSamplingFreq"));
        if (!input) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Sampling frequency input not found")}, {QStringLiteral("code"), -32603}};
        int freq = p.value(QStringLiteral("value")).toInt(100000);
        input->setText(QString::number(freq));
        input->processInput();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_set_data_length — set custom data length (samples)
    m_handlers[QStringLiteral("scope_set_data_length")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ScopeWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        auto *input = win->findChild<WidgetTextInput *>(QStringLiteral("scopeDataLength"));
        if (!input) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Data length input not found")}, {QStringLiteral("code"), -32603}};
        int len = p.value(QStringLiteral("value")).toInt(1000);
        input->setText(QString::number(len));
        input->processInput();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_add_measurement — add a measurement (type + channel)
    // Types: frequency, period, phase, duty, low, high, rms, rms_ac, mean, pkpk, max, min
    m_handlers[QStringLiteral("scope_add_measurement")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        QString typeStr = p.value(QStringLiteral("type")).toString().toLower();
        int channel = p.value(QStringLiteral("channel")).toInt(0);
        // Map string to MeasurementType
        static const QHash<QString, MeasurementType> typeMap = {
            {QStringLiteral("frequency"), MeasurementType::FREQUENCY},
            {QStringLiteral("period"),    MeasurementType::PERIOD},
            {QStringLiteral("phase"),     MeasurementType::PHASE},
            {QStringLiteral("duty"),      MeasurementType::DUTY},
            {QStringLiteral("low"),       MeasurementType::LOW},
            {QStringLiteral("high"),      MeasurementType::HIGH},
            {QStringLiteral("rms"),       MeasurementType::RMS},
            {QStringLiteral("rms_ac"),    MeasurementType::RMS_AC},
            {QStringLiteral("mean"),      MeasurementType::MEAN},
            {QStringLiteral("pkpk"),      MeasurementType::PKPK},
            {QStringLiteral("max"),       MeasurementType::MAX},
            {QStringLiteral("min"),       MeasurementType::MIN},
        };
        auto it = typeMap.find(typeStr);
        if (it == typeMap.end())
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid type. Use: frequency|period|phase|duty|low|high|rms|rms_ac|mean|pkpk|max|min")}, {QStringLiteral("code"), -32602}};
        // For phase measurement, channel encodes both channels as chA*10+chB
        int chIdx = channel;
        if (*it == MeasurementType::PHASE) {
            int chB = p.value(QStringLiteral("channel_b")).toInt(1);
            chIdx = channel * 10 + chB;
        }
        auto *m = new Measurement(*it, chIdx);
        scope->addMeasurement(m);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_clear_measurements — remove all measurements
    m_handlers[QStringLiteral("scope_clear_measurements")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        scope->clearMeasurement();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // scope_get_measurements — read current measurement results
    m_handlers[QStringLiteral("scope_get_measurements")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *scope = qobject_cast<Scope *>(mod.data());
        if (!scope) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Scope module")}, {QStringLiteral("code"), -32602}};
        auto *cfg = scope->getConfig();
        QJsonArray arr;
        for (auto *m : cfg->scopeMeasList) {
            QJsonObject mObj;
            mObj[QStringLiteral("label")]     = m->getLabel();
            mObj[QStringLiteral("value")]     = m->getValue();
            mObj[QStringLiteral("value_str")] = m->getValueString();
            mObj[QStringLiteral("channel")]   = m->getChannelIndex();
            arr.append(mObj);
        }
        return QJsonObject{{QStringLiteral("ok"), true}, {QStringLiteral("measurements"), arr}};
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
        auto *dial = win->findChild<WidgetDialRange *>(QStringLiteral("voltSourceDialCH") + QString::number(ch + 1));
        if (!dial)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Channel dial not found")}, {QStringLiteral("code"), -32603}};
        dial->setRealValue(static_cast<float>(voltage), false);
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

    // arbgen_set_memory — 0=Best fit, 1=Long, 2=Custom
    m_handlers[QStringLiteral("arbgen_set_memory")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int mode = p.value(QStringLiteral("mode")).toInt(0);
        settings->buttonsMemory->clickedInternal(mode);
        if (mode == 2) { // Custom — also set the length if provided
            double len = p.value(QStringLiteral("length")).toDouble(0);
            if (len > 0) {
                settings->customLengthInput->setText(QString::number(static_cast<int>(len)));
                settings->customLengthInput->processInput();
            }
        }
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_sweep — enable/disable SW sweep and optionally set min/max freq + time
    m_handlers[QStringLiteral("arbgen_set_sweep")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        bool enable = p.value(QStringLiteral("enable")).toBool(false);
        settings->buttonSWSweepEnable->clickedInternal(enable ? 1 : 0);
        if (enable) {
            if (p.contains(QStringLiteral("min_freq")))
                settings->dialFreqSweepMin->setRealValue(static_cast<float>(p.value(QStringLiteral("min_freq")).toDouble()), false);
            if (p.contains(QStringLiteral("max_freq")))
                settings->dialFreqSweepMax->setRealValue(static_cast<float>(p.value(QStringLiteral("max_freq")).toDouble()), false);
            if (p.contains(QStringLiteral("sweep_time")))
                settings->dialSweepTime->setRealValue(static_cast<float>(p.value(QStringLiteral("sweep_time")).toDouble()), false);
        }
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_freq_sync — set CH1 freq sync on/off for a channel (CH2-CH4)
    m_handlers[QStringLiteral("arbgen_set_freq_sync")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Settings panel not found")}, {QStringLiteral("code"), -32603}};
        int ch = p.value(QStringLiteral("channel")).toInt(1);
        if (ch < 1 || ch >= MAX_ARB_CHANNELS_NUM || !settings->swSyncWithCH1[ch])
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid channel for freq sync (must be 1-3, i.e. CH2-CH4)")}, {QStringLiteral("code"), -32602}};
        bool on = p.value(QStringLiteral("enabled")).toBool(false);
        QMetaObject::invokeMethod(settings->swSyncWithCH1[ch],
                                  on ? "setRight" : "setLeft",
                                  Qt::DirectConnection);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_pwm_frequency — set per-channel PWM carrier frequency (PWM generator only)
    m_handlers[QStringLiteral("arbgen_set_pwm_frequency")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen/PWM module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings || !settings->isPWMbased)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("PWM frequency only available on PWM generator")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(0);
        if (ch < 0 || ch >= MAX_ARB_CHANNELS_NUM)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Channel out of range")}, {QStringLiteral("code"), -32602}};
        double val = p.value(QStringLiteral("value")).toDouble();
        settings->dialPWMFreqCh[ch]->setRealValue(static_cast<float>(val), false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // arbgen_set_pwm_freq_sync — sync PWM freq of CH2-CH4 with CH1 (PWM generator only)
    m_handlers[QStringLiteral("arbgen_set_pwm_freq_sync")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<ArbGeneratorWindow *>(mod->getWidget());
        if (!win) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not an ArbGen/PWM module")}, {QStringLiteral("code"), -32602}};
        auto *settings = win->findChild<ArbGenPanelSettings *>();
        if (!settings || !settings->isPWMbased)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("PWM freq sync only available on PWM generator")}, {QStringLiteral("code"), -32602}};
        int ch = p.value(QStringLiteral("channel")).toInt(1);
        if (ch < 1 || ch >= MAX_ARB_CHANNELS_NUM || !settings->swSyncPWMWithCH1[ch])
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Invalid channel for PWM freq sync (must be 1-3, i.e. CH2-CH4)")}, {QStringLiteral("code"), -32602}};
        bool on = p.value(QStringLiteral("enabled")).toBool(false);
        QMetaObject::invokeMethod(settings->swSyncPWMWithCH1[ch],
                                  on ? "setRight" : "setLeft",
                                  Qt::DirectConnection);
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

    // patgen_set_channels — set channel count / data length dial for the current pattern
    m_handlers[QStringLiteral("patgen_set_channels")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<PatternGeneratorWindow *>(mod->getWidget());
        if (!win || !win->settings)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Pattern Generator module")}, {QStringLiteral("code"), -32602}};
        float val = static_cast<float>(p.value(QStringLiteral("value")).toDouble());
        int pattern = win->settings->comboPatternSelection->getSelectedIndex();
        WidgetDialRange *chanDial = nullptr;
        switch (pattern) {
        case 0:  chanDial = win->settings->dialUserDefLength;  break;
        case 1:  chanDial = win->settings->dialCounterLength;  break;
        case 2:  chanDial = win->settings->dialBinaryChanNum;  break;
        case 3:  chanDial = win->settings->dialGrayCodeChanNum; break;
        case 8:  chanDial = win->settings->dial4b5bGroups;     break;
        case 9:  chanDial = win->settings->dialJohnsonPhases;  break;
        case 11: chanDial = win->settings->dialParBusWidth;    break;
        default: break;
        }
        if (!chanDial)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("No channel/length dial for this pattern")}, {QStringLiteral("code"), -32602}};
        chanDial->setRealValue(val, false);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // patgen_reset — reset current pattern to defaults
    m_handlers[QStringLiteral("patgen_reset")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *win = qobject_cast<PatternGeneratorWindow *>(mod->getWidget());
        if (!win || !win->settings || !win->settings->buttonSetDefault)
            return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Pattern Generator module")}, {QStringLiteral("code"), -32602}};
        win->settings->buttonSetDefault->clickedInternal(0);
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

    // voltmeter_set_channels — enable channels by bitmask
    m_handlers[QStringLiteral("voltmeter_set_channels")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        QWidget *w = mod->getWidget();
        if (!w) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        auto *btn = w->findChild<WidgetButtons *>(QStringLiteral("ChannelvoltmeterEnable"));
        if (!btn) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Channel enable buttons not found")}, {QStringLiteral("code"), -32603}};
        int mask = p.value(QStringLiteral("channel_mask")).toInt(1);
        for (int i = 0; i < 4; ++i)
            btn->setChecked((mask >> i) & 1, i);
        btn->clickedInternal(0);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // voltmeter_get_readings — read live voltage data
    m_handlers[QStringLiteral("voltmeter_get_readings")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *vm = qobject_cast<Voltmeter *>(mod.data());
        if (!vm) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        QJsonArray readings;
        for (int i = 0; i < vm->getNumChannelsEnabled(); ++i) {
            const auto &ch = vm->channelData(i);
            QJsonObject chObj;
            chObj[QStringLiteral("voltage")]   = ch.voltage;
            chObj[QStringLiteral("min")]        = ch.min;
            chObj[QStringLiteral("max")]        = ch.max;
            chObj[QStringLiteral("ripple")]     = ch.ripple;
            chObj[QStringLiteral("frequency")]  = ch.frequency;
            chObj[QStringLiteral("percent")]    = ch.percent;
            readings.append(chObj);
        }
        return QJsonObject{{QStringLiteral("ok"), true}, {QStringLiteral("vdd"), vm->getVdd()}, {QStringLiteral("readings"), readings}};
    };

    // voltmeter_set_datalog_file — set data log file path (no GUI dialog)
    m_handlers[QStringLiteral("voltmeter_set_datalog_file")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *vw = qobject_cast<VoltmeterWindow *>(mod->getWidget());
        if (!vw) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        QString path = p.value(QStringLiteral("path")).toString();
        if (path.isEmpty()) return QJsonObject{{QStringLiteral("error"), QStringLiteral("path is required")}, {QStringLiteral("code"), -32602}};
        vw->setDatalogFilePath(path);
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // voltmeter_start_datalog — start data logging
    m_handlers[QStringLiteral("voltmeter_start_datalog")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *vw = qobject_cast<VoltmeterWindow *>(mod->getWidget());
        if (!vw) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        if (vw->isLogging()) return QJsonObject{{QStringLiteral("ok"), true}, {QStringLiteral("info"), QStringLiteral("already logging")}};
        vw->startDatalog();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // voltmeter_stop_datalog — stop data logging
    m_handlers[QStringLiteral("voltmeter_stop_datalog")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        auto *vw = qobject_cast<VoltmeterWindow *>(mod->getWidget());
        if (!vw) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Not a Voltmeter module")}, {QStringLiteral("code"), -32602}};
        if (!vw->isLogging()) return QJsonObject{{QStringLiteral("ok"), true}, {QStringLiteral("info"), QStringLiteral("not logging")}};
        vw->stopDatalog();
        return QJsonObject{{QStringLiteral("ok"), true}};
    };

    // ── get_module_state — read current GUI state of a module ──
    m_handlers[QStringLiteral("get_module_state")] = [this](const QJsonObject &p) -> QJsonObject {
        auto mod = findModule(p.value(QStringLiteral("module")).toString());
        if (!mod) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module not found")}, {QStringLiteral("code"), -32602}};
        QWidget *w = mod->getWidget();
        if (!w) return QJsonObject{{QStringLiteral("error"), QStringLiteral("Module has no window (not started?)")}, {QStringLiteral("code"), -32603}};
        QJsonObject state = buildModuleState(w, p.value(QStringLiteral("module")).toString(), mod.data());
        state[QStringLiteral("ok")] = true;
        return state;
    };
}
