#ifndef ABSTRACTTHEME_H
#define ABSTRACTTHEME_H

#include <QObject>

class AbstractTheme : public QObject
{
    Q_OBJECT
public:
    explicit AbstractTheme(QObject *parent = nullptr);

    virtual QString getAppGlobalStyle() = 0;
    virtual QString getGraphicsPath() = 0;
    virtual QString getChannelColor(int channelIndex) = 0;
};

#endif // ABSTRACTTHEME_H
