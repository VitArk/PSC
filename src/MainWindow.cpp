#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QTextCharFormat>
#include <QGraphicsSvgItem>
#include <QSerialPortInfo>

#include "Application.h"

const double vDialCorrection = 100.0;
const double aDialCorrection = 1000.0;
const double V0= 00.00;
const double A0 = 0.000;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(maximumSize());
    setWindowTitle(tr("Power Supply Management v%1").arg(Application::applicationVersion()));

    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->graphicsView->setStyleSheet("background-color: transparent;");

    mStatusBarDeviceInfo = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(mStatusBarDeviceInfo, 40);
    mStatusBarDeviceLock = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(mStatusBarDeviceLock, 120);
    mStatusDebug = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(mStatusDebug, 120);
    mStatusBarConnectionStatus = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(mStatusBarConnectionStatus);


    connect(ui->actionReadonlyMode, &QAction::toggled, this, &MainWindow::slotEnableReadonlyMode);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onSerialPortDoClose);
    connect(ui->actionLockDevice, &QAction::toggled, this, &MainWindow::onLockOperationPanelChanged);
    connect(ui->actionBuzzer, &QAction::toggled, this, &MainWindow::onBuzzerChanged);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionDebugMode, &QAction::triggered, this, &MainWindow::slotEnableDebugMode);

    connect(ui->rdoBtnOutIndependent, &QRadioButton::clicked, this, &MainWindow::slotOutputConnectionMethodChanged);
    connect(ui->rdoBtnOutParallel, &QRadioButton::clicked, this, &MainWindow::slotOutputConnectionMethodChanged);
    connect(ui->rdoBtnOutSerial, &QRadioButton::clicked, this, &MainWindow::slotOutputConnectionMethodChanged);

    connect(ui->btnOVP, &QPushButton::clicked, this, &MainWindow::slotOutputProtectionChanged);
    connect(ui->btnOCP, &QPushButton::clicked, this, &MainWindow::slotOutputProtectionChanged);

    connect(ui->btnM1, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM2, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM3, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM4, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM5, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);

    connect(ui->btnOutput, &QPushButton::clicked, this, &MainWindow::onOutputSwitchChanged);

    connect(ui->dialCh1V, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);
    connect(ui->dialCh1A, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);
    connect(ui->dialCh2V, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);
    connect(ui->dialCh2A, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);

    connect(ui->dialCh1V, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);
    connect(ui->dialCh1A, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);
    connect(ui->dialCh2V, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);
    connect(ui->dialCh2A, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);

    connect(ui->spinCh1V, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);
    connect(ui->spinCh1A, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);
    connect(ui->spinCh2V, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);
    connect(ui->spinCh2A, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);

    connect(&mDebouncedCh1V, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);
    connect(&mDebouncedCh1A, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);
    connect(&mDebouncedCh2V, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);
    connect(&mDebouncedCh2A, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);

    connect(ui->spinCh1OVP, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotOverProtectionChanged);
    connect(ui->spinCh2OVP, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotOverProtectionChanged);
    connect(ui->spinCh1OCP, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotOverProtectionChanged);
    connect(ui->spinCh2OCP, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotOverProtectionChanged);

    connect(ui->menuPort, &QMenu::aboutToShow, this, &MainWindow::slotCreateSerialPortMenuItems);
    connect(ui->menuHelp, &QMenu::triggered, this, &MainWindow::slotShowAboutBox);

    createBaudRatesMenu();
    slotSerialPortClosed();
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::acceptEnable() const {
    return mIsSerialConnected && !ui->actionReadonlyMode->isChecked();
}

void MainWindow::slotSerialPortOpened(const QString &serialPortName, int baudRate) {
    mIsSerialConnected = true;
    mStatusBarConnectionStatus->setText(tr("Connected: %1@%2").arg(serialPortName).arg(baudRate));
    ui->actionDisconnect->setEnabled(true);

    slotEnableDebugMode(mSettings.isDebugModeEnabled());
}

