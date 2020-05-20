#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>

class Commands
{
public:
const QString IDN = "IDN_";

/* COMMUNICATION */
// General
const QString IDN_REQUEST = "IDN?";
const QString VERSION_REQUEST = "VER?";
const QString CONFIG_REQUEST = "CFG?";
const QString IS_SHIELD_CONNECTED = "SH_?";
const QString RESET_DEVICE = "RES!";


const QString GENERATOR = "GEN_";
const QString PWM_GENERATOR = "GENP";
const QString SYNC_PWM_GEN = "SYNP";
const QString LOG_ANLYS = "LOGA";
const QString COUNTER = "CNT_";
const QString SCOPE = "OSCP";
const QString SCOPE_INCOME = "OSC_";
const QString COMMS = "COMS";
const QString SYSTEM = "SYST";

const QString GET_REAL_SMP_FREQ = "FRQ?";
const QString GET_SCOPE_INPUTS = "INP?";
};

#endif // COMMANDS_H
