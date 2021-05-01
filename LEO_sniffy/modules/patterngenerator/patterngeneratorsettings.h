#ifndef PATTERNGENERATORSETTINGS_H
#define PATTERNGENERATORSETTINGS_H

#include <QObject>
#include <QComboBox>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetselection.h"

class PatternGeneratorSettings : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorSettings(QVBoxLayout *destination, QWidget *parent = nullptr);

    WidgetSelection *comboPatternSelection;

signals:

};

#endif // PATTERNGENERATORSETTINGS_H
