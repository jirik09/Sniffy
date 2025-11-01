#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <QDebug>

// Simple build-time toggles (could later map to CMake/qmake DEFINES)
#ifndef SNIFFY_DIAG_LEVEL
#define SNIFFY_DIAG_LEVEL 1
#endif

// Levels: 0=off, 1=errors, 2=warn, 3=info, 4=verbose
#if SNIFFY_DIAG_LEVEL >= 1
#define SNIFFY_LOG_ERROR(msg)   qCritical().noquote() << "[ERR]" << msg
#else
#define SNIFFY_LOG_ERROR(msg)   do{}while(false)
#endif

#if SNIFFY_DIAG_LEVEL >= 2
#define SNIFFY_LOG_WARN(msg)    qWarning().noquote() << "[WRN]" << msg
#else
#define SNIFFY_LOG_WARN(msg)    do{}while(false)
#endif

#if SNIFFY_DIAG_LEVEL >= 3
#define SNIFFY_LOG_INFO(msg)    qInfo().noquote() << "[INF]" << msg
#else
#define SNIFFY_LOG_INFO(msg)    do{}while(false)
#endif

#if SNIFFY_DIAG_LEVEL >= 4
#define SNIFFY_LOG_VERBOSE(msg) qDebug().noquote() << "[VRB]" << msg
#else
#define SNIFFY_LOG_VERBOSE(msg) do{}while(false)
#endif

// Helper macro for guarded early returns
#define SNIFFY_GUARD(cond, msg) \
    do { if(cond){ SNIFFY_LOG_WARN(msg); return; } } while(false)

#endif // DIAGNOSTICS_H
