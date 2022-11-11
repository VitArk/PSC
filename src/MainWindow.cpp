// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "Application.h"

const double V0 = 0.00;
const double A0 = 0.000;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setupUI();

    createBaudRatesMenu();
    SerialPortClosed();
}

MainWindow::~MainWindow() {
    SerialPortClosed();
    blockSignals(true);
    delete ui;
}

void MainWindow::setupUI() {
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(maximumSize());
    setWindowTitle(tr("Power Supply Management v%1").arg(Application::applicationVersion()));
    ui->groupBoxCh1->setMinimumSize(QSize(0,0));
    ui->groupBoxCh2->setMinimumSize(QSize(0,0));

    connect(ui->actionReadonlyMode, &QAction::toggled, this, &MainWindow::SetEnableReadonlyMode);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onSerialPortDoClose);
    connect(ui->actionLockDevice, &QAction::toggled, this, &MainWindow::onSetLocked);
    connect(ui->actionBuzzer, &QAction::toggled, this, &MainWindow::onSetEnabledBeep);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->menuPort, &QMenu::aboutToShow, this, &MainWindow::CreateSerialPortMenuItems);
    connect(ui->menuHelp, &QMenu::triggered, this, &MainWindow::ShowAboutBox);

    mStatusBar = new StatusBar(this);
    connect(mStatusBar, &StatusBar::onDeviceInfoDoubleClick, this, &MainWindow::ShowDeviceNameOrID);
    setStatusBar(mStatusBar);

    auto display = new DisplayWidget(this);
    ui->layoutDisplayCh1->addWidget(display);
    mDisplay[Global::Channel1] = display;

    display = new DisplayWidget(this);
    ui->layoutDisplayCh2->addWidget(display);
    mDisplay[Global::Channel2] = display;

    auto dial = createDialWidget(tr("Voltage (V)"), tr("V"), 0.01, [=](double value){ emit onSetVoltage(Global::Channel1, value);});
    ui->layoutControlCh1->addWidget(dial);
    mInputVoltage[Global::Channel1] = dial;

    dial = createDialWidget(tr("Voltage (V)"), tr("V"), 0.01, [=](double value){ emit onSetVoltage(Global::Channel2, value);});
    ui->layoutControlCh2->addWidget(dial);
    mInputVoltage[Global::Channel2] = dial;

    dial = createDialWidget(tr("Current (A)"), tr("A"), 0.001, [=](double value){ emit onSetCurrent(Global::Channel1, value);});
    ui->layoutControlCh1->addWidget(dial);
    mInputCurrent[Global::Channel1] = dial;

    dial = createDialWidget(tr("Current (A)"), tr("A"), 0.001, [=](double value){ emit onSetCurrent(Global::Channel2, value);});
    ui->layoutControlCh2->addWidget(dial);
    mInputCurrent[Global::Channel2] = dial;

    auto protectionSet = createProtectionWidget(Global::Channel1);
    ui->layoutProtectionCh1->addWidget(protectionSet);
    mProtectionSet[Global::Channel1] = protectionSet;

    protectionSet = createProtectionWidget(Global::Channel2);
    ui->layoutProtectionCh2->addWidget(protectionSet);
    mProtectionSet[Global::Channel2] = protectionSet;

    mProtectionControl = new ProtectionControlWidget(this);
    ui->groupBoxOutProtection->layout()->addWidget(mProtectionControl);
    connect(mProtectionControl, &ProtectionControlWidget::onProtectionChanged, this, &MainWindow::onSetEnableOutputProtection);

    mPreset = new PresetWidget(this);
    ui->groupBoxPreset->addWidget(mPreset);
    connect(mPreset, &PresetWidget::onPresetClicked,  this, &MainWindow::onSetPreset);
    connect(mPreset, &PresetWidget::onSaveClicked,  this, &MainWindow::onSavePreset);

    mChannelsTracking = new ChannelsTrackingWidget(this);
    ui->groupBoxChannelsTracking->layout()->addWidget(mChannelsTracking);
    connect(mChannelsTracking, &ChannelsTrackingWidget::onSetChannelsTracking, this, &MainWindow::onSetChannelsTracking);

    mOutputSwitch = new OutputSwitch(this);
    ui->outputSwitchLayout->addWidget(mOutputSwitch);
    connect(mOutputSwitch, &QPushButton::clicked, this,&MainWindow::onSetEnableOutputSwitch);
}

template<typename func>
DialWidget *MainWindow::createDialWidget(const QString &title, const QString &suffix, double precision, func lambdaFn) {
    auto dial = new DialWidget(title, this);
    dial->setPrecision(precision);
    dial->setSuffix(suffix);
    connect(dial, &DialWidget::onSetValue, lambdaFn);

    return dial;
}