void MainWindow::slotSerialPortErrorOccurred(QString error) {
    QMessageBox::warning(this, tr("Serial Port Error Occurred"),error, QMessageBox::Close);
}

void MainWindow::slotDeviceReady(DeviceInfo info) {
    slotDisplayDeviceID(info.name);
    setControlLimits(info);
    enableControls(true);
}

void MainWindow::slotUnknownDevice(QString deviceID) {
    QMessageBox::warning(this, tr("Unknown Device"),
                         tr("Unknown Device (ID: %1)").arg(deviceID),
                         QMessageBox::Close
    );
}

void MainWindow::slotShowDebugInfo(DebugInfo info) {
    mStatusDebug->setText(tr("Q:%1 E:%2 D:%3")
                                  .arg(info.queueSize)
                                  .arg(info.errorCount)
                                  .arg(info.currentRequestDelay));
}

void MainWindow::slotSerialPortClosed() {
    mIsSerialConnected = false;

    ui->actionDisconnect->setEnabled(false);
    mStatusBarConnectionStatus->setText(tr("Disconnected"));
    mStatusBarDeviceInfo->setText(tr("N/A"));
    mStatusBarDeviceLock->setText(tr("N/A"));

    slotShowOutputConnectionMethod(Independent);
    enableControls(false);

    slotDisplayOutputVoltage(Channel1, V0);
    slotDisplayOutputVoltage(Channel2, V0);
    slotDisplayOutputCurrent(Channel1, A0);
    slotDisplayOutputCurrent(Channel2, A0);

    slotDisplayOverVoltageProtectionValue(Channel1, V0);
    slotDisplayOverVoltageProtectionValue(Channel2, V0);
    slotDisplayOverCurrentProtectionValue(Channel1, A0);
    slotDisplayOverCurrentProtectionValue(Channel2, A0);
}

void MainWindow::enableControls(bool enable) {
    if (enable && !acceptEnable()) {
        return;
    }

    enableChannel(Channel1, enable);
    enableChannel(Channel2, enable);

    ui->groupBoxOperation->setEnabled(enable);
    ui->actionLockDevice->setEnabled(enable);
    ui->actionBuzzer->setEnabled(enable);
}

void MainWindow::setControlLimits(const DeviceInfo &info) {
    ui->dialCh1V->setMaximum(int(info.maxChannelVoltage * vDialCorrection));
    ui->dialCh2V->setMaximum(int(info.maxChannelVoltage * vDialCorrection));
    ui->spinCh1V->setMaximum(info.maxChannelVoltage);
    ui->spinCh2V->setMaximum(info.maxChannelVoltage);
    ui->spinCh1OVP->setMaximum(info.maxChannelVoltage);
    ui->spinCh2OVP->setMaximum(info.maxChannelVoltage);

    ui->dialCh1A->setMaximum(int(info.maxChannelCurrent * aDialCorrection));
    ui->dialCh2A->setMaximum(int(info.maxChannelCurrent * aDialCorrection));
    ui->spinCh1A->setMaximum(info.maxChannelCurrent);
    ui->spinCh2A->setMaximum(info.maxChannelCurrent);
    ui->spinCh1OCP->setMaximum(info.maxChannelCurrent);
    ui->spinCh2OCP->setMaximum(info.maxChannelCurrent);
}

void MainWindow::slotEnableReadonlyMode(bool enable) {
    ui->actionReadonlyMode->setChecked(enable);
    enableControls(!enable);
}

void MainWindow::slotEnableDebugMode(bool enable) {
    ui->actionDebugMode->setChecked(enable);
    mSettings.setDebugModeEnabled(enable);
    emit onEnableDebugMode(enable);
}

void MainWindow::slotDisplayDeviceID(const QString &deviceID) {
    mStatusBarDeviceInfo->setText(deviceID);
}

void MainWindow::slotEnableBuzzer(bool enabled) {
    ui->actionBuzzer->setChecked(enabled);
}

