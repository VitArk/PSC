#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QSvgRenderer>
#include <QTextCharFormat>
#include <QGraphicsSvgItem>

const double vDialCorrection = 100.0;
const double aDialCorrection = 1000.0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(maximumSize());

    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->graphicsView->setStyleSheet("background-color: transparent;");


    connect(ui->rdoBtnOutIndependent, &QRadioButton::clicked, this, &MainWindow::slotIndependentMode);
    connect(ui->rdoBtnOutParallel, &QRadioButton::clicked, this, &MainWindow::slotParallelMode);
    connect(ui->rdoBtnOutSerial, &QRadioButton::clicked, this, &MainWindow::slotSerialMode);

    connect(ui->btnOVP, &QPushButton::toggled, this, &MainWindow::onOverVoltageProtection);
    connect(ui->btnOCP, &QPushButton::toggled, this, &MainWindow::onOverCurrentProtection);

    connect(ui->btnM1, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM2, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM3, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM4, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);
    connect(ui->btnM5, &QPushButton::clicked, this, &MainWindow::slotMemoryKeyChanged);

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

    connect(ui->spinCh1OVP, SIGNAL(valueChanged(double)), this, SLOT(slotOverProtectionChanged(double)));
    connect(ui->spinCh2OVP, SIGNAL(valueChanged(double)), this, SLOT(slotOverProtectionChanged(double)));
    connect(ui->spinCh1OCP, SIGNAL(valueChanged(double)), this, SLOT(slotOverProtectionChanged(double)));
    connect(ui->spinCh2OCP, SIGNAL(valueChanged(double)), this, SLOT(slotOverProtectionChanged(double)));

//    const QString message =
//            tr("%1x%2 SCENE: %3x%4").arg(size.width()).arg(size.height()).arg(view->size().width()).arg(view->size().height());
//    statusBar()->showMessage(message);

    slotControlValueChanged(); // ???
    showIndependentOutputConfiguration();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotIndependentMode() {
    showIndependentOutputConfiguration();
    emit onIndependentMode();
}

void MainWindow::slotSerialMode() {
    showSerialOutputConfiguration();
    emit onSerialMode();
}

void MainWindow::slotParallelMode() {
    showParallelOutputConfiguration();
    emit onParallelMode();
}

void MainWindow::showIndependentOutputConfiguration() {
    setEnableChannel(Channel1, true);
    setEnableChannel(Channel2, true);
    showOutputMode(Independent);
}

void MainWindow::showSerialOutputConfiguration() {
    setEnableChannel(Channel1, false);
    setEnableChannel(Channel2, true);
    showOutputMode(Serial);
}

void MainWindow::showParallelOutputConfiguration() {
    setEnableChannel(Channel1, false);
    setEnableChannel(Channel2, true);
    showOutputMode(Parallel);
}

void MainWindow::showOutputMode(OutputMode mode) {
    QString resourceName;
    QString labelText;
    switch (mode) {
        case Independent:
            resourceName = ":independent-mode";
            labelText = tr("Two independent channels U: 0-30V, I: 0-5A");
            break;
        case Serial:
            resourceName = ":serial-mode";
            labelText = tr("One channel U: 0-60V, I: 0-5A");
            break;
        case Parallel:
            resourceName = ":parallel-mode";
            labelText = tr("One channel U: 0-30V, I: 0-10A");
            break;
    }
    ui->lblOutputModeHint->setText(labelText);
    openSvg(QFile(resourceName).fileName());
}

void MainWindow::setEnableChannel(TChannel ch, bool enable) {
    if (ch == Channel1) {
        ui->lblCh1->setEnabled(enable);
        ui->groupBoxCh1->setEnabled(enable);
    } else {
        ui->lblCh2->setEnabled(enable);
        ui->groupBoxCh2->setEnabled(enable);
    }
}

void MainWindow::enableMemoryKey(TMemoryKey key) {
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
        if (ui->btnM1 == sender()) emit onMemoryKey(M1); else
        if (ui->btnM2 == sender()) emit onMemoryKey(M2); else
        if (ui->btnM3 == sender()) emit onMemoryKey(M3); else
        if (ui->btnM4 == sender()) emit onMemoryKey(M4); else
        if (ui->btnM5 == sender()) emit onMemoryKey(M5);
    } else {
        auto senderBtn = qobject_cast<QPushButton*>(sender());
        senderBtn->toggle();
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

void MainWindow::slotDialControlChanged() {
    ui->spinCh1V->setValue(ui->dialCh1V->value() / vDialCorrection);
    ui->spinCh2V->setValue(ui->dialCh2V->value() / vDialCorrection);
    ui->spinCh1A->setValue(ui->dialCh1A->value() / aDialCorrection);
    ui->spinCh2A->setValue(ui->dialCh2A->value() / aDialCorrection);

    slotDisplayVoltage(Channel1, ui->spinCh1V->value()); // temporary solution, must show real device's info.
    slotDisplayVoltage(Channel2, ui->spinCh2V->value()); // temporary solution, must show real device's info.
    slotDisplayCurrent(Channel1, ui->spinCh1A->value()); // temporary solution, must show real device's info.
    slotDisplayCurrent(Channel2, ui->spinCh2A->value()); // temporary solution, must show real device's info.
}

void MainWindow::slotSpinControlChanged() {
    ui->dialCh1V->setValue(ui->spinCh1V->value() * vDialCorrection);
    ui->dialCh2V->setValue(ui->spinCh2V->value() * vDialCorrection);
    ui->dialCh1A->setValue(ui->spinCh1A->value() * aDialCorrection);
    ui->dialCh2A->setValue(ui->spinCh2A->value() * aDialCorrection);

    slotControlValueChanged();
}

void MainWindow::slotControlValueChanged() {
    mDebouncedCh1V.setValue(ui->spinCh1V->value());
    mDebouncedCh1A.setValue(ui->spinCh1A->value());
    mDebouncedCh2V.setValue(ui->spinCh2V->value());
    mDebouncedCh2A.setValue(ui->spinCh2A->value());
}

void MainWindow::slotControlValueChangedDebounced(double value) {
    if (sender() == &mDebouncedCh1V) emit onVoltageChanged(Channel1, value); else
    if (sender() == &mDebouncedCh1A) emit onCurrentChanged(Channel1, value); else
    if (sender() == &mDebouncedCh2V) emit onVoltageChanged(Channel2, value); else
    if (sender() == &mDebouncedCh2A) emit onCurrentChanged(Channel2, value);
}

void MainWindow::slotDisplayVoltage(TChannel channel, double value) {
    if (channel == Channel1) {
        ui->lcdCh1V->display(value);
    } else {
        ui->lcdCh2V->display(value);
    }
}

void MainWindow::slotDisplayCurrent(TChannel channel, double value) {
    if (channel == Channel1) {
        ui->lcdCh1A->display(value);
    } else {
        ui->lcdCh2A->display(value);
    }
}

void MainWindow::slotOverProtectionChanged(double value) {
    if (sender() == ui->spinCh1OVP) emit onOverVoltageProtectionChanged(Channel1, value); else
    if (sender() == ui->spinCh2OVP) emit onOverVoltageProtectionChanged(Channel1, value); else
    if (sender() == ui->spinCh1OCP) emit onOverCurrentProtectionChanged(Channel2, value); else
    if (sender() == ui->spinCh2OCP) emit onOverCurrentProtectionChanged(Channel2, value);
}

void MainWindow::slotUpdateOutputStatus(TOutputStatus channel1, TOutputStatus channel2, bool outputOn) {
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
