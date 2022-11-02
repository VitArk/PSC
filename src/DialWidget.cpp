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

const int DialSizeW = 60;
const int DialSizeH = 60;

const int DebounceTime = 1000;

DialWidget::DialWidget(const QString &title, QWidget *parent) : QGroupBox(title, parent) {
    setLayout(new QVBoxLayout(this));

    mDial = new QDial(this);
    layout()->addWidget(mDial);

    mSpinBox = new QDoubleSpinBox(this);
    mSpinBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    mSpinBox->setValue(0.0);

    layout()->addWidget(mSpinBox);

    setupDial();
    setupSpinBox();
    setPrecision(0.01);

    connect(mDial, &QDial::valueChanged, this, &DialWidget::ValueChanged);
    connect(mSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ValueChanged()));
}

void DialWidget::setupDial() {
    mDial->setMaximumSize(DialSizeW, DialSizeH);
    mDial->setMinimumSize(DialSizeW,DialSizeH);
    mDial->setMinimum(0);
    mDial->setMaximum(3000);
    mDial->setSingleStep(10);
    mDial->setPageStep(100);
    mDial->setWrapping(false);
    mDial->setNotchesVisible(true);
}

void DialWidget::setupSpinBox() {
    mSpinBox->setFrame(false);
    mSpinBox->setMinimum(0.0);
    mSpinBox->setMaximum(30.0);
    mSpinBox->setSingleStep(mPrecision);
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
    qDebug() << value;
    mSpinBox->setValue(value);
}

void DialWidget::setLimits(double min, double max, double precision) {
    mSpinBox->blockSignals(true);
    mDial->blockSignals(true);

    setPrecision(precision);
    mSpinBox->setMinimum(min);
    qDebug() << max;
    mSpinBox->setMaximum(max);
    mDial->setMinimum(toInteger(min));
    mDial->setMaximum(toInteger(max));

    mSpinBox->blockSignals(false);
    mDial->blockSignals(false);
}







