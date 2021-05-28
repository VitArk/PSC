#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QSvgRenderer>
#include <QTextCharFormat>
#include <QGraphicsSvgItem>

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


//    const QString message =
//            tr("%1x%2 SCENE: %3x%4").arg(size.width()).arg(size.height()).arg(view->size().width()).arg(view->size().height());
//    statusBar()->showMessage(message);

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
        case M1:
            ui->btnM1->setChecked(true);
            break;
        case M2:
            ui->btnM2->setChecked(true);
            break;
        case M3:
            ui->btnM3->setChecked(true);
            break;
        case M4:
            ui->btnM4->setChecked(true);
            break;
        case M5:
            ui->btnM5->setChecked(true);
            break;
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
