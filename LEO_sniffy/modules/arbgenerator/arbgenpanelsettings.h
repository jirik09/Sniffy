#ifndef ARBGENPANELSETTINGS_H
#define ARBGENPANELSETTINGS_H

#include <QObject>
#include <QVBoxLayout>
#include <QScrollArea>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgettextinput.h"
#include "../../GUI/widgetlabel.h"

#include "../labelformator.h"

#include "arbgeneratordefs.h"
#include "arbgeneratorconfig.h"



class ArbGenPanelSettings : public QObject
{
    Q_OBJECT
public:
    explicit ArbGenPanelSettings(QVBoxLayout *destination, QWidget *parent = nullptr);

    void setChannelShown(int index, bool isShown);
    void setChannelShapeControl(int index, int shape);
    void restoreGUI();
    void setMaxNumChannels(int numChannels);
    void setLabels(QString freq, QString length, int index);
    void setFreqLabel(QString freq, int index);

signals:

public:

    //common
    WidgetButtons *buttonsGenerate;
    WidgetButtons *buttonsEnable;
    WidgetButtons *buttonsMemory;
    WidgetTextInput *customLengthInput;

    int numChannelsEnabled = 0;
    MemoryLength memorySet = MemoryLength::BEST_FIT;
    int customLength = 0;

    //for each channel
    QScrollArea *verChannArea[MAX_ARB_CHANNELS_NUM];
    WidgetButtons *buttonsShape[MAX_ARB_CHANNELS_NUM];
    WidgetButtons *buttonSelectFile[MAX_ARB_CHANNELS_NUM];
    WidgetLabel *labelFile[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialFreqCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialOffsetCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialAmplitudeCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialDutyCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialPhaseCh[MAX_ARB_CHANNELS_NUM];
    WidgetSwitch *syncwithCh1[MAX_ARB_CHANNELS_NUM];
    WidgetLabel *labelRealFreq[MAX_ARB_CHANNELS_NUM];
    WidgetLabel *labelDataLength[MAX_ARB_CHANNELS_NUM];
    WidgetSwitch *swSyncWithCH1[MAX_ARB_CHANNELS_NUM];

    SignalShape signalShape[MAX_ARB_CHANNELS_NUM];
    bool channelEnabled[MAX_ARB_CHANNELS_NUM];
    bool channelSyncWithCH1[MAX_ARB_CHANNELS_NUM];


private:
    void setCopyFreq(int fromCh, int toCh);

signals:
    void signalChanged();

private slots:
    void buttonEnableChannelCallback(int index);
    void buttonShapeCallback(int clicked, int channel);
    void syncWithCH1Callback(int clicked, int channel);
    void signalFrequencyCallback(qreal value, int channel);
    void signalChangedCallback();
    void memoryCallback(int index);
    void customLenghtCallback(qreal value);
};

#endif // ARBGENPANELSETTINGS_H
