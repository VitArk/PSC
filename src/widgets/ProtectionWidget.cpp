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

#include "ProtectionWidget.h"
#include <QFormLayout>
#include <QLabel>
#include <QDebug>
#include <QScopedValueRollback>

ProtectionWidget::ProtectionWidget(QWidget *parent) : QGroupBox(parent) {
    auto layout = new QFormLayout(this);
    setLayout(layout);
    setTitle(tr("Output protection"));

    mOVPSet = new SpinBoxDebounce(this);
    layout->addRow(new QLabel(tr("OVP Value"), this), mOVPSet);

    mOCPSet = new SpinBoxDebounce(this);
    layout->addRow(new QLabel(tr("OCP Value"), this), mOCPSet);

    setupInput();

    connect(mOCPSet, &SpinBoxDebounce::onSetValue, this, &ProtectionWidget::SetValueInternal);
    connect(mOVPSet, &SpinBoxDebounce::onSetValue, this, &ProtectionWidget::SetValueInternal);
}

void ProtectionWidget::setupInput() {
    QScopedValueRollback scope(mBlockSignals, false);
    mBlockSignals = true;

    mOVPSet->setFrame(false);
    mOVPSet->setSuffix(tr("V"));
    mOVPSet->setDecimals(2);
    setLimitsOVP(0.00, 30.00, 0.01);

    mOCPSet->setFrame(false);
    mOCPSet->setSuffix(tr("A"));
    mOCPSet->setDecimals(3);
    setLimitsOCP(0.000, 3.000, 0.001);
}

void ProtectionWidget::setLimitsOVP(double min, double max, double precision) {
    QScopedValueRollback scope(mBlockSignals, false);
    mBlockSignals = true;

    mOVPSet->setMinimum(min);
    mOVPSet->setMaximum(max);
    mOVPSet->setSingleStep(precision);
}

void ProtectionWidget::setLimitsOCP(double min, double max, double precision) {
    QScopedValueRollback scope(mBlockSignals, false);
    mBlockSignals = true;

    mOCPSet->setMinimum(min);
    mOCPSet->setMaximum(max);
    mOCPSet->setSingleStep(precision);
}

void ProtectionWidget::setValueOVP(double value) {
    mOVPSet->setValue(value);
}

double ProtectionWidget::valueOVP() const {
    return mOVPSet->value();
}

void ProtectionWidget::setValueOCP(double value) {
    mOCPSet->setValue(value);
}

double ProtectionWidget::valueOCP() const {
    return mOCPSet->value();
}

void ProtectionWidget::SetValueInternal(double value) {
    if (mBlockSignals) {
        return;
    }

    if (sender() == mOCPSet) {
        emit onSetOCP(value);
    } else {
        emit onSetOVP(value);
    }
}


