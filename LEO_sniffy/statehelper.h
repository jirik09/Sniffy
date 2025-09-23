#ifndef STATEHELPER_H
#define STATEHELPER_H

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QPoint>
#include <QSize>
#include <QRect>

namespace StateHelper {

// Serialize integral (including enum) types
template<typename T,
         typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
inline QByteArray pack(T value){ return QByteArray::number(static_cast<qint64>(value)); }

inline QByteArray pack(const QString &s){ return s.toUtf8(); }
inline QByteArray pack(const QPoint &p){ return QByteArray::number(p.x()) + ',' + QByteArray::number(p.y()); }
inline QByteArray pack(const QSize &s){ return QByteArray::number(s.width()) + 'x' + QByteArray::number(s.height()); }
inline QByteArray pack(const QRect &r){ return pack(r.topLeft()) + ';' + pack(r.size()); }

// Parsing helpers return success flag via reference
inline bool unpack(const QByteArray &ba, int &out){ bool ok=false; int v = ba.toInt(&ok); if(ok){ out=v; } return ok; }
inline bool unpack(const QByteArray &ba, qreal &out){ bool ok=false; double v = ba.toDouble(&ok); if(ok){ out=v; } return ok; }
inline bool unpack(const QByteArray &ba, QString &out){ out = QString::fromUtf8(ba); return true; }

// Generic convenience wrapper: returns defaultValue if parse fails
template<typename T>
inline T unpackOr(const QByteArray &ba, const T &defaultValue){ T tmp{}; if(unpack(ba,tmp)) return tmp; return defaultValue; }

} // namespace StateHelper

#endif // STATEHELPER_H
