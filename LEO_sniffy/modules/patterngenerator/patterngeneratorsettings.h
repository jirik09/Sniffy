#ifndef PATTERNGENERATORSETTINGS_H
#define PATTERNGENERATORSETTINGS_H

#include <QObject>
#include <QComboBox>
#include <QVBoxLayout>
#include <QMap>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetdialrange.h"

#include "patterngeneratorconfig.h"

class PatternGeneratorSettings : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorSettings(QVBoxLayout *destination, PatternGeneratorConfig *config, QWidget *parent = nullptr);

    WidgetSelection *comboPatternSelection;

    WidgetButtons *buttonLoadUserDef;

    WidgetDialRange *dialFreq;

    void createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination);
    void createCounterComponents(QWidget *parent, QVBoxLayout *destination);
    void createGreyCodeComponents(QWidget *parent, QVBoxLayout *destination);
    void createQuadratureComponents(QWidget *parent, QVBoxLayout *destination);
    void createUartComponents(QWidget *parent, QVBoxLayout *destination);
    void createSpiComponents(QWidget *parent, QVBoxLayout *destination);
    void createI2cComponents(QWidget *parent, QVBoxLayout *destination);

    void showUserDefinedComponents(bool show);
    void showCounterComponents(bool show);
    void showGreyCodeComponents(bool show);
    void showQuadratureComponents(bool show);
    void showUartComponents(bool show);
    void showSpiComponents(bool show);
    void showI2cComponents(bool show);

private:
    PatternGeneratorConfig *config;

    QList<QString> pattList = {
        "User defined",
        "Counter",
        "Grey code",
        "Quadrature",
        "UART",
        "SPI",
        "I2C"
    };

    /* Must be in line with pattList, to avoid index mismatch
     * we may use a different approach - mapping QMap<string,funptr>.
     * Combo then would be willed by the mapped string and function
     * called by finding the string int the map. */
    void (PatternGeneratorSettings::*selectPatternComponents[7])(bool) = {
            &PatternGeneratorSettings::showUserDefinedComponents,
            &PatternGeneratorSettings::showCounterComponents,
            &PatternGeneratorSettings::showGreyCodeComponents,
            &PatternGeneratorSettings::showQuadratureComponents,
            &PatternGeneratorSettings::showUartComponents,
            &PatternGeneratorSettings::showSpiComponents,
            &PatternGeneratorSettings::showI2cComponents,   };

    /*QMap<QString, std::function<void(bool)>> optionFunction {
    {"User defined", &PatternGeneratorSettings::showUserDefinedComponents}
    };*/

private slots:
    void patternSelectionChangedCallback(int index);

signals:

};

#endif // PATTERNGENERATORSETTINGS_H
