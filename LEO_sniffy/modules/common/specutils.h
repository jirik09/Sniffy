#pragma once
#include <QDataStream>
#include <QString>

namespace SpecParsing {

// Remove padding underscores, replace non-printable / non-ASCII with '?'
inline QString sanitizePin(QString s) {
    s.remove('_');
    for (int i = 0; i < s.size(); ++i) {
        const QChar qc = s.at(i);
        if (qc.unicode() < 0x20 || qc.unicode() > 0x7E) {
            s.replace(i, 1, QLatin1Char('?'));
        }
    }
    return s;
}

inline bool readOnePin4(QDataStream &stream, QString &out) {
    char buf[5] = {0,0,0,0,0};
    if (stream.readRawData(buf, 4) != 4) {
        return false;
    }
    out = sanitizePin(QString::fromLatin1(buf, 4));
    return true;
}

template <typename Setter>
inline void readPins4(QDataStream &stream, int count, Setter set) {
    for (int i = 0; i < count; ++i) {
        QString pin;
        if (!readOnePin4(stream, pin)) {
            break; // premature end of data
        }
        set(i, pin);
    }
}

} // namespace SpecParsing
