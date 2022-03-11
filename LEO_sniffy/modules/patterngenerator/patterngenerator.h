#ifndef PATTERNGENERATOR_H
#define PATTERNGENERATOR_H

#include <QObject>

#include "../abstractmodule.h"
#include "../arbgenerator/gencommons.h"
#include "patterngeneratorspec.h"
#include "patterngeneratorconfig.h"
#include "patterngeneratorwindow.h"

class PatternGenerator : public AbstractModule
{
    Q_OBJECT
public:
    explicit PatternGenerator(QObject *parent = nullptr);
    QWidget* getWidget();

signals:


public slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();
    void startModule();
    void stopModule();

private:
    PatternGeneratorConfig *config;
    PatternGeneratorWindow *pattGenWindow;   
    GenCommons *genComms;    

    bool dataBeingUploaded = false;

    void startGenerator();
    void stopGenerator();
    void buildModuleDescription(PatternGeneratorSpec *spec);

    void startPatternUpload();
    void dataTransferNext();
    void dataTransferFinished();

private slots:
    void startGeneratorCallback();
    void stopGeneratorCallback();
};

#endif // PATTERNGENERATOR_H
