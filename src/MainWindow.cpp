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
#include <QFile>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QTextCharFormat>
#include <QGraphicsSvgItem>
#include <QSerialPortInfo>

#include "Application.h"

const double V0= 00.00;
const double A0 = 0.000;

const char* const STYLE_BG_RED = "background-color: rgb(210, 0, 0)";
const char* const STYLE_BG_GREEN = "background-color: rgb(0, 210, 0)";
const char* const STYLE_BG_NONE = "";

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(maximumSize());
    setWindowTitle(tr("Power Supply Management v%1").arg(Application::applicationVersion()));
    ui->groupBoxCh1->setMinimumSize(QSize(0,0));
    ui->groupBoxCh2->setMinimumSize(QSize(0,0));

    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->graphicsView->setStyleSheet("background-color: transparent;");

    mStatusBarDeviceInfo = new ClickableLabel(this);
    connect(mStatusBarDeviceInfo, &ClickableLabel::onDoubleClick, this, &MainWindow::ShowDeviceNameOrID);
    QMainWindow::statusBar()->addPermanentWidget(mStatusBarDeviceInfo, 100);
    mStatusBarDeviceLock = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(mStatusBarDeviceLock, 120);
    mStatusCommunicationMetrics = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(mStatusCommunicationMetrics, 80);
    mStatusBarConnectionStatus = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(mStatusBarConnectionStatus);

    mDisplayCh1 = new DisplayWidget(this);
    ui->layoutDisplayCh1->addWidget(mDisplayCh1);

    mDisplayCh2 = new DisplayWidget(this);
    ui->layoutDisplayCh2->addWidget(mDisplayCh2);

    mInputCh1V = new DialWidget(tr("Voltage (V)"), this);
    mInputCh1V->setPrecision(0.01);
    mInputCh1V->setSuffix(tr("V"));
    ui->layoutControlCh1->addWidget(mInputCh1V);

    mInputCh1A = new DialWidget(tr("Current (A)"), this);
    mInputCh1A->setPrecision(0.001);
    mInputCh1A->setSuffix(tr("A"));
    ui->layoutControlCh1->addWidget(mInputCh1A);

    mInputCh2V = new DialWidget(tr("Voltage (V)"), this);
    mInputCh2V->setPrecision(0.01);
    mInputCh2V->setSuffix(tr("V"));
    ui->layoutControlCh2->addWidget(mInputCh2V);

    mInputCh2A = new DialWidget(tr("Current (A)"), this);
    mInputCh2A->setPrecision(0.001);
    mInputCh2A->setSuffix(tr("A"));
    ui->layoutControlCh2->addWidget(mInputCh2A);

    mProtectionSetCh1 = new ProtectionWidget(this);
    ui->layoutProtectionCh1->addWidget(mProtectionSetCh1);

    mProtectionSetCh2 = new ProtectionWidget(this);
    ui->layoutProtectionCh2->addWidget(mProtectionSetCh2);

    mProtectionControl = new ProtectionControlWidget(this);
    ui->groupBoxOutProtection->layout()->addWidget(mProtectionControl);

    mPreset = new PresetWidget(this);
    ui->groupBoxPreset->addWidget(mPreset);

    connect(ui->actionReadonlyMode, &QAction::toggled, this, &MainWindow::SetEnableReadonlyMode);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onSerialPortDoClose);
    connect(ui->actionLockDevice, &QAction::toggled, this, &MainWindow::onSetLocked);
    connect(ui->actionBuzzer, &QAction::toggled, this, &MainWindow::onSetEnabledBeep);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    connect(ui->rdoBtnOutIndependent, &QRadioButton::clicked, this, &MainWindow::ChannelTrackingChanged);
    connect(ui->rdoBtnOutParallel, &QRadioButton::clicked, this, &MainWindow::ChannelTrackingChanged);
    connect(ui->rdoBtnOutSerial, &QRadioButton::clicked, this, &MainWindow::ChannelTrackingChanged);

    connect(mPreset, &PresetWidget::onPresetClicked,  this, &MainWindow::onSetPreset);
    connect(mPreset, &PresetWidget::onSaveClicked,  this, &MainWindow::onSavePreset);

    connect(ui->btnOutput, &QPushButton::clicked, this,&MainWindow::onSetEnableOutputSwitch);

    connect(mInputCh1V, &DialWidget::onSetValue, [this] (double value){ emit onSetVoltage(Global::Channel1, value);});
    connect(mInputCh1A, &DialWidget::onSetValue, [this] (double value){ emit onSetCurrent(Global::Channel1, value);});
    connect(mInputCh2V, &DialWidget::onSetValue, [this] (double value){ emit onSetVoltage(Global::Channel2, value);});
    connect(mInputCh2A, &DialWidget::onSetValue, [this] (double value){ emit onSetCurrent(Global::Channel2, value);});

    connect(mProtectionSetCh1, &ProtectionWidget::onSetOVP, [this] (double value) { emit onSetOverVoltageProtectionValue(Global::Channel1, value); });
    connect(mProtectionSetCh2, &ProtectionWidget::onSetOVP, [this] (double value) { emit onSetOverVoltageProtectionValue(Global::Channel2, value); });
    connect(mProtectionSetCh1, &ProtectionWidget::onSetOCP, [this] (double value) { emit onSetOverCurrentProtectionValue(Global::Channel1, value); });
    connect(mProtectionSetCh2, &ProtectionWidget::onSetOCP, [this] (double value) { emit onSetOverCurrentProtectionValue(Global::Channel2, value); });

    connect(mProtectionControl, &ProtectionControlWidget::onProtectionChanged, this, &MainWindow::onSetEnableOutputProtection);

    connect(ui->menuPort, &QMenu::aboutToShow, this, &MainWindow::slotCreateSerialPortMenuItems);
    connect(ui->menuHelp, &QMenu::triggered, this, &MainWindow::ShowAboutBox);

    createBaudRatesMenu();
    SerialPortClosed();
}

