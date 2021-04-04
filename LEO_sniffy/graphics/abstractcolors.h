#ifndef ABSTRACTCOLORS_H
#define ABSTRACTCOLORS_H

#include <QObject>

class AbstractColors : public QObject
{
    Q_OBJECT
public:
    explicit AbstractColors(QObject *parent = nullptr);

    virtual QColor getAppBackgroundColor() = 0;
    virtual QString getAppBackgroundColorString() = 0;

    virtual QColor getDataAreaColor() = 0;
    virtual QString getDataAreaColorString() = 0;

    virtual QColor getControlBackgroundColor() = 0;
    virtual QString getControlBackgroundColorString() = 0;

    virtual QColor getButtonBackgroundColor() = 0;
    virtual QString getButtonBackgroundColorString() = 0;

    virtual QColor getButtonDisabledBackgroundColor() = 0;
    virtual QString getButtonDisabledBackgroundColorString() = 0;

    virtual QColor getTextDarkColor() = 0;
    virtual QString getTextDarkColorString() = 0;

    virtual QColor getTextLightColor() = 0;
    virtual QString getTextLightColorString() = 0;

    virtual QColor getControlsColor() = 0;
    virtual QString getControlsColorString() = 0;

    virtual QColor getChannelColor(int channelIndex) = 0;
    virtual QString getChannelColorString(int channelIndex) = 0;

protected:

private:

signals:

};

#endif // ABSTRACTCOLORS_H
