#ifndef ARBGENERATOR_H
#define ARBGENERATOR_H

#include <QObject>
#include <QtEndian>

#include "../abstractmodule.h"
#include "arbgeneratorspec.h"
#include "arbgeneratorconfig.h"
#include "arbgeneratorwindow.h"
#include "arbgeneratordefs.h"
#include "gencommons.h"


class ArbGenerator : public AbstractModule
{
    Q_OBJECT
public:
    explicit ArbGenerator(QObject *parent = nullptr, bool isPWMbased = false);
    QWidget* getWidget();

signals:


public slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();
    void startModule();
    void stopModule();


private slots:
    void sendSignalCallback();
    void stopCallback();
    void updateFrequencyCallback();
    void quickRestartCalback();

private:
    ArbGeneratorConfig *config;
    ArbGeneratorWindow *arbGenWindow;
    GenCommons *genComms;
    bool isPWMbased = false;

    int numChannelsUsed;
    bool dataBeingUploaded = false;

    void buildModuleDescription(ArbGeneratorSpec *spec);

    void startGenerator ();
    void stopGenerator ();
};

#endif // ARBGENERATOR_H