void MainWindow::slotOutputConnectionMethodChanged() {
    OutputConnectionMethod outputMethod = Independent;
    if (ui->rdoBtnOutParallel->isChecked()) {
        outputMethod = Parallel;
    } else if (ui->rdoBtnOutSerial->isChecked()) {
        outputMethod = Serial;
    }

    emit onOutputConnectionMethodChanged(outputMethod);
}

void MainWindow::slotShowOutputConnectionMethod(OutputConnectionMethod method) {
    QString resourceName;
    QString labelText;
    switch (method) {
        case Independent:
            ui->rdoBtnOutIndependent->setChecked(true);
            enableChannel(Channel1, true);
            enableChannel(Channel2, true);

            resourceName = ":independent-mode";
            labelText = tr("Two independent channels U: 0-30V, I: 0-5A");
            break;
        case Serial:
            ui->rdoBtnOutSerial->setChecked(true);
            enableChannel(Channel1, false);
            enableChannel(Channel2, true);

            resourceName = ":serial-mode";
            labelText = tr("One channel U: 0-60V, I: 0-5A");
            break;
        case Parallel:
            ui->rdoBtnOutParallel->setChecked(true);
            enableChannel(Channel1, false);
            enableChannel(Channel2, true);

            resourceName = ":parallel-mode";
            labelText = tr("One channel U: 0-30V, I: 0-10A");
            break;
    }
    ui->lblOutputModeHint->setText(labelText);
    openSvg(QFile(resourceName).fileName());
}

void MainWindow::slotOutputProtectionChanged() {
    OutputProtection protection = OutputProtectionAllDisabled;
    protection = ui->btnOVP->isChecked() ? OverVoltageProtectionOnly : protection;
    protection = ui->btnOCP->isChecked() ? OverCurrentProtectionOnly : protection;
    protection = ui->btnOCP->isChecked() && ui->btnOVP->isChecked() ? OutputProtectionAllEnabled : protection;

    emit onOutputProtectionChanged(protection);
}

void MainWindow::slotShowOutputProtectionMode(OutputProtection protection) {
    switch (protection) {
        case OutputProtectionAllDisabled:
            ui->btnOVP->setChecked(false);
            ui->btnOCP->setChecked(false);
            break;
        case OutputProtectionAllEnabled:
            ui->btnOVP->setChecked(true);
            ui->btnOCP->setChecked(true);
            break;
        case OverVoltageProtectionOnly:
            ui->btnOVP->setChecked(true);
            ui->btnOCP->setChecked(false);
            break;
        case OverCurrentProtectionOnly:
            ui->btnOVP->setChecked(false);
            ui->btnOCP->setChecked(true);
            break;
    }
}

void MainWindow::slotShowOutputStabilizingMode(TOutputStabilizingMode channel1, TOutputStabilizingMode channel2) {
    if (channel1 == ConstantCurrent) {
        highlight(HighlightRed, ui->lblCh1CC);
        highlight(HighlightNone, ui->lblCh1CV);
    } else {
        highlight(HighlightNone, ui->lblCh1CC);
        highlight(HighlightGreen, ui->lblCh1CV);
    }

    if (channel2 == ConstantCurrent) {
        highlight(HighlightRed, ui->lblCh2CC);
        highlight(HighlightNone, ui->lblCh2CV);
    } else {
        highlight(HighlightNone, ui->lblCh2CC);
        highlight(HighlightGreen, ui->lblCh2CV);
    }
}

void MainWindow::slotShowOutputSwitchStatus(bool state) {
    if (state) {
        ui->btnOutput->setChecked(true);
        ui->btnOutput->setStyleSheet("background-color: rgb(0, 210, 0)");
        ui->btnOutput->setText(tr("OUTPUT ON"));
    } else {
        ui->btnOutput->setChecked(false);
        ui->btnOutput->setStyleSheet("");
        ui->btnOutput->setText(tr("OUTPUT OFF"));
    }
}

