#ifndef WIDGETDISPLAY_H
#define WIDGETDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QLocale>

#include <QtCharts/QXYSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <QGridLayout>

#include "widgetlabel.h"

#include "GUI/clickablelabel.h"
#include "GUI/widgetchart.h"
#include "GUI/widgetlist.h"
#include "GUI/widgetdialrange.h"

#include "math/timing.h"

#include "../graphics/colors.h"
#include "../graphics/styles.h"

namespace Ui {
class WidgetDisplay;
}

class WidgetDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDisplay(QString name, QString firstLabelText, QString &unitsStyleSheet,
                           bool showPrgrssBar, int historyTracesNum = 1, int historySize = 100,
                           QWidget *parent = nullptr);
    ~WidgetDisplay();

    QString formatNumber(double val, char f, int prec);

    /* Unit styles */
    void setUnitsStyle(QString &unitsStyleSheet);
    void setAvgStyle(QString &avgStyleSheet);
    void setErrStyle(QString &errStyleSheet);
    void setQerrStyle(QString &qerrStyleSheet);
    void setTerrStyle(QString &terrStyleSheet);
    void setBarStyle(QString &barStyleSheet);

    /* Displays */
    void displayNumber(double number);
    void displayString(const QString &string);
    void displayAvgNumber(double number);
    void displayAvgString(const QString &string);
    void displayQerrNumber(double number);
    void displayQerrString(const QString &string);
    void displayTerrNumber(double number);
    void displayTerrString(const QString &string);

    void showQerrStyle(bool visible);
    void showTerrStyle(bool visible);
    void showQerrTerrStyle(bool visible);
    void showAvgDisplay(bool visible);
    void showErrDisplay(bool visible);
    void showBarDisplay(bool visible);

    void changeAvgColor(QColor color);

    /* Label area */
    void configLabel(int labelNumber, const QString text, QString colorStyle, bool visible);
    void setLabelText(int labelNumber, const QString text);
    void setLabelColor(int labelNumber, const QString &textStyleSheet);
    QString getLabelText(int labelNumber);
    void hideLabel(int labelNumber);
    void showLabel(int labelNumber);
    void drawIndicationFlag(int labelNumber, QString color);

    /* Progress bar */
    void showProgressBar(bool visible);
    void setProgressBarRange(int min, int max);
    void updateProgressBar(int value);

    /* History area */
    void setHistorySize(int smplNumber);
    void appendNewHistorySample(QString prefix, double sample, QString affix, float timeStep = 1);
    void associateSample(int traceIndex, QString prefix, double sample, QString affix);
    void updateHistoryData(QVector<QPointF> *points, int index);        

    widgetChart *chart;
    WidgetList *list;

private:
    Ui::WidgetDisplay *ui;
    QList<QLabel*> labelList;
    QPalette palette;
    QLocale loc;

    QChartView *chartView;
    QVector<QVector<QPointF>> *historyData;
    int historySize = 100;
    QString name;
    int actualHistorySize = 0;
    float rememberMax = 0;
    qreal timeAxisMax = 0;
    qreal timeAxisMin = 0;    
    int drawFlag = 0;
    QLabel *labelFloatHistNum;

    const int SPLITTER_LINE = 5;

    enum { ENABLED, DISABLED }
         historyView = DISABLED,
         listView = DISABLED,
         control = ENABLED;

    void hideHistoryChartArea();    

    void configureCustomDial();
    void configureFloatingHistoryNumber();
    void configureDialContextMenu();
    void createHistoryChart(int historyTracesNum);
    void createHistoryList();

    void clearHistoryChart();
    void clearHistoryList();
    void clearHistoryData();
    void clearExpiredPointsFromChart();
    void clearExpiredPointsFromList();
    void clearExpiredData();
    void recalcHistorySizeAndSetDial(int histSize);

    void setHistoryMinMaxTime(qreal minX, qreal maxX);
    void setHistoryMinMaxData(qreal minY, qreal maxY);        

private slots:    
    void historyButtonClickedCallback();
    void clearHistoryButtonClickedCallback();
    void listChartSwitchClickedCallback();
    void saveListClickedCallback();
    void dialHistoryValueChangedCallback(int val);
    void chartShowMenuOnRightClickCallback(const QPoint &mousePos);
    void dialShowMenuOnRightClickCallback(const QPoint &mousePos);
    void dialHistoryShowFloatingLabelCallback(QMouseEvent *me);
    void dialHistoryHideFloatingLabelCallback(QMouseEvent *me);

    void changeHistorySizeTo100();
    void changeHistorySizeTo300();
    void changeHistorySizeTo500();
    void changeHistorySizeTo700();
    void changeHistorySizeTo1000();

};

#endif // WIDGETDISPLAY_H
