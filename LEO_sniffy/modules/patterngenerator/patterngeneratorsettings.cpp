#include "patterngeneratorsettings.h"

PatternGeneratorSettings::PatternGeneratorSettings(QVBoxLayout *destination, PatternGeneratorConfig *config, QWidget *parent) : QObject(parent),
                                                                                                                                config(config)
{
    WidgetSeparator *sepStartPattern = new WidgetSeparator(parent, "Start pattern generation");
    destination->addWidget(sepStartPattern);

    buttonStart = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonStart->setText("Start");
    buttonStart->setObjectName("buttonPattGenStart");
    destination->addWidget(buttonStart);

    WidgetSeparator *sepPatternSelection = new WidgetSeparator(parent, "Pattern selection");
    destination->addWidget(sepPatternSelection);

    comboPatternSelection = new WidgetSelection(parent, "");
    for (int i = 0; i < pattList.length(); i++)
        comboPatternSelection->addOption(pattList.at(i), i);

    destination->addWidget(comboPatternSelection);

    createComponents(parent, destination);

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);

    buttonSetDefault = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonSetDefault->setText("Reset");
    buttonSetDefault->setObjectName("buttonPattGenSetDefault");
    destination->addWidget(buttonSetDefault);

    connect(buttonSetDefault, &WidgetButtons::clicked, this, &PatternGeneratorSettings::resetPatternCallback);
}

void PatternGeneratorSettings::restoreSettingsAfterStartup()
{
    showComponents(config->pattIndex, true);
    comboPatternSelection->setSelected(config->pattIndex, true);
}

void PatternGeneratorSettings::setSpecification(PatternGeneratorSpec *spec)
{
    int maxSamplingFreq = spec->maxSamplingRate / 2;

    dialUserDefFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialCounterFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialBinaryCodeFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialGrayCodeFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    dialQuadratureFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);

    // New patterns
    if (dialPrbsFreq)
        dialPrbsFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    if (dialPwmFreq)
        dialPwmFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    if (dialLineCodeFreq)
        dialLineCodeFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    if (dial4b5bFreq)
        dial4b5bFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    if (dialJohnsonFreq)
        dialJohnsonFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    if (dialPdmFreq)
        dialPdmFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
    if (dialParBusFreq)
        dialParBusFreq->setRange(PATT_MIN_GEN_FREQ, maxSamplingFreq, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2, true);
}

void PatternGeneratorSettings::createComponents(QWidget *parent, QVBoxLayout *destination)
{
    for (int i = 0; i < pattList.length(); i++)
    {
        QVBoxLayout *layout = new QVBoxLayout();
        patternArea[i] = new QWidget(parent, Qt::Window);
        patternArea[i]->setLayout(layout);
        (this->*createPatternComponents[i])(parent, layout, i);
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(verticalSpacer);
        destination->addWidget(patternArea[i]);
        patternArea[i]->hide();
    }
}

void PatternGeneratorSettings::createUserDefinedComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    buttonUserDefLoadPattern = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, "", 0);
    buttonUserDefLoadPattern->setText("Load CSV");
    buttonUserDefLoadPattern->setObjectName("buttonLoadUserDef");
    destination->addWidget(buttonUserDefLoadPattern);

    dialUserDefFreq = createFrequencyDial(parent, "dialUserDefFreq");
    destination->addWidget(dialUserDefFreq);

    dialUserDefLength = new WidgetDialRange(parent, "");
    dialUserDefLength->setName("Data Length");
    dialUserDefLength->hideUnitSelection();
    dialUserDefLength->setRange(2, 100, "Tick", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialUserDefLength->setObjectName("dialUserDefLength");
    destination->addWidget(dialUserDefLength);

    config->freq[index] = dialUserDefFreq->getRealValue();
}

void PatternGeneratorSettings::createCounterComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialCounterFreq = createFrequencyDial(parent, "dialCounterFreq");
    destination->addWidget(dialCounterFreq);

    dialCounterLength = new WidgetDialRange(parent, "");
    dialCounterLength->setName("Data Length");
    dialCounterLength->hideUnitSelection();
    dialCounterLength->setRange(2, 100, "Tick", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialCounterLength->setObjectName("dialCounterLength");
    destination->addWidget(dialCounterLength);

    config->freq[index] = dialCounterFreq->getRealValue();
}