void MainWindow::enableChannel(TChannel ch, bool enable) {
    if (enable && !acceptEnable()) {
        return;
    }

    if (ch == Channel1) {
        ui->lblCh1->setEnabled(enable);
        ui->groupBoxCh1->setEnabled(enable);
    } else {
        ui->lblCh2->setEnabled(enable);
        ui->groupBoxCh2->setEnabled(enable);
    }
}

void MainWindow::slotEnableMemoryKey(TMemoryKey key) {
    switch (key) {
        case M1: ui->btnM1->setChecked(true); break;
        case M2: ui->btnM2->setChecked(true); break;
        case M3: ui->btnM3->setChecked(true); break;
        case M4: ui->btnM4->setChecked(true); break;
        case M5: ui->btnM5->setChecked(true); break;
    }
}

void MainWindow::slotMemoryKeyChanged(bool toggle) {
    if (toggle) {
        if (sender() == ui->btnM1) emit onMemoryKeyChanged(M1);
        else if (sender() == ui->btnM2) emit onMemoryKeyChanged(M2);
        else if (sender() == ui->btnM3) emit onMemoryKeyChanged(M3);
        else if (sender() == ui->btnM4) emit onMemoryKeyChanged(M4);
        else if (sender() == ui->btnM5) emit onMemoryKeyChanged(M5);
    } else {
        auto senderBtn = qobject_cast<QPushButton *>(sender());
        senderBtn->toggle();
    }
}

void MainWindow::slotDialControlChanged() {
    ui->spinCh1V->setValue(ui->dialCh1V->value() / vDialCorrection);
    ui->spinCh2V->setValue(ui->dialCh2V->value() / vDialCorrection);
    ui->spinCh1A->setValue(ui->dialCh1A->value() / aDialCorrection);
    ui->spinCh2A->setValue(ui->dialCh2A->value() / aDialCorrection);
}

void MainWindow::slotSpinControlChanged() {
    ui->dialCh1V->setValue(int(ui->spinCh1V->value() * vDialCorrection));
    ui->dialCh2V->setValue(int(ui->spinCh2V->value() * vDialCorrection));
    ui->dialCh1A->setValue(int(ui->spinCh1A->value() * aDialCorrection));
    ui->dialCh2A->setValue(int(ui->spinCh2A->value() * aDialCorrection));
    slotControlValueChanged();
}

void MainWindow::slotControlValueChanged() {
    mDebouncedCh1V.setValue(ui->spinCh1V->value());
    mDebouncedCh1A.setValue(ui->spinCh1A->value());
    mDebouncedCh2V.setValue(ui->spinCh2V->value());
    mDebouncedCh2A.setValue(ui->spinCh2A->value());
}

void MainWindow::slotControlValueChangedDebounced(double value) {
    if (sender() == &mDebouncedCh1V) emit onVoltageChanged(Channel1, value);
    else if (sender() == &mDebouncedCh1A) emit onCurrentChanged(Channel1, value);
    else if (sender() == &mDebouncedCh2V) emit onVoltageChanged(Channel2, value);
    else if (sender() == &mDebouncedCh2A) emit onCurrentChanged(Channel2, value);

    ui->spinCh1A->clearFocus();
    ui->spinCh1V->clearFocus();
    ui->spinCh2V->clearFocus();
    ui->spinCh2A->clearFocus();
    ui->dialCh1A->clearFocus();
    ui->dialCh1V->clearFocus();
    ui->dialCh2A->clearFocus();
    ui->dialCh2V->clearFocus();
}

void MainWindow::slotDisplayOutputVoltage(TChannel channel, double voltage) {
    if (channel == Channel1) {
        ui->lcdCh1V->display(voltageFormat(voltage));
    } else {
        ui->lcdCh2V->display(voltageFormat(voltage));
    }
}

void MainWindow::slotDisplayOutputCurrent(TChannel channel, double current) {
    if (channel == Channel1) {
        ui->lcdCh1A->display(currentFormat(current));
    } else {
        ui->lcdCh2A->display(currentFormat(current));
    }
}

