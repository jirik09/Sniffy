#pragma once
#include <QDataStream>
#include <QString>

namespace SpecParsing {

// Remove padding underscores, replace non-printable / non-ASCII with '?'
inline QString sanitizePin(QString s) {
    // Replace non-printable chars with nothing (strip) but keep valid ASCII alphanumerics
    QString cleaned;
    cleaned.reserve(s.size());
    for(QChar qc : s){
        ushort u = qc.unicode();
        if(u < 0x20 || u > 0x7E) continue; // skip non-printable/out-of-range
        cleaned.append(qc);
    }
    // Trim only trailing underscores (common padding) not internal ones
    while(cleaned.endsWith('_')) cleaned.chop(1);
    if(cleaned.isEmpty()) return QStringLiteral("-");
    return cleaned;
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