MainWindow::~MainWindow() {
    SerialPortClosed();
    delete ui;
}

bool MainWindow::acceptEnable() const {
    return mIsSerialConnected && !ui->actionReadonlyMode->isChecked();
}

void MainWindow::SerialPortOpened(const QString &serialPortName, int baudRate) {
    mIsSerialConnected = true;
    mStatusBarConnectionStatus->setText(tr("Connected: %1@%2").arg(serialPortName).arg(baudRate));
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

void MainWindow::ConnectionUnknownDevice(QString deviceID) {
    QMessageBox::warning(this, tr("Unknown Device"),
                         tr("Unknown Device (ID: %1)").arg(deviceID),
                         QMessageBox::Close
    );
}

void MainWindow::UpdateCommunicationMetrics(const CommunicationMetrics &info) {
    mStatusCommunicationMetrics->setText(tr("Q:%1 E:%2 D:%3 T:%4")
                                  .arg(info.queueLength())
                                  .arg(info.errorCount)
                                  .arg(info.droppedCount)
                                  .arg(info.responseTimeoutCount));
}

void MainWindow::SerialPortClosed() {
    mIsSerialConnected = false;

    ui->actionDisconnect->setEnabled(false);
    mStatusBarConnectionStatus->setText(tr("Disconnected"));
    mStatusBarDeviceInfo->setText(tr("N/A"));
    mStatusBarDeviceLock->setText(tr("N/A"));

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
    mInputCh1V->setLimits(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mInputCh1A->setLimits(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);
    mInputCh2V->setLimits(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mInputCh2A->setLimits(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);

    mProtectionSetCh1->setLimitsOVP(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mProtectionSetCh2->setLimitsOVP(info.MinVoltage, info.MaxVoltage, info.VoltageSetPrecision);
    mProtectionSetCh1->setLimitsOCP(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);
    mProtectionSetCh2->setLimitsOCP(info.MinCurrent, info.MaxCurrent, info.CurrentSetPrecision);
}

void MainWindow::SetEnableReadonlyMode(bool enable) {
    ui->actionReadonlyMode->setChecked(enable);
    enableControls(!enable);
}

void MainWindow::SetEnableBeep(bool enable) {
    ui->actionBuzzer->setChecked(enable);
}

void MainWindow::ChannelTrackingChanged() {
    auto channelTracking = Global::Independent;
    if (ui->rdoBtnOutParallel->isChecked()) {
        channelTracking = Global::Parallel;
    } else if (ui->rdoBtnOutSerial->isChecked()) {
        channelTracking = Global::Serial;
    }

    emit onSetChannelTracking(channelTracking);
}

void MainWindow::UpdateChannelTrackingMode(Global::ChannelTracking tracking) {
    QString resourceName;
    QString labelText;
    switch (tracking) {
        case Global::Independent:
            ui->rdoBtnOutIndependent->setChecked(true);
            enableChannel(Global::Channel1, true);
            enableChannel(Global::Channel2, true);

            resourceName = ":independent-mode";
            labelText = tr("Two independent channels U: 0-30V, I: 0-5A");
            break;
        case Global::Serial:
            ui->rdoBtnOutSerial->setChecked(true);
            enableChannel(Global::Channel1, false);
            enableChannel(Global::Channel2, true);

            resourceName = ":serial-mode";
            labelText = tr("One channel U: 0-60V, I: 0-5A");
            break;
        case Global::Parallel:
            ui->rdoBtnOutParallel->setChecked(true);
            enableChannel(Global::Channel1, false);
            enableChannel(Global::Channel2, true);

            resourceName = ":parallel-mode";
            labelText = tr("One channel U: 0-30V, I: 0-10A");
            break;
    }
    ui->lblOutputModeHint->setText(labelText);
    openSvg(QFile(resourceName).fileName());
}

void MainWindow::UpdateOutputProtectionMode(Global::OutputProtection protection) {
    mProtectionControl->SetActiveProtection(protection);
}

void MainWindow::UpdateChannelMode(Global::Channel channel, Global::OutputMode mode) {
    if (channel == Global::Channel1) {
        mode == Global::ConstantVoltage ? mDisplayCh1->constantVoltage() : mDisplayCh1->constantCurrent();
    }
    if (channel == Global::Channel2) {
        mode == Global::ConstantVoltage ? mDisplayCh2->constantVoltage() : mDisplayCh2->constantCurrent();
    }
}

void MainWindow::SetEnableOutputSwitch(bool enable) {
    if (enable) {
        ui->btnOutput->setChecked(true);
        ui->btnOutput->setStyleSheet(STYLE_BG_GREEN);
        ui->btnOutput->setText(tr("OUTPUT ON"));
    } else {
        ui->btnOutput->setChecked(false);
        ui->btnOutput->setStyleSheet(STYLE_BG_NONE);
        ui->btnOutput->setText(tr("OUTPUT OFF"));
    }
}

void MainWindow::enableChannel(Global::Channel ch, bool enable) {
    if (enable && !acceptEnable()) {
        return;
    }

    if (ch == Global::Channel1) {
        ui->lblCh1->setEnabled(enable);
        ui->groupBoxCh1->setEnabled(enable);
    } else {
        ui->lblCh2->setEnabled(enable);
        ui->groupBoxCh2->setEnabled(enable);
    }
}

void MainWindow::UpdateActivePreset(Global::MemoryKey key) {
    mPreset->SetActivePreset(key);
}

void MainWindow::UpdateActualVoltage(Global::Channel channel, double voltage) {
    if (channel == Global::Channel1) {
        mDisplayCh1->displayVoltage(voltage);
    } else {
        mDisplayCh2->displayVoltage(voltage);
    }
}

void MainWindow::UpdateActualCurrent(Global::Channel channel, double current) {
    if (channel == Global::Channel1) {
        mDisplayCh1->displayCurrent(current);
    } else {
        mDisplayCh2->displayCurrent(current);
    }
}

void MainWindow::UpdateVoltageSet(Global::Channel channel, double voltage) {
    if (channel == Global::Channel1) {
        mInputCh1V->setValue(voltage);
    }
    if (channel == Global::Channel2) {
        mInputCh2V->setValue(voltage);
    }
}

void MainWindow::UpdateCurrentSet(Global::Channel channel, double current) {
    if (channel == Global::Channel1) {
        mInputCh1A->setValue(current);
    }
    if (channel == Global::Channel2) {
        mInputCh2A->setValue(current);
    }
}

void MainWindow::UpdateOverCurrentProtectionSet(Global::Channel channel, double current) {
    if (channel == Global::Channel1) {
        mProtectionSetCh1->setValueOCP(current);
    } else {
        mProtectionSetCh2->setValueOCP(current);
    }
}

void MainWindow::UpdateOverVoltageProtectionSet(Global::Channel channel, double voltage) {
    if (channel == Global::Channel1) {
        mProtectionSetCh1->setValueOVP(voltage);
    } else {
        mProtectionSetCh2->setValueOVP(voltage);
    }
}


void MainWindow::SetEnableLock(bool enable) {
    if (enable) {
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

void MainWindow::ShowAboutBox() {
    QPixmap icon;
    icon.load(":power-supply-64");
    QMessageBox aboutBox(
            QMessageBox::NoIcon,
            tr("About PS Management"),
            trUtf8("<p align=\"center\">Power Supply Management</p><p align=\"center\">Version %1</p><p>GUI application for remote control of Programmable Power Supply.</p><p>Supported protocol: <ul><li>UNI-T UTP3303C</li><li>UNI-T UTP3305C</li></ul></p><p>This is a free software distributed under MIT license.</p><p>Visit official <a href=\"https://github.com/vitark/PS-Management\">GitHub</a> page for more details.</p> <p>Ⓒ 2021-2022 VitArk</p>").arg(Application::applicationVersion()),
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
        mStatusBarDeviceInfo->setText(mDeviceInfo.ID);
    } else {
        mStatusBarDeviceInfo->setText(mDeviceInfo.Name);
    }
    showDeviceID = !showDeviceID;
}