void MainWindow::slotDisplaySetVoltage(TChannel channel, double voltage) {
    if (channel == Channel1) {
        if (ui->spinCh1V->hasFocus() || ui->dialCh1V->hasFocus())
            return;
        ui->spinCh1V->setValue(voltage);
        ui->dialCh1V->setValue(int(ui->spinCh1V->value() * vDialCorrection));
    } else {
        if (ui->spinCh2V->hasFocus() || ui->dialCh2V->hasFocus())
            return;
        ui->spinCh2V->setValue(voltage);
        ui->dialCh2V->setValue(int(ui->spinCh2V->value() * vDialCorrection));
    }
}

void MainWindow::slotDisplaySetCurrent(TChannel channel, double current) {
    if (channel == Channel1) {
        if (ui->spinCh1A->hasFocus() || ui->dialCh1A->hasFocus())
            return;
        ui->spinCh1A->setValue(current);
        ui->dialCh1A->setValue(int(ui->spinCh1A->value() * aDialCorrection));
    } else {
        if (ui->spinCh2A->hasFocus() || ui->dialCh2A->hasFocus())
            return;
        ui->spinCh2A->setValue(current);
        ui->dialCh2A->setValue(int(ui->spinCh2A->value() * aDialCorrection));
    }
}

void MainWindow::slotDisplayOverCurrentProtectionValue(TChannel channel, double current) {
    if (channel == Channel1) {
        ui->spinCh1OCP->setValue(current);
    } else {
        ui->spinCh2OCP->setValue(current);
    }
}

void MainWindow::slotDisplayOverVoltageProtectionValue(TChannel channel, double voltage) {
    if (channel == Channel1) {
        ui->spinCh1OVP->setValue(voltage);
    } else {
        ui->spinCh2OVP->setValue(voltage);
    }
}

void MainWindow::slotOverProtectionChanged() {
    if (sender() == ui->spinCh1OVP) emit onOverVoltageProtectionChanged(Channel1, ui->spinCh1OVP->value());
    else if (sender() == ui->spinCh2OVP) emit onOverVoltageProtectionChanged(Channel1, ui->spinCh2OVP->value());
    else if (sender() == ui->spinCh1OCP) emit onOverCurrentProtectionChanged(Channel2, ui->spinCh1OCP->value());
    else if (sender() == ui->spinCh2OCP) emit onOverCurrentProtectionChanged(Channel2, ui->spinCh2OCP->value());
}

void MainWindow::slotLockOperationPanel(bool lock) {
    if (lock) {
        mStatusBarDeviceLock->setText(tr("Device Controls: Lock"));
        ui->actionLockDevice->setText(tr("Unlock Device Controls"));
    } else {
        mStatusBarDeviceLock->setText(tr("Device Controls: Unlocked"));
        ui->actionLockDevice->setText(tr("Lock Device Controls"));
    }
}

void MainWindow::autoOpenSerialPort() {
    int baudRate = mSettings.serialPortBaudRate();
    QString portName = mSettings.serialPortName();
    if (portName.isEmpty()) {
        return;
    }

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.isNull())
            continue;
        if (portName == info.portName()) {
            emit onSerialPortSettingsChanged(portName, baudRate);
            break;
        }
    }
}

int MainWindow::chosenBaudRates(int defaultValue) const {
    foreach(auto item, ui->menuBaudRate->actions()) {
        if (item->isChecked()) {
            return item->text().toInt();
        }
    }
    return defaultValue;
}

QString MainWindow::chosenSerialPort() const {
    foreach(auto item, ui->menuPort->actions()) {
        if (item->isChecked()) {
            return item->text();
        }
    }
    return "";
}

void MainWindow::slotSerialPortConnectionToggled(bool toggled) {
    if (!toggled)
        return;

    QString portName = chosenSerialPort();
    if (portName.isEmpty()) {
        return;
    }
    mSettings.setSerialPortName(portName);

    int baudRate = chosenBaudRates();
    mSettings.setSerialPortBaudRate(baudRate);

    emit onSerialPortSettingsChanged(portName, baudRate);
}

