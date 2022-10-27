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

const double vDialCorrection = 100.0;
const double aDialCorrection = 1000.0;
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


    connect(ui->actionReadonlyMode, &QAction::toggled, this, &MainWindow::SetEnableReadonlyMode);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onSerialPortDoClose);
    connect(ui->actionLockDevice, &QAction::toggled, this, &MainWindow::onSetLocked);
    connect(ui->actionBuzzer, &QAction::toggled, this, &MainWindow::onSetEnabledBeep);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    connect(ui->rdoBtnOutIndependent, &QRadioButton::clicked, this, &MainWindow::ChannelTrackingChanged);
    connect(ui->rdoBtnOutParallel, &QRadioButton::clicked, this, &MainWindow::ChannelTrackingChanged);
    connect(ui->rdoBtnOutSerial, &QRadioButton::clicked, this, &MainWindow::ChannelTrackingChanged);

    connect(ui->btnOVP, &QPushButton::clicked, this, &MainWindow::OutputProtectionChanged);
    connect(ui->btnOCP, &QPushButton::clicked, this, &MainWindow::OutputProtectionChanged);

    connect(ui->btnM1, &QPushButton::clicked, this, &MainWindow::PresetChanged);
    connect(ui->btnM2, &QPushButton::clicked, this, &MainWindow::PresetChanged);
    connect(ui->btnM3, &QPushButton::clicked, this, &MainWindow::PresetChanged);
    connect(ui->btnM4, &QPushButton::clicked, this, &MainWindow::PresetChanged);
    connect(ui->btnM5, &QPushButton::clicked, this, &MainWindow::PresetChanged);
    connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::PresetSave);

    connect(ui->btnOutput, &QPushButton::clicked, this, &MainWindow::onSetEnableOutputSwitch);

    connect(ui->dialCh1V, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);
    connect(ui->dialCh1A, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);
    connect(ui->dialCh2V, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);
    connect(ui->dialCh2A, &QDial::valueChanged, this, &MainWindow::slotDialControlChanged);

//    connect(ui->dialCh1V, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);
//    connect(ui->dialCh1A, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);
//    connect(ui->dialCh2V, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);
//    connect(ui->dialCh2A, &QDial::sliderReleased, this, &MainWindow::slotControlValueChanged);

    connect(ui->spinCh1V, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);
    connect(ui->spinCh1A, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);
    connect(ui->spinCh2V, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);
    connect(ui->spinCh2A, &QDoubleSpinBox::editingFinished, this, &MainWindow::slotSpinControlChanged);

    connect(&mDebouncedCh1V, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);
    connect(&mDebouncedCh1A, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);
    connect(&mDebouncedCh2V, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);
    connect(&mDebouncedCh2A, &Debounce::onChangedDebounced, this, &MainWindow::slotControlValueChangedDebounced);

    connect(ui->spinCh1OVP, &QDoubleSpinBox::editingFinished, this, &MainWindow::OverProtectionValueChanged);
    connect(ui->spinCh2OVP, &QDoubleSpinBox::editingFinished, this, &MainWindow::OverProtectionValueChanged);
    connect(ui->spinCh1OCP, &QDoubleSpinBox::editingFinished, this, &MainWindow::OverProtectionValueChanged);
    connect(ui->spinCh2OCP, &QDoubleSpinBox::editingFinished, this, &MainWindow::OverProtectionValueChanged);

    connect(ui->menuPort, &QMenu::aboutToShow, this, &MainWindow::slotCreateSerialPortMenuItems);
    connect(ui->menuHelp, &QMenu::triggered, this, &MainWindow::ShowAboutBox);

    createBaudRatesMenu();
    SerialPortClosed();
}