void PatternGeneratorSettings::createBinaryCodeComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialBinaryCodeFreq = createFrequencyDial(parent, "dialBinaryCodeFreq");
    destination->addWidget(dialBinaryCodeFreq);

    dialBinaryChanNum = new WidgetDialRange(parent, "");
    dialBinaryChanNum->setName("Channel no.");
    dialBinaryChanNum->hideUnitSelection();
    dialBinaryChanNum->setRange(2, PATT_DEFAULT_DATA_LENGTH, "#", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialBinaryChanNum->setObjectName("dialBinaryChanNum");
    destination->addWidget(dialBinaryChanNum);

    config->freq[index] = dialBinaryCodeFreq->getRealValue();
}

void PatternGeneratorSettings::createGrayCodeComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialGrayCodeFreq = createFrequencyDial(parent, "dialGrayCodeFreq");
    destination->addWidget(dialGrayCodeFreq);

    dialGrayCodeChanNum = new WidgetDialRange(parent, "");
    dialGrayCodeChanNum->setName("Channel no.");
    dialGrayCodeChanNum->hideUnitSelection();
    dialGrayCodeChanNum->setRange(2, PATT_DEFAULT_DATA_LENGTH, "#", 1, 1, PATT_DEFAULT_DATA_LENGTH, false, 0);
    dialGrayCodeChanNum->setObjectName("dialGrayCodeChanNum");
    destination->addWidget(dialGrayCodeChanNum);

    config->freq[index] = dialGrayCodeFreq->getRealValue();
}

void PatternGeneratorSettings::createQuadratureComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialQuadratureFreq = createFrequencyDial(parent, "dialQuadratureFreq");
    destination->addWidget(dialQuadratureFreq);

    comboQuadratureSeqAbba = new WidgetSelection(parent, "Speed");
    comboQuadratureSeqAbba->addOption("A/B", 0);
    comboQuadratureSeqAbba->addOption("B/A", 1);
    destination->addWidget(comboQuadratureSeqAbba);

    config->freq[index] = dialQuadratureFreq->getRealValue();
}

void PatternGeneratorSettings::createUartComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    comboUartBaud = new WidgetSelection(parent, "Baud rate");
    const int baudRates[] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
    for (int b : baudRates)
        comboUartBaud->addOption(QString::number(b), b);
    destination->addWidget(comboUartBaud);

    dialUartDataBits = new WidgetDialRange(parent, "");
    dialUartDataBits->setName("Data bits");
    dialUartDataBits->hideUnitSelection();
    dialUartDataBits->setRange(5, 9, "bits", 1, 1, PATT_DEFAULT_UART_DATABITS, false, 0);
    dialUartDataBits->setObjectName("dialUartDataBits");
    destination->addWidget(dialUartDataBits);

    comboUartParity = new WidgetSelection(parent, "Parity");
    comboUartParity->addOption("None", (int)UartParity::None);
    comboUartParity->addOption("Even", (int)UartParity::Even);
    comboUartParity->addOption("Odd", (int)UartParity::Odd);
    destination->addWidget(comboUartParity);

    comboUartStopBits = new WidgetSelection(parent, "Stop bits");
    comboUartStopBits->addOption("1", 1);
    comboUartStopBits->addOption("2", 2);
    destination->addWidget(comboUartStopBits);

    comboUartBitOrder = new WidgetSelection(parent, "Bit order");
    comboUartBitOrder->addOption("LSB first", (int)BitOrder::LSB_First);
    comboUartBitOrder->addOption("MSB first", (int)BitOrder::MSB_First);
    destination->addWidget(comboUartBitOrder);

    comboUartIdle = new WidgetSelection(parent, "Idle level");
    comboUartIdle->addOption("High", (int)UartIdleLevel::High);
    comboUartIdle->addOption("Low", (int)UartIdleLevel::Low);
    destination->addWidget(comboUartIdle);

    comboUartFramingErr = new WidgetSelection(parent, "Framing error");
    comboUartFramingErr->addOption("Off", 0);
    comboUartFramingErr->addOption("On", 1);
    destination->addWidget(comboUartFramingErr);

    comboUartBreak = new WidgetSelection(parent, "Break");
    comboUartBreak->addOption("Off", 0);
    comboUartBreak->addOption("On", 1);
    destination->addWidget(comboUartBreak);

    // set freq for this pattern as baud
    config->freq[index] = comboUartBaud->getSelectedValue();
}

