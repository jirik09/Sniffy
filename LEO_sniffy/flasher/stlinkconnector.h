#ifndef STLINKCONNECTOR_H
#define STLINKCONNECTOR_H

#include <QObject>
#include <QString>

// Forward declaration
typedef struct _stlink stlink_t;

class StLinkConnector : public QObject
{
    Q_OBJECT
public:
    explicit StLinkConnector(QObject *parent = nullptr);
    ~StLinkConnector();

    bool init();
    void cleanup();
    stlink_t* handle() const;
    bool isConnected() const;

signals:
    void logMessage(const QString &msg);

private:
    stlink_t *m_stlink;
    bool loadDeviceParamsFallback();
};

#endif // STLINKCONNECTOR_H