MainWindow::~MainWindow() {
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

void MainWindow::SerialPortErrorOccurred(QString error) {
    QMessageBox::warning(this, tr("Serial Port Error Occurred"),error, QMessageBox::Close);
}

void MainWindow::ConnectionDeviceReady(const Protocol::DeviceInfo &info) {
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

    UpdateChannelTrackingMode(Protocol::Independent);
    enableControls(false);

    UpdateActualVoltage(Protocol::Channel1, V0);
    UpdateActualVoltage(Protocol::Channel2, V0);
    UpdateActualCurrent(Protocol::Channel1, A0);
    UpdateActualCurrent(Protocol::Channel2, A0);

    UpdateOverVoltageProtectionSet(Protocol::Channel1, V0);
    UpdateOverVoltageProtectionSet(Protocol::Channel2, V0);
    UpdateOverCurrentProtectionSet(Protocol::Channel1, A0);
    UpdateOverCurrentProtectionSet(Protocol::Channel2, A0);
}

void MainWindow::enableControls(bool enable) {
    if (enable && !acceptEnable()) {
        return;
    }

    enableChannel(Protocol::Channel1, enable);
    enableChannel(Protocol::Channel2, enable);

    ui->groupBoxOperation->setEnabled(enable);
    ui->actionLockDevice->setEnabled(enable);
    ui->actionBuzzer->setEnabled(enable);
}

void MainWindow::setControlLimits(const Protocol::DeviceInfo &info) {
    ui->dialCh1V->setMaximum(int(info.MaxVoltage * vDialCorrection));
    ui->dialCh2V->setMaximum(int(info.MaxVoltage * vDialCorrection));
    ui->spinCh1V->setMaximum(info.MaxVoltage);
    ui->spinCh2V->setMaximum(info.MaxVoltage);
    ui->spinCh1OVP->setMaximum(info.MaxVoltage);
    ui->spinCh2OVP->setMaximum(info.MaxVoltage);

    ui->dialCh1A->setMaximum(int(info.MaxCurrent * aDialCorrection));
    ui->dialCh2A->setMaximum(int(info.MaxCurrent * aDialCorrection));
    ui->spinCh1A->setMaximum(info.MaxCurrent);
    ui->spinCh2A->setMaximum(info.MaxCurrent);
    ui->spinCh1OCP->setMaximum(info.MaxCurrent);
    ui->spinCh2OCP->setMaximum(info.MaxCurrent);
}

void MainWindow::SetEnableReadonlyMode(bool enable) {
    ui->actionReadonlyMode->setChecked(enable);
    enableControls(!enable);
}

void MainWindow::SetEnableBeep(bool enable) {
    ui->actionBuzzer->setChecked(enable);
}

void MainWindow::ChannelTrackingChanged() {
    auto channelTracking = Protocol::Independent;
    if (ui->rdoBtnOutParallel->isChecked()) {
        channelTracking = Protocol::Parallel;
    } else if (ui->rdoBtnOutSerial->isChecked()) {
        channelTracking = Protocol::Serial;
    }

    emit onSetChannelTracking(channelTracking);
}

void MainWindow::UpdateChannelTrackingMode(Protocol::ChannelTracking tracking) {
    QString resourceName;
    QString labelText;
    switch (tracking) {
        case Protocol::Independent:
            ui->rdoBtnOutIndependent->setChecked(true);
            enableChannel(Protocol::Channel1, true);
            enableChannel(Protocol::Channel2, true);

            resourceName = ":independent-mode";
            labelText = tr("Two independent channels U: 0-30V, I: 0-5A");
            break;
        case Protocol::Serial:
            ui->rdoBtnOutSerial->setChecked(true);
            enableChannel(Protocol::Channel1, false);
            enableChannel(Protocol::Channel2, true);

            resourceName = ":serial-mode";
            labelText = tr("One channel U: 0-60V, I: 0-5A");
            break;
        case Protocol::Parallel:
            ui->rdoBtnOutParallel->setChecked(true);
            enableChannel(Protocol::Channel1, false);
            enableChannel(Protocol::Channel2, true);

            resourceName = ":parallel-mode";
            labelText = tr("One channel U: 0-30V, I: 0-10A");
            break;
    }
    ui->lblOutputModeHint->setText(labelText);
    openSvg(QFile(resourceName).fileName());
}

void MainWindow::OutputProtectionChanged() {
    auto protection = Protocol::OutputProtectionAllDisabled;
    protection = ui->btnOVP->isChecked() ? Protocol::OverVoltageProtectionOnly : protection;
    protection = ui->btnOCP->isChecked() ? Protocol::OverCurrentProtectionOnly : protection;
    protection = ui->btnOCP->isChecked() && ui->btnOVP->isChecked() ? Protocol::OutputProtectionAllEnabled : protection;

    emit onSetEnableOutputProtection(protection);
}

void MainWindow::OverProtectionValueChanged() {
    if (sender() == ui->spinCh1OVP)      emit onSetOverVoltageProtectionValue(Protocol::Channel1,ui->spinCh1OVP->value());
    else if (sender() == ui->spinCh2OVP) emit onSetOverVoltageProtectionValue(Protocol::Channel2,ui->spinCh2OVP->value());
    else if (sender() == ui->spinCh1OCP) emit onSetOverCurrentProtectionValue(Protocol::Channel1,ui->spinCh1OCP->value());
    else if (sender() == ui->spinCh2OCP) emit onSetOverCurrentProtectionValue(Protocol::Channel2,ui->spinCh2OCP->value());
}


void MainWindow::UpdateOutputProtectionMode(Protocol::OutputProtection protection) {
    ui->btnOVP->setChecked(protection == Protocol::OverVoltageProtectionOnly || protection == Protocol::OutputProtectionAllEnabled);
    ui->btnOCP->setChecked(protection == Protocol::OverCurrentProtectionOnly || protection == Protocol::OutputProtectionAllEnabled);
}

void MainWindow::UpdateChannelMode(Protocol::Channel channel, Protocol::OutputMode mode) {
    if (channel == Protocol::Channel1) {
        if (mode == Protocol::ConstantCurrent) {
            ui->lblCh1CC->setStyleSheet(STYLE_BG_RED);
            ui->lblCh1CV->setStyleSheet(STYLE_BG_NONE);
        } else {
            ui->lblCh1CC->setStyleSheet(STYLE_BG_NONE);
            ui->lblCh1CV->setStyleSheet(STYLE_BG_GREEN);
        }
    }
    if (channel == Protocol::Channel2) {
        if (mode == Protocol::ConstantCurrent) {
            ui->lblCh2CC->setStyleSheet(STYLE_BG_RED);
            ui->lblCh2CV->setStyleSheet(STYLE_BG_NONE);
        } else {
            ui->lblCh2CC->setStyleSheet(STYLE_BG_NONE);
            ui->lblCh2CV->setStyleSheet(STYLE_BG_GREEN);
        }
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

void MainWindow::enableChannel(Protocol::Channel ch, bool enable) {
    if (enable && !acceptEnable()) {
        return;
    }

    if (ch == Protocol::Channel1) {
        ui->lblCh1->setEnabled(enable);
        ui->groupBoxCh1->setEnabled(enable);
    } else {
        ui->lblCh2->setEnabled(enable);
        ui->groupBoxCh2->setEnabled(enable);
    }
}

void MainWindow::UpdateActivePreset(Protocol::MemoryKey key) {
    switch (key) {
        case Protocol::MemoryKey::M1: ui->btnM1->setChecked(true); break;
        case Protocol::MemoryKey::M2: ui->btnM2->setChecked(true); break;
        case Protocol::MemoryKey::M3: ui->btnM3->setChecked(true); break;
        case Protocol::MemoryKey::M4: ui->btnM4->setChecked(true); break;
        case Protocol::MemoryKey::M5: ui->btnM5->setChecked(true); break;
    }
}

void MainWindow::PresetChanged() {
    if (sender() == ui->btnM1) emit onSetPreset(Protocol::MemoryKey::M1);
    else if (sender() == ui->btnM2) emit onSetPreset(Protocol::MemoryKey::M2);
    else if (sender() == ui->btnM3) emit onSetPreset(Protocol::MemoryKey::M3);
    else if (sender() == ui->btnM4) emit onSetPreset(Protocol::MemoryKey::M4);
    else if (sender() == ui->btnM5) emit onSetPreset(Protocol::MemoryKey::M5);
}

void MainWindow::PresetSave() {
    if (ui->btnM1->isChecked()) emit onSavePreset(Protocol::MemoryKey::M1);
    else if (ui->btnM2->isChecked()) emit onSavePreset(Protocol::MemoryKey::M2);
    else if (ui->btnM3->isChecked()) emit onSavePreset(Protocol::MemoryKey::M3);
    else if (ui->btnM4->isChecked()) emit onSavePreset(Protocol::MemoryKey::M4);
    else if (ui->btnM5->isChecked()) emit onSavePreset(Protocol::MemoryKey::M5);
}

//void MainWindow::ControlValueChanged() {
//
//}



void MainWindow::slotDialControlChanged() {
    qDebug() << "slotDialControlChanged" << qobject_cast<QDial*>(sender())->value();
    ui->spinCh1V->setValue(ui->dialCh1V->value() / vDialCorrection);
    ui->spinCh2V->setValue(ui->dialCh2V->value() / vDialCorrection);
    ui->spinCh1A->setValue(ui->dialCh1A->value() / aDialCorrection);
    ui->spinCh2A->setValue(ui->dialCh2A->value() / aDialCorrection);
}

void MainWindow::slotSpinControlChanged() {
    qDebug() << "slotSpinControlChanged - edit finished" << qobject_cast<QDoubleSpinBox*>(sender())->value();
    ui->dialCh1V->setValue(int(ui->spinCh1V->value() * vDialCorrection));
    ui->dialCh2V->setValue(int(ui->spinCh2V->value() * vDialCorrection));
    ui->dialCh1A->setValue(int(ui->spinCh1A->value() * aDialCorrection));
    ui->dialCh2A->setValue(int(ui->spinCh2A->value() * aDialCorrection));
    slotControlValueChanged();
}

void MainWindow::slotControlValueChanged() {
    //qDebug() << "slotControlValueChanged" << qobject_cast<QDial*>(sender())->value();
    mDebouncedCh1V.setValue(ui->spinCh1V->value());
    mDebouncedCh1A.setValue(ui->spinCh1A->value());
    mDebouncedCh2V.setValue(ui->spinCh2V->value());
    mDebouncedCh2A.setValue(ui->spinCh2A->value());
}

void MainWindow::slotControlValueChangedDebounced(double value) {
    qDebug() << "slotControlValueChangedDebounced" << value;
    if (sender() == &mDebouncedCh1V) emit onSetVoltage(Protocol::Channel1, value);
    else if (sender() == &mDebouncedCh1A) emit onSetCurrent(Protocol::Channel1, value);
    else if (sender() == &mDebouncedCh2V) emit onSetVoltage(Protocol::Channel2, value);
    else if (sender() == &mDebouncedCh2A) emit onSetCurrent(Protocol::Channel2, value);

    ui->spinCh1A->clearFocus();
    ui->spinCh1V->clearFocus();
    ui->spinCh2V->clearFocus();
    ui->spinCh2A->clearFocus();
    ui->dialCh1A->clearFocus();
    ui->dialCh1V->clearFocus();
    ui->dialCh2A->clearFocus();
    ui->dialCh2V->clearFocus();
}

void MainWindow::UpdateActualVoltage(Protocol::Channel channel, double voltage) {
    if (channel == Protocol::Channel1) {
        ui->lcdCh1V->display(voltageFormat(voltage));
    } else {
        ui->lcdCh2V->display(voltageFormat(voltage));
    }
}

void MainWindow::UpdateActualCurrent(Protocol::Channel channel, double current) {
    if (channel == Protocol::Channel1) {
        ui->lcdCh1A->display(currentFormat(current));
    } else {
        ui->lcdCh2A->display(currentFormat(current));
    }
}

void MainWindow::UpdateVoltageSet(Protocol::Channel channel, double voltage) {
    if (channel == Protocol::Channel1) {
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

void MainWindow::UpdateCurrentSet(Protocol::Channel channel, double current) {
    if (channel == Protocol::Channel1) {
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

void MainWindow::UpdateOverCurrentProtectionSet(Protocol::Channel channel, double current) {
    if (channel == Protocol::Channel1) {
        ui->spinCh1OCP->setValue(current);
    } else {
        ui->spinCh2OCP->setValue(current);
    }
}

void MainWindow::UpdateOverVoltageProtectionSet(Protocol::Channel channel, double voltage) {
    if (channel == Protocol::Channel1) {
        ui->spinCh1OVP->setValue(voltage);
    } else {
        ui->spinCh2OVP->setValue(voltage);
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

QString MainWindow::currentFormat(double value) {
    return QString::asprintf("%05.03f", value);
}

QString MainWindow::voltageFormat(double value) {
    return QString::asprintf("%05.02f", value);
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