void PatternGeneratorSettings::createSpiComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialSpiClockFreq = createFrequencyDial(parent, "dialSpiClockFreq");
    dialSpiClockFreq->setName("SPI clock");
    destination->addWidget(dialSpiClockFreq);

    comboSpiMode = new WidgetSelection(parent, "Mode (CPOL/CPHA)");
    comboSpiMode->addOption("Mode 0", (int)SpiMode::Mode0);
    comboSpiMode->addOption("Mode 1", (int)SpiMode::Mode1);
    comboSpiMode->addOption("Mode 2", (int)SpiMode::Mode2);
    comboSpiMode->addOption("Mode 3", (int)SpiMode::Mode3);
    destination->addWidget(comboSpiMode);

    dialSpiWordSize = new WidgetDialRange(parent, "");
    dialSpiWordSize->setName("Word size");
    dialSpiWordSize->hideUnitSelection();
    dialSpiWordSize->setRange(4, 16, "bits", 1, 1, PATT_DEFAULT_SPI_WORDSIZE, false, 0);
    dialSpiWordSize->setObjectName("dialSpiWordSize");
    destination->addWidget(dialSpiWordSize);

    comboSpiBitOrder = new WidgetSelection(parent, "Bit order");
    comboSpiBitOrder->addOption("MSB first", (int)BitOrder::MSB_First);
    comboSpiBitOrder->addOption("LSB first", (int)BitOrder::LSB_First);
    destination->addWidget(comboSpiBitOrder);

    comboSpiCsGating = new WidgetSelection(parent, "CS gating");
    comboSpiCsGating->addOption("On (active low)", 1);
    comboSpiCsGating->addOption("Off (always high)", 0);
    destination->addWidget(comboSpiCsGating);

    dialSpiPauseTicks = new WidgetDialRange(parent, "");
    dialSpiPauseTicks->setName("Pause after frame");
    dialSpiPauseTicks->hideUnitSelection();
    dialSpiPauseTicks->setRange(0, 50, "ticks", 1, 1, PATT_DEFAULT_SPI_PAUSE_TICKS, false, 0);
    dialSpiPauseTicks->setObjectName("dialSpiPauseTicks");
    destination->addWidget(dialSpiPauseTicks);

    config->freq[index] = dialSpiClockFreq->getRealValue();
}

void PatternGeneratorSettings::createI2cComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    comboI2cClockFreq = new WidgetSelection(parent, "Speed");
    comboI2cClockFreq->addOption("10 kbit/s", 10000);
    comboI2cClockFreq->addOption("100 kbit/s", 100000);
    comboI2cClockFreq->addOption("400 kbit/s", 400000);
    comboI2cClockFreq->addOption("3.4 Mbit/s", 3400000);
    comboI2cClockFreq->addOption("5 Mbit/s", 5000000);
    destination->addWidget(comboI2cClockFreq);

    comboI2cCommType = new WidgetSelection(parent, "R/W");
    comboI2cCommType->addOption("Read", 0);
    comboI2cCommType->addOption("Write", 1);
    destination->addWidget(comboI2cCommType);

    comboI2cAddrMode = new WidgetSelection(parent, "Address mode");
    comboI2cAddrMode->addOption("7-bit", (int)I2cAddrMode::Addr7Bit);
    comboI2cAddrMode->addOption("10-bit", (int)I2cAddrMode::Addr10Bit);
    destination->addWidget(comboI2cAddrMode);

    dialI2cAddress = new WidgetDialRange(parent, "");
    dialI2cAddress->setName("Address");
    dialI2cAddress->hideUnitSelection();
    dialI2cAddress->setRange(0x00, 0x3FF, "val", 1, 1, PATT_DEFAULT_I2C_ADDRESS, false, 0);
    dialI2cAddress->setObjectName("dialI2cAddress");
    destination->addWidget(dialI2cAddress);

    comboI2cAck = new WidgetSelection(parent, "ACK");
    comboI2cAck->addOption("ACK", 1);
    comboI2cAck->addOption("NACK", 0);
    destination->addWidget(comboI2cAck);

    comboI2cStretch = new WidgetSelection(parent, "Clock stretching");
    comboI2cStretch->addOption("Off", 0);
    comboI2cStretch->addOption("On", 1);
    destination->addWidget(comboI2cStretch);

    comboI2cRepStart = new WidgetSelection(parent, "Repeated start");
    comboI2cRepStart->addOption("Off", 0);
    comboI2cRepStart->addOption("On", 1);
    destination->addWidget(comboI2cRepStart);

    config->freq[index] = comboI2cClockFreq->getSelectedValue();
}

