#ifndef PINFUNCTIONINFO_H
#define PINFUNCTIONINFO_H

#include <QString>
#include <QList>
#include <QMetaType>

// Describes the Sniffy function assigned to a board pin.
// `pin` matches the pin name string sent by firmware (e.g. "A5", "D13", "PB14").
// `label` is the channel label displayed on the pinout (e.g. "CH1", "PWM CH2").
// `moduleName` must match the icon filename convention: icon_<moduleName>.png
//   Known values: "scope", "counter", "signal_generator", "pwm_gen",
//                 "pattern_generator", "syncpwm"
struct PinFunctionInfo {
    QString pin;
    QString label;
    QString moduleName;
};

Q_DECLARE_METATYPE(PinFunctionInfo)
Q_DECLARE_METATYPE(QList<PinFunctionInfo>)

#endif // PINFUNCTIONINFO_H
