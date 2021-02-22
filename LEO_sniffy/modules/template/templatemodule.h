#ifndef TEMPLATEMODULE_H
#define TEMPLATEMODULE_H

#include <QObject>

#include "../abstractmodule.h"
#include "templatemodulespec.h"
#include "templatemoduleconfig.h"
#include "templatemodulewindow.h"

class TemplateModule : public AbstractModule
{
    Q_OBJECT
public:
    explicit TemplateModule(QObject *parent = nullptr);
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
    //In case hold is needed

    void showHoldButtonCallback();
    void holdButtonCallback(bool held);


private:
    TemplateModuleConfig *config;
    TemplateModuleWindow *tempWindow;
   // scpWindow = new ScopeWindow();

};

#endif // TEMPLATEMODULE_H