ProtectionWidget *MainWindow::createProtectionWidget(Global::Channel channel) {
    auto protection = new ProtectionWidget(this);
    connect(protection, &ProtectionWidget::onSetOVP, [=] (double value) { emit onSetOverVoltageProtectionValue(channel, value); });
    connect(protection, &ProtectionWidget::onSetOCP, [=] (double value) { emit onSetOverCurrentProtectionValue(channel, value); });

    return protection;
}

bool MainWindow::acceptEnable() const {
    return mIsSerialConnected && !ui->actionReadonlyMode->isChecked();
}

void MainWindow::SerialPortOpened(const QString &serialPortName, int baudRate) {
    mIsSerialConnected = true;
    mStatusBar->setText(tr("Connected: %1@%2").arg(serialPortName).arg(baudRate), StatusBar::ConnectionStatus);
    ui->actionDisconnect->setEnabled(true);
}

void MainWindow::SerialPortErrorOccurred(const QString &error) {
    QMessageBox::warning(this, tr("Serial Port Error Occurred"),error, QMessageBox::Close);
}

void MainWindow::ConnectionDeviceReady(const Global::DeviceInfo &info) {
    mDeviceInfo = info;
    ShowDeviceNameOrID();
    setControlLimits(info);
    enableControls(true);
}

void MainWindow::ConnectionUnknownDevice(const QString& deviceID) {
    QMessageBox::warning(this, tr("Unknown Device"),
                         tr("Unknown Device (ID: %1)").arg(deviceID),
                         QMessageBox::Close
    );
}

void MainWindow::UpdateCommunicationMetrics(const CommunicationMetrics &info) {
    mStatusBar->setText(tr("Q:%1 E:%2 D:%3 T:%4")
                                  .arg(info.queueLength())
                                  .arg(info.errorCount)
                                  .arg(info.droppedCount)
                                  .arg(info.responseTimeoutCount), StatusBar::DebugInfo);
}

void MainWindow::SerialPortClosed() {
    mIsSerialConnected = false;

    ui->actionDisconnect->setEnabled(false);
    mStatusBar->setText(tr("Disconnected"), StatusBar::ConnectionStatus);
    mStatusBar->setText(tr("N/A"), StatusBar::DeviceInfo);
    mStatusBar->setText(tr("N/A"), StatusBar::LockStatus);

    UpdateChannelTrackingMode(Global::Independent);
    enableControls(false);

    UpdateActualVoltage(Global::Channel1, V0);
    UpdateActualVoltage(Global::Channel2, V0);
    UpdateActualCurrent(Global::Channel1, A0);
    UpdateActualCurrent(Global::Channel2, A0);

    UpdateOverVoltageProtectionSet(Global::Channel1, V0);
    UpdateOverVoltageProtectionSet(Global::Channel2, V0);
    UpdateOverCurrentProtectionSet(Global::Channel1, A0);
    UpdateOverCurrentProtectionSet(Global::Channel2, A0);
}

void MainWindow::enableControls(bool enable) {
    if (enable && !acceptEnable()) {
        return;
    }

    enableChannel(Global::Channel1, enable);
    enableChannel(Global::Channel2, enable);

    ui->groupBoxOperation->setEnabled(enable);
    ui->actionLockDevice->setEnabled(enable);
    ui->actionBuzzer->setEnabled(enable);
}