void PatternGeneratorSettings::createPrbsComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialPrbsFreq = createFrequencyDial(parent, "dialPrbsFreq");
    destination->addWidget(dialPrbsFreq);

    comboPrbsOrder = new WidgetSelection(parent, "Order");
    comboPrbsOrder->addOption("PRBS7", 7);
    comboPrbsOrder->addOption("PRBS9", 9);
    comboPrbsOrder->addOption("PRBS11", 11);
    comboPrbsOrder->addOption("PRBS15", 15);
    comboPrbsOrder->addOption("PRBS23", 23);
    comboPrbsOrder->addOption("PRBS31", 31);
    destination->addWidget(comboPrbsOrder);

    config->freq[index] = dialPrbsFreq->getRealValue();
}

void PatternGeneratorSettings::createPwmComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialPwmFreq = createFrequencyDial(parent, "dialPwmFreq");
    destination->addWidget(dialPwmFreq);

    dialPwmDuty = new WidgetDialRange(parent, "");
    dialPwmDuty->setName("Duty");
    dialPwmDuty->hideUnitSelection();
    dialPwmDuty->setRange(0, 100, "%", 1, 1, PATT_DEFAULT_PWM_DUTY, false, 0);
    dialPwmDuty->setObjectName("dialPwmDuty");
    destination->addWidget(dialPwmDuty);

    config->freq[index] = dialPwmFreq->getRealValue();
}

void PatternGeneratorSettings::createLineCodeComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialLineCodeFreq = createFrequencyDial(parent, "dialLineCodeFreq");
    destination->addWidget(dialLineCodeFreq);

    comboLineCodeType = new WidgetSelection(parent, "Type");
    comboLineCodeType->addOption("NRZ-L", (int)LineCodeType::NRZ_L);
    comboLineCodeType->addOption("RZ", (int)LineCodeType::RZ);
    comboLineCodeType->addOption("Manchester", (int)LineCodeType::Manchester);
    comboLineCodeType->addOption("Miller", (int)LineCodeType::Miller);
    comboLineCodeType->addOption("Bi-phase Mark", (int)LineCodeType::BiPhaseMark);
    comboLineCodeType->addOption("Bi-phase Space", (int)LineCodeType::BiPhaseSpace);
    destination->addWidget(comboLineCodeType);

    config->freq[index] = dialLineCodeFreq->getRealValue();
}

void PatternGeneratorSettings::create4b5bComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dial4b5bFreq = createFrequencyDial(parent, "dial4b5bFreq");
    destination->addWidget(dial4b5bFreq);

    dial4b5bGroups = new WidgetDialRange(parent, "");
    dial4b5bGroups->setName("Groups");
    dial4b5bGroups->hideUnitSelection();
    dial4b5bGroups->setRange(1, 32, "#", 1, 1, 8, false, 0);
    dial4b5bGroups->setObjectName("dial4b5bGroups");
    destination->addWidget(dial4b5bGroups);

    config->freq[index] = dial4b5bFreq->getRealValue();
}