void MainWindow::slotCreateSerialPortMenuItems() {
    foreach (auto a, ui->menuPort->actions()) {
        a->deleteLater();
    }

    ui->menuPort->clear();
    ui->menuPort->addAction(tr("Serial ports"))->setEnabled(false);
    auto availableSerialPortsGroup = new QActionGroup(this);
    availableSerialPortsGroup->setExclusive(true);

    QString portName = "";
    if (mIsSerialConnected) {
        portName = mSettings.serialPortName();
    }

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.isNull())
            continue;

#if not defined(Q_OS_WIN)
        if ( !info.portName().startsWith("cu."))
            continue;
#endif

        auto action = new QAction(info.portName(), this);
        action->setCheckable(true);
        action->setChecked(portName == info.portName());
        availableSerialPortsGroup->addAction(action);
        ui->menuPort->addAction(action);

        connect(action, &QAction::toggled, this, &MainWindow::slotSerialPortConnectionToggled);
    }
}

void MainWindow::createBaudRatesMenu() {
    ui->menuBaudRate->addAction(tr("Baud Rate"))->setEnabled(false);
    auto baudRatesGroup = new QActionGroup(this);
    baudRatesGroup->setExclusive(true);

    int defaultValue = mSettings.serialPortBaudRate();
    foreach (auto baud, QSerialPortInfo::standardBaudRates()) {
        if (baud < 9600 || baud > 115200) {
            continue;
        }
        auto action = new QAction(QString::number(baud), this);
        action->setCheckable(true);
        action->setChecked(baud == defaultValue);

        baudRatesGroup->addAction(action);
        ui->menuBaudRate->addAction(action);

        connect(action, &QAction::toggled, this, &MainWindow::slotSerialPortConnectionToggled);
    }
}

QString MainWindow::currentFormat(double value) {
    return QString::asprintf("%01.03f", value);
}

QString MainWindow::voltageFormat(double value) {
    return QString::asprintf("%02.02f", value);
}

void MainWindow::highlight(MainWindow::THighlight color, QLabel *label) {
    switch (color) {
        case HighlightRed:
            label->setStyleSheet("background-color: rgb(210, 0, 0)");
            break;
        case HighlightGreen:
            label->setStyleSheet("background-color: rgb(0, 210, 0)");
            break;
        default:
            label->setStyleSheet("");
            break;
    }
}

void MainWindow::openSvg(const QString &resource) {
    QGraphicsScene *s = ui->graphicsView->scene();
    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(resource));
    if (!svgItem->renderer()->isValid())
        return;

    s->clear();
    ui->graphicsView->resetTransform();

    auto item = svgItem.take();
    item->setFlags(QGraphicsItem::ItemClipsToShape);
    item->setCacheMode(QGraphicsItem::NoCache);
    item->setZValue(0);
    s->addItem(item);
}

void MainWindow::slotShowAboutBox() {
    QPixmap icon;
    icon.load(":power-supply-64");
    QMessageBox aboutBox(
            QMessageBox::NoIcon,
            tr("About PS Management"),
            trUtf8("<p align=\"center\">Power Supply Management</p><p align=\"center\">Version %1</p><p>GUI application for remote control of Programmable Power Supply.</p><p>Supported devices: <ul><li>UNI-T UTP3303C</li><li>UNI-T UTP3305C</li></ul></p><p>This is a free software distributed under MIT license.</p><p>Visit official <a href=\"https://github.com/vitark/PS-Management\">GitHub</a> page for more details.</p> <p>Ⓒ 2021-2022 VitArk</p>").arg(Application::applicationVersion()),
            QMessageBox::Ok
    );
    aboutBox.setIconPixmap(icon);
    QList<QLabel *> labels = aboutBox.findChildren<QLabel *>();
    for (int i = 0; i < labels.count(); i++) {
        QLabel *lab = labels.at(i);
        lab->setTextInteractionFlags(Qt::TextBrowserInteraction);
    }
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.exec();
}






