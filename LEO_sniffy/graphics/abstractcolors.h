#ifndef ABSTRACTCOLORS_H
#define ABSTRACTCOLORS_H

#include <QObject>

class AbstractColors : public QObject
{
    Q_OBJECT
public:
    explicit AbstractColors(QObject *parent = nullptr);

    virtual QString getThemeName() = 0;
    virtual QString getAppGlobalStyle() = 0;
    virtual QString getGraphicsPath() = 0;

    virtual QString getAppBackgroundColor() = 0;
    virtual QString getDataAreaColor() = 0;
    virtual QString getControlBackgroundColor() = 0;
    virtual QString getButtonBackgroundColor() = 0;
    virtual QString getButtonDisabledBackgroundColor() = 0;
    virtual QString getTextDarkColor() = 0;
    virtual QString getTextLightColor() = 0;
    virtual QString getControlsColor() = 0;
    virtual QString getChannelColor(int channelIndex) = 0;
    virtual QString getHoverColor() = 0;

protected:

private:

signals:

};

#endif // ABSTRACTCOLORS_H