void PatternGeneratorSettings::createJohnsonComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialJohnsonFreq = createFrequencyDial(parent, "dialJohnsonFreq");
    destination->addWidget(dialJohnsonFreq);

    dialJohnsonPhases = new WidgetDialRange(parent, "");
    dialJohnsonPhases->setName("Phases");
    dialJohnsonPhases->hideUnitSelection();
    dialJohnsonPhases->setRange(2, PATT_MAX_CHANNELS_NUM, "#", 1, 1, PATT_DEFAULT_JOHNSON_PHASES, false, 0);
    dialJohnsonPhases->setObjectName("dialJohnsonPhases");
    destination->addWidget(dialJohnsonPhases);

    config->freq[index] = dialJohnsonFreq->getRealValue();
}

void PatternGeneratorSettings::createPdmComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialPdmFreq = createFrequencyDial(parent, "dialPdmFreq");
    destination->addWidget(dialPdmFreq);

    dialPdmLevel = new WidgetDialRange(parent, "");
    dialPdmLevel->setName("Level");
    dialPdmLevel->hideUnitSelection();
    dialPdmLevel->setRange(0, 100, "%", 1, 1, PATT_DEFAULT_PDM_LEVEL, false, 0);
    dialPdmLevel->setObjectName("dialPdmLevel");
    destination->addWidget(dialPdmLevel);

    config->freq[index] = dialPdmFreq->getRealValue();
}

void PatternGeneratorSettings::createParBusComponents(QWidget *parent, QVBoxLayout *destination, int index)
{
    dialParBusFreq = createFrequencyDial(parent, "dialParBusFreq");
    destination->addWidget(dialParBusFreq);

    dialParBusWidth = new WidgetDialRange(parent, "");
    dialParBusWidth->setName("Bus width");
    dialParBusWidth->hideUnitSelection();
    dialParBusWidth->setRange(1, PATT_MAX_CHANNELS_NUM, "bits", 1, 1, PATT_DEFAULT_PARBUS_WIDTH, false, 0);
    dialParBusWidth->setObjectName("dialParBusWidth");
    destination->addWidget(dialParBusWidth);

    config->freq[index] = dialParBusFreq->getRealValue();
}

WidgetDialRange *PatternGeneratorSettings::createFrequencyDial(QWidget *parent, QString objName)
{
    WidgetDialRange *dial = new WidgetDialRange(parent, "");
    dial->setName("Cycle frequency");
    dial->hideUnitSelection();
    dial->setRange(1, 4800000, "Hz", 1, 1, PATT_DEFAULT_GEN_FREQ, true, 2);
    dial->setObjectName(objName);

    return dial;
}

/******************************************************************************************/

void PatternGeneratorSettings::resetPatternCallback(int index)
{
    Q_UNUSED(index);
    (this->*resetPatternComponents[config->pattIndex])();
}

void PatternGeneratorSettings::resetUserDefinedComponents()
{
    resetFreqAndDataLenDials(dialUserDefFreq, dialUserDefLength);
}

void PatternGeneratorSettings::resetCounterComponents()
{
    resetFreqAndDataLenDials(dialCounterFreq, dialCounterLength);
}

void PatternGeneratorSettings::resetBinaryCodeComponents()
{
    resetFreqAndDataLenDials(dialBinaryCodeFreq, dialBinaryChanNum);
}

void PatternGeneratorSettings::resetGrayCodeComponents()
{
    resetFreqAndDataLenDials(dialGrayCodeFreq, dialGrayCodeChanNum);
}

