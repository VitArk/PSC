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

//
// Created by Vitalii Arkusha on 01.11.2022.
//

#include "ChannelsTrackingWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QFile>
#include <QDebug>
#include <QStyle>

ChannelsTrackingWidget::ChannelsTrackingWidget(QWidget *parent)
: QWidget(parent) {
    setupUI();
    showChannelsTrackingMode(mTracking);
}

void ChannelsTrackingWidget::setupUI() {
    auto vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);

    auto hLayout = new QHBoxLayout();
    mButtonIndependent = createRadioButton(tr("Independent"));
    hLayout->addWidget(mButtonIndependent);

    mButtonSerial = createRadioButton(tr("Series"));
    hLayout->addWidget(mButtonSerial);

    mButtonParallel = createRadioButton(tr("Parallel"));
    hLayout->addWidget(mButtonParallel);

    vLayout->addLayout(hLayout);

    mGraphicsView = createGraphicsView();
    vLayout->addWidget(mGraphicsView);

    mLabelHint = new QLabel("Two independent channels U: 0-30V, I: 0-5A", this);
    mLabelHint->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(mLabelHint);

    setLayout(vLayout);
}

QRadioButton *ChannelsTrackingWidget::createRadioButton(const QString &text) {
    auto button = new QRadioButton(text, this);
    connect(button, &QRadioButton::clicked, this, &ChannelsTrackingWidget::ChannelsTrackingChanged);

    return button;
}

QGraphicsView *ChannelsTrackingWidget::createGraphicsView() {
    auto graphicsView = new QGraphicsView(this);
    graphicsView->setScene(new QGraphicsScene(this));
    graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    graphicsView->setStyleSheet("background-color: transparent;");

    graphicsView->setEnabled(false);
    graphicsView->setMinimumSize(QSize(0, 0));
    graphicsView->setAutoFillBackground(true);
    graphicsView->setFrameShape(QFrame::NoFrame);
    graphicsView->setFrameShadow(QFrame::Plain);
    graphicsView->setLineWidth(0);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    graphicsView->setInteractive(false);
    graphicsView->setSceneRect(QRectF(0, 0, 0, 0));

    return graphicsView;
}

void ChannelsTrackingWidget::SetChannelsTracking(Global::ChannelsTracking tracking) {
    if (channelsTracking() == tracking) {
        return;
    }

    mTracking = tracking;
    showChannelsTrackingMode(tracking);
}

void ChannelsTrackingWidget::ChannelsTrackingChanged() {
    auto tracking = Global::Independent;
    if (mButtonParallel->isChecked()) {
        tracking = Global::Parallel;
    } else if (mButtonSerial->isChecked()) {
        tracking = Global::Serial;
    }

    emit onSetChannelsTracking(tracking);
}

void ChannelsTrackingWidget::showChannelsTrackingMode(Global::ChannelsTracking tracking) {
    switch (tracking) {
        case Global::Independent:
            mButtonIndependent->setChecked(true);
            loadSvg(QFile(":independent-mode").fileName());
            mLabelHint->setText(tr("Two independent channels U: 0-30V, I: 0-5A"));
            break;
        case Global::Serial:
            mButtonSerial->setChecked(true);
            loadSvg(QFile(":serial-mode").fileName());
            mLabelHint->setText(tr("One channel U: 0-60V, I: 0-5A"));
            break;
        case Global::Parallel:
            mButtonParallel->setChecked(true);
            loadSvg(QFile(":parallel-mode").fileName());
            mLabelHint->setText(tr("One channel U: 0-30V, I: 0-10A"));
            break;
    }
}


void ChannelsTrackingWidget::loadSvg(const QString &resource) {
    QGraphicsScene *s = mGraphicsView->scene();
    Q_ASSERT(s);
    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(resource));
    if (!svgItem->renderer()->isValid())
        return;

    s->clear();
    mGraphicsView->resetTransform();

    auto item = svgItem.take();
    item->setFlags(QGraphicsItem::ItemClipsToShape);
    item->setCacheMode(QGraphicsItem::NoCache);
    item->setZValue(0);
    s->addItem(item);
}