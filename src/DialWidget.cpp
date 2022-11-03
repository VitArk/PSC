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
// Created by Vitalii Arkusha on 27.10.2022.
//

#include "DialWidget.h"
#include <QLayout>
#include <QDebug>

const int DebounceTime = 1000;

DialWidget::DialWidget(const QString &title, QWidget *parent) : QGroupBox(title, parent) {
    setupUI();
}

void DialWidget::setupUI() {
    setFixedWidth(100);
    auto vLayout = new QVBoxLayout(this);
    setLayout(vLayout);

    mDial = createDial();
    vLayout->addWidget(mDial, 0, Qt::AlignHCenter);

    mSpinBox = createSpinBox();
    vLayout->addWidget(mSpinBox);
}


QDial* DialWidget::createDial() {
    auto dial = new QDial(this);
    dial->setMaximumSize(QSize(60, 60));
    dial->setMinimumSize(QSize(60, 60));
    dial->setMinimum(0);
    dial->setMaximum(3000);
    dial->setSingleStep(10);
    dial->setPageStep(100);
    dial->setWrapping(false);
    dial->setNotchesVisible(true);
    connect(dial, &QDial::valueChanged, this, &DialWidget::ValueChanged);

    return dial;
}

QDoubleSpinBox* DialWidget::createSpinBox() {
    auto spinBox = new QDoubleSpinBox(this);
    spinBox->setFrame(false);
    spinBox->setMinimum(0.0);
    spinBox->setMaximum(30.0);
    spinBox->setSingleStep(0.01);
    spinBox->setValue(0.0);
    connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(ValueChanged()));

    return spinBox;
}

void DialWidget::ValueChanged() {
    mTimer.stop();
    if (sender() == mDial) {
        mSpinBox->blockSignals(true);
        mSpinBox->setValue(toDouble(mDial->value()));
        mSpinBox->blockSignals(false);
    } else {
        mDial->blockSignals(true);
        mDial->setValue(toInteger(mSpinBox->value()));
        mDial->blockSignals(false);
    }

    if (value() != mExternalValue) {
        mTimer.start(DebounceTime, this);
    }
}

int DialWidget::toInteger(double value) const {
    if (mPrecision == 0)
        return 0;

    return int(1.0/mPrecision * value);
}

double DialWidget::toDouble(int value) const {
    return value * mPrecision;
}

void DialWidget::timerEvent(QTimerEvent *event) {
    mTimer.stop();
    emit onSetValue(value());
}

void DialWidget::setPrecision(double precision) {
    mPrecision = precision;
    mSpinBox->setSingleStep(precision);
    mSpinBox->setDecimals(QString::number(precision).count()-2);
}

void DialWidget::setSuffix(const QString &suffix) {
    mSpinBox->setSuffix(suffix);
}

double DialWidget::value() const {
    return mSpinBox->value();
}

void DialWidget::setValue(double value) {
    mExternalValue = value;
    if (mDial->underMouse() || mSpinBox->underMouse() || mTimer.isActive()) {
        return;
    }
    mSpinBox->setValue(value);
}

void DialWidget::setLimits(double min, double max, double precision) {
    mSpinBox->blockSignals(true);
    mDial->blockSignals(true);

    setPrecision(precision);
    mSpinBox->setMinimum(min);
    mSpinBox->setMaximum(max);
    mDial->setMinimum(toInteger(min));
    mDial->setMaximum(toInteger(max));

    mSpinBox->blockSignals(false);
    mDial->blockSignals(false);
}








