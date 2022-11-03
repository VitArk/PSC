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

#include "SpinBoxDebounce.h"
#include <QDebug>

const int DebounceTime = 1000;

SpinBoxDebounce::SpinBoxDebounce(QWidget *parent) : QDoubleSpinBox(parent) {
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(ValueChanged()));
}

void SpinBoxDebounce::setValue(double value) {
    mExternalValue = value;
    if (underMouse() || mTimer.isActive()) {
        return;
    }

    QDoubleSpinBox::setValue(value);
}

void SpinBoxDebounce::ValueChanged() {
    if (value() != mExternalValue) {
        mTimer.start(DebounceTime, this);
    }
}

void SpinBoxDebounce::timerEvent(QTimerEvent *event) {
    mTimer.stop();
    if (value() != mExternalValue) {
        emit onSetValue(value());
    }
}
