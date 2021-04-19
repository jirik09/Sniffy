#include "countertabintervals.h"

CounterTabIntervals::CounterTabIntervals(QVBoxLayout *destination, CounterConfig *conf, QWidget *parent) :
    QObject(parent),
    conf(conf)
{
    WidgetSeparator *separatorStart = new WidgetSeparator(parent,"Let's roll");
    destination->addWidget(separatorStart);

    buttonStart = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonStart->setObjectName("btnStart");
    buttonStart->setText("Start", 0);
    destination->addWidget(buttonStart);

    dialTimeout = new WidgetDialRange(parent ,"Timeout");
    dialTimeout->setObjectName("dialIntTimeout");
    dialTimeout->setRange(1, INTERVAL_TIMEOUT_MAX, "Sec", 1, 1, INTERVAL_TIMEOUT_DEFAULT, false, 0);
    dialTimeout->setColor(Graphics::COLOR_CONTROLS);
    dialTimeout->hideUnitSelection();    
    destination->addWidget(dialTimeout);

    WidgetSeparator *separatorEventSequence = new WidgetSeparator(parent,"Events sequence");
    destination->addWidget(separatorEventSequence);

    buttonsEventsSeq = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsEventsSeq->setObjectName("evetnSeq");
    buttonsEventsSeq->setText("A -> B", 0);
    buttonsEventsSeq->setText("B -> A", 1);
    destination->addWidget(buttonsEventsSeq);

    horizontalLayout_switchArea = new QHBoxLayout();
    destination->addLayout(horizontalLayout_switchArea);

    switchEdgeEventA = new WidgetSwitch(parent, "Rising", "Falling", "");
    switchEdgeEventA->setObjectName("edgeEventA");
    switchEdgeEventA->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    switchEdgeEventA->setColor(Graphics::COLOR_CONTROLS);
    horizontalLayout_switchArea->addWidget(switchEdgeEventA);

    switchEdgeEventB = new WidgetSwitch(parent, "Rising", "Falling", "");
    switchEdgeEventB->setObjectName("edgeEventB");
    switchEdgeEventB->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    switchEdgeEventB->setColor(Graphics::COLOR_CONTROLS);
    horizontalLayout_switchArea->addWidget(switchEdgeEventB);

    labelPicEventsSeq = new QLabel(parent);
    labelPicEventsSeq->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    width = labelPicEventsSeq->width() * 3;
    height = labelPicEventsSeq->height() * 3;
    labelPicEventsSeq->setAlignment(Qt::AlignCenter);
    destination->addWidget(labelPicEventsSeq);

    pixmapEventsSeq = new QPixmap();
    QString picPath = PATH_IMG_EVENT_AB_RR;
    pixmapEventsSeq->load(picPath);
    labelPicEventsSeq->setPixmap(pixmapEventsSeq->scaled(width, height, Qt::KeepAspectRatio));

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);

    connect(buttonsEventsSeq, &WidgetButtons::clicked, this, &CounterTabIntervals::eventsSequenceChangedCallback);
    connect(switchEdgeEventA, &WidgetSwitch::clicked, this, &CounterTabIntervals::eventAEdgeChangedCallback);
    connect(switchEdgeEventB, &WidgetSwitch::clicked, this, &CounterTabIntervals::eventBEdgeChangedCallback);
}

void CounterTabIntervals::loadGraphics(){
    seqAB = (conf->intState.seqAB == IntState::Sequence::AB) ? false : true;
    eventA = (conf->intState.eventA == IntState::Event::RISING) ? false : true;
    eventB = (conf->intState.eventB == IntState::Event::RISING) ? false : true;
    setSeqPicture();
}

void CounterTabIntervals::eventsSequenceChangedCallback(int index){
    seqAB = index;
    setSeqPicture();
}

void CounterTabIntervals::eventAEdgeChangedCallback(int index){
    eventA = index;
    setSeqPicture();
}

void CounterTabIntervals::eventBEdgeChangedCallback(int index){
    eventB = index;
    setSeqPicture();
}

void CounterTabIntervals::setSeqPicture()
{
    setStartButton(false);

    QString picPath;
    if(!seqAB){
        if(eventA && eventB){
            picPath = PATH_IMG_EVENT_AB_FF;
        }else if (!eventA && !eventB) {
            picPath = PATH_IMG_EVENT_AB_RR;
        }else if (!eventA && eventB) {
            picPath = PATH_IMG_EVENT_AB_RF;
        }else if (eventA && !eventB) {
            picPath = PATH_IMG_EVENT_AB_FR;
        }
    }else {
        if(eventA && eventB){
            picPath = PATH_IMG_EVENT_BA_FF;
        }else if (!eventA && !eventB) {
            picPath = PATH_IMG_EVENT_BA_RR;
        }else if (!eventA && eventB) {
            picPath = PATH_IMG_EVENT_BA_RF;
        }else if (eventA && !eventB) {
            picPath = PATH_IMG_EVENT_BA_FR;
        }
    }

    pixmapEventsSeq->load(picPath);
    labelPicEventsSeq->setPixmap(pixmapEventsSeq->scaled(width, height, Qt::KeepAspectRatio));
}

void CounterTabIntervals::setStartButton(bool start){
    if(start){
        buttonStart->setColor(Graphics::COLOR_RUNNING,0);
        buttonStart->setText("Stop",0);
    }else{
        buttonStart->setColor(Graphics::COLOR_CONTROLS,0);
        buttonStart->setText("Start",0);
    }
}
