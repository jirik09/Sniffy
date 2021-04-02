#ifndef COLORS_H
#define COLORS_H

#include <QObject>
#include <QColor>

//backgrounds
#define BACKGROUND_QCOLOR_DATA_AREA QColor(38,38,38)
#define BACKGROUND_QCOLOR_BUTTON QColor(38,38,38)
#define BACKGROUND_QCOLOR_CONTROL QColor(58,58,58)
#define BACKGROUND_QCOLOR_APP QColor(48,48,48)
#define BACKGROUND_QCOLOR_BUTTON_DISABLED QColor(48,48,48)

#define BACKGROUND_COLOR_DATA_AREA "rgb(38,38,38);"
#define BACKGROUND_COLOR_BUTTON "rgb(38,38,38);"
#define BACKGROUND_COLOR_CONTROL "rgb(58,58,58);"
#define BACKGROUND_COLOR_APP "rgb(48,48,48);"
#define BACKGROUND_COLOR_FOCUS_IN "rgb(48,48,48);"
#define BACKGROUND_COLOR_BUTTON_DISABLED "rgb(48,48,48);"

//Text colors
#define COLOR_GREY   "rgb(124,124,124);"
#define COLOR_DARK_GREY   "rgb(87,87,87);"
#define COLOR_WHITE  "rgb(214,214,214);"
#define QCOLOR_GREY   QColor(124,124,124)
#define QCOLOR_DARK_GREY   QColor(87,87,87)
#define QCOLOR_WHITE  QColor(214,214,214)

//Special purpose color
#define COLOR_HOVER "rgb(71,76,94);"

//Common
#define COLOR_BLACK "rgb(0,0,0);"
#define COLOR_FULL_WHITE "rgb(255,255,255);"

#define COLOR_BLUE   "rgb(98,177,208);"
#define COLOR_RED "rgb(219,88,77);"
#define COLOR_YELLOW "rgb(240,214,76);"
#define COLOR_GREEN  "rgb(129,166,91);"
#define COLOR_PURPLE "rgb(205,114,192);"
#define COLOR_BROWN "rgb(185,136,93);"
#define COLOR_ORANGE "rgb(239,161,57);"
#define COLOR_GREEN_OIL "rgb(94,152,133);"
#define COLOR_DARK_BLUE "rgb(99,122,172);"

#define QCOLOR_BLACK QColor(0,0,0)
#define QCOLOR_FULL_WHITE QColor(255,255,255)

#define QCOLOR_BLUE   QColor(98,177,208)
#define QCOLOR_RED    QColor(219,88,77)
#define QCOLOR_YELLOW QColor(240,214,76)
#define QCOLOR_GREEN  QColor(129,166,91)
#define QCOLOR_PURPLE QColor(205,114,192)
#define QCOLOR_BROWN QColor(185,136,93)
#define QCOLOR_ORANGE QColor(239,161,57)
#define QCOLOR_GREEN_OIL QColor(94,152,133)
#define QCOLOR_DARK_BLUE QColor(99,122,172)

// Channel colors
#define CHANNEL_1_COLOR COLOR_BLUE
#define CHANNEL_2_COLOR COLOR_RED
#define CHANNEL_3_COLOR COLOR_YELLOW
#define CHANNEL_4_COLOR COLOR_GREEN
#define CHANNEL_5_COLOR COLOR_PURPLE
#define CHANNEL_6_COLOR COLOR_BROWN
#define CHANNEL_7_COLOR COLOR_ORANGE
#define CHANNEL_8_COLOR COLOR_GREEN_OIL
#define CHANNEL_9_COLOR COLOR_DARK_BLUE

#define CHANNEL_1_QCOLOR QCOLOR_BLUE
#define CHANNEL_2_QCOLOR QCOLOR_RED
#define CHANNEL_3_QCOLOR QCOLOR_YELLOW
#define CHANNEL_4_QCOLOR QCOLOR_GREEN
#define CHANNEL_5_QCOLOR QCOLOR_PURPLE
#define CHANNEL_6_QCOLOR QCOLOR_BROWN
#define CHANNEL_7_QCOLOR QCOLOR_ORANGE
#define CHANNEL_8_QCOLOR QCOLOR_GREEN_OIL
#define CHANNEL_9_QCOLOR QCOLOR_DARK_BLUE

//extend colors
//#define COLOR_RED "rgb(224,24,0);"
//#define COLOR_BROWN "rgb(170,105,39);"
//#define COLOR_DARK_BLUE "rgb(24,0,185);"
//#define COLOR_YELLOW "rgb(255,217,15);"
//#define COLOR_LIGHT_GREEN "rgb(24,224,0);"
//#define COLOR_DARK_CYAN "rgb(30,170,100);"
//#define COLOR_PINK "rgb(255,18,108);"

//#define QCOLOR_RED QColor(224,24,0)
//#define QCOLOR_BROWN QColor(170,105,39)
//#define QCOLOR_DARK_BLUE QColor(24,0,185)
//#define QCOLOR_YELLOW QColor(255,217,15)
//#define QCOLOR_LIGHT_GREEN QColor(24,224,0)
//#define QCOLOR_DARK_CYAN QColor(30,170,100)
//#define QCOLOR_PINK QColor(255,18,108)

// basic Channel colors
//#define COLOR_BLUE   "rgb(24,154,224);"
//#define COLOR_ORANGE "rgb(255,154,0);"
//#define COLOR_GREEN  "rgb(54,154,24);"
//#define COLOR_PURPLE "rgb(185,24,224);"
//#define QCOLOR_BLUE   QColor(24,154,224)
//#define QCOLOR_ORANGE QColor(255,154,0)
//#define QCOLOR_GREEN  QColor(54,154,24)
//#define QCOLOR_PURPLE QColor(185,24,224)

class Colors : public QObject
{
    Q_OBJECT
public:
    explicit Colors(QObject *parent = nullptr);
    static QColor getChannelColor(int channelIndex);
    static QByteArray getChannelColorString(int channelIndex);
};

#endif // COLORS_H