void PatternGeneratorSettings::resetQuadratureComponents()
{
    dialQuadratureFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    comboQuadratureSeqAbba->setSelected(0, false);
    config->freq[config->pattIndex] = dialQuadratureFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetUartComponents()
{
    // Baud options order: 9600,19200,38400,57600,115200,230400,460800,921600
    comboUartBaud->setSelected(4, false); // 115200
    dialUartDataBits->setRealValue(PATT_DEFAULT_UART_DATABITS, false);
    comboUartParity->setSelected(0, false);   // None
    comboUartStopBits->setSelected(0, false); // 1
    comboUartBitOrder->setSelected(0, false); // LSB first
    comboUartIdle->setSelected(0, false);     // High
    comboUartFramingErr->setSelected(0, false);
    comboUartBreak->setSelected(0, false);
    config->freq[config->pattIndex] = comboUartBaud->getSelectedValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetSpiComponents()
{
    dialSpiClockFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    comboSpiMode->setSelected(0, false); // Mode 0
    dialSpiWordSize->setRealValue(PATT_DEFAULT_SPI_WORDSIZE, false);
    comboSpiBitOrder->setSelected(0, false); // MSB first
    comboSpiCsGating->setSelected(0, false); // On
    dialSpiPauseTicks->setRealValue(PATT_DEFAULT_SPI_PAUSE_TICKS, false);
    config->freq[config->pattIndex] = dialSpiClockFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetI2cComponents()
{
    comboI2cClockFreq->setSelected(1, false); // 100 kbit/s
    comboI2cCommType->setSelected(1, false);  // Write
    comboI2cAddrMode->setSelected(0, false);  // 7-bit
    dialI2cAddress->setRealValue(PATT_DEFAULT_I2C_ADDRESS, false);
    comboI2cAck->setSelected(0, false);      // ACK
    comboI2cStretch->setSelected(0, false);  // Off
    comboI2cRepStart->setSelected(0, false); // Off
    config->freq[config->pattIndex] = comboI2cClockFreq->getSelectedValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetPrbsComponents()
{
    dialPrbsFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    // Default to the first option (PRBS7)
    comboPrbsOrder->setSelected(0, false);
    config->freq[config->pattIndex] = dialPrbsFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetPwmComponents()
{
    dialPwmFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    dialPwmDuty->setRealValue(PATT_DEFAULT_PWM_DUTY, true);
    config->freq[config->pattIndex] = dialPwmFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetLineCodeComponents()
{
    dialLineCodeFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    comboLineCodeType->setSelected(0, false);
    config->freq[config->pattIndex] = dialLineCodeFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::reset4b5bComponents()
{
    dial4b5bFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    dial4b5bGroups->setRealValue(8, true);
    config->freq[config->pattIndex] = dial4b5bFreq->getRealValue();
    config->dataLen[config->pattIndex] = 8;
}

void PatternGeneratorSettings::resetJohnsonComponents()
{
    dialJohnsonFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    dialJohnsonPhases->setRealValue(PATT_DEFAULT_JOHNSON_PHASES, true);
    config->freq[config->pattIndex] = dialJohnsonFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_JOHNSON_PHASES;
}

void PatternGeneratorSettings::resetPdmComponents()
{
    dialPdmFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    dialPdmLevel->setRealValue(PATT_DEFAULT_PDM_LEVEL, true);
    config->freq[config->pattIndex] = dialPdmFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_DATA_LENGTH;
}

void PatternGeneratorSettings::resetParBusComponents()
{
    dialParBusFreq->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    dialParBusWidth->setRealValue(PATT_DEFAULT_PARBUS_WIDTH, true);
    config->freq[config->pattIndex] = dialParBusFreq->getRealValue();
    config->dataLen[config->pattIndex] = PATT_DEFAULT_PARBUS_WIDTH;
}

void PatternGeneratorSettings::resetFreqAndDataLenDials(WidgetDialRange *freqDial, WidgetDialRange *dataLenDial)
{
    freqDial->setRealValue(PATT_DEFAULT_GEN_FREQ, true);
    dataLenDial->setRealValue(PATT_DEFAULT_DATA_LENGTH, true);
    config->freq[config->pattIndex] = freqDial->getRealValue();
    config->dataLen[config->pattIndex] = dataLenDial->getRealValue();
}

/******************************************************************************************/

void PatternGeneratorSettings::showComponents(int pattIndex, bool visible)
{
    patternArea[pattIndex]->setVisible(visible);
}

void PatternGeneratorSettings::enableGuiComponents(bool enable)
{
    comboPatternSelection->setEnabled(enable);
    patternArea[config->pattIndex]->setEnabled(enable);
    buttonSetDefault->setEnabled(enable);
}