void MainWindow::setControlLimits(const Global::DeviceInfo &info) {
    mInputVoltage[Global::Channel1]->setLimits(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mInputVoltage[Global::Channel2]->setLimits(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mInputCurrent[Global::Channel1]->setLimits(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);
    mInputCurrent[Global::Channel2]->setLimits(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);

    mProtectionSet[Global::Channel1]->setLimitsOVP(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mProtectionSet[Global::Channel2]->setLimitsOVP(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mProtectionSet[Global::Channel1]->setLimitsOCP(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);
    mProtectionSet[Global::Channel2]->setLimitsOCP(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);
}

void MainWindow::SetEnableReadonlyMode(bool enable) {
    ui->actionReadonlyMode->setChecked(enable);
    enableControls(!enable);
}

void MainWindow::SetEnableBeep(bool enable) {
    ui->actionBuzzer->setChecked(enable);
}

void MainWindow::UpdateChannelTrackingMode(Global::ChannelsTracking tracking) {
    if (mChannelsTracking->channelsTracking() == tracking) {
        return;
    }

    mChannelsTracking->SetChannelsTracking(tracking);
    switch (tracking) {
        case Global::Independent:
            enableChannel(Global::Channel1, true);
            enableChannel(Global::Channel2, true);
            break;
        case Global::Serial:
        case Global::Parallel:
            enableChannel(Global::Channel1, false);
            enableChannel(Global::Channel2, true);
            break;
    }
}

void MainWindow::UpdateOutputProtectionMode(Global::OutputProtection protection) {
    mProtectionControl->SetActiveProtection(protection);
}

void MainWindow::UpdateChannelMode(Global::Channel channel, Global::OutputMode mode) {
    if (mode == Global::ConstantVoltage) {
        mDisplay[channel]->constantVoltage();
    } else {
        mDisplay[channel]->constantCurrent();
    }
}

void MainWindow::SetEnableOutputSwitch(bool enable) {
    mOutputSwitch->SetSwitchOn(enable);
}

void MainWindow::enableChannel(Global::Channel ch, bool enable) {
    if (enable && !acceptEnable()) {
        return;
    }

    if (ch == Global::Channel1) {
        ui->lblCh1->setEnabled(enable);
        ui->groupBoxCh1->setEnabled(enable);
        return;
    }

    if (ch == Global::Channel2) {
        ui->lblCh2->setEnabled(enable);
        ui->groupBoxCh2->setEnabled(enable);
        return;
    }
}

void MainWindow::UpdateActivePreset(Global::MemoryKey key) {
    mPreset->SetActivePreset(key);
}

void MainWindow::UpdateActualVoltage(Global::Channel channel, double voltage) {
    mDisplay[channel]->displayVoltage(voltage);
}

void MainWindow::UpdateActualCurrent(Global::Channel channel, double current) {
    mDisplay[channel]->displayCurrent(current);
}

void MainWindow::UpdateVoltageSet(Global::Channel channel, double voltage) {
    mInputVoltage[channel]->setValue(voltage);
}

void MainWindow::UpdateCurrentSet(Global::Channel channel, double current) {
    mInputCurrent[channel]->setValue(current);
}

void MainWindow::UpdateOverCurrentProtectionSet(Global::Channel channel, double current) {
    mProtectionSet[channel]->setValueOCP(current);
}

void MainWindow::UpdateOverVoltageProtectionSet(Global::Channel channel, double voltage) {
    mProtectionSet[channel]->setValueOVP(voltage);
}

void MainWindow::SetEnableLock(bool enable) {
    if (enable) {
        mStatusBar->setText(tr("Device Controls: Lock"), StatusBar::LockStatus);
        ui->actionLockDevice->setText(tr("Unlock Device Controls"));
    } else {
        mStatusBar->setText(tr("Device Controls: Unlocked"), StatusBar::LockStatus);
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

void MainWindow::SerialPortChanged(bool toggled) {
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

void MainWindow::CreateSerialPortMenuItems() {
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

#ifndef Q_OS_WIN
        if ( !info.portName().startsWith("cu."))
            continue;
#endif

        auto action = new QAction(info.portName(), this);
        action->setCheckable(true);
        action->setChecked(portName == info.portName());
        availableSerialPortsGroup->addAction(action);
        ui->menuPort->addAction(action);

        connect(action, &QAction::toggled, this, &MainWindow::SerialPortChanged);
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

        connect(action, &QAction::toggled, this, &MainWindow::SerialPortChanged);
    }
}

void MainWindow::ShowAboutBox() {
    QPixmap icon;
    icon.load(":power-supply-64");
    QMessageBox aboutBox(
            QMessageBox::NoIcon,
            tr("About PS Management"),
            tr("<p align=\"center\">Power Supply Management</p><p align=\"center\">Version %1</p>"
                   "<p>GUI application for remote control of Programmable Power Supply.</p>"
                   "<p>Supported devices:<ul><li>UNI-T UTP3303C</li><li>UNI-T UTP3305C</li></ul></p>"
                   "<p>This is a free software distributed under GPLv3 license.</p>"
                   "<p>Visit official <a href=\"https://github.com/vitark/PS-Management\">GitHub</a> page for more details.</p>"
                   "<p>Ⓒ 2021-2022 VitArk</p>").arg(Application::applicationVersion()),
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

void MainWindow::ShowDeviceNameOrID() {
    static bool showDeviceID = false;
    if (showDeviceID) {
        mStatusBar->setText(mDeviceInfo.ID, StatusBar::DeviceInfo);
    } else {
        mStatusBar->setText(mDeviceInfo.Name, StatusBar::DeviceInfo);
    }
    showDeviceID = !showDeviceID;
}
