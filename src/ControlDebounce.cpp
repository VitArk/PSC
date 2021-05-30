//
// Created by Vitalii Arkusha on 30.05.2021.
//

#include "ControlDebounce.h"

const int debounceTimeoutMs = 500;

ControlDebounce::ControlDebounce(QObject *parent) :
        QObject(parent) {
    mTimer.setInterval(debounceTimeoutMs);
    mTimer.setSingleShot(true);
    connect(&mTimer, &QTimer::timeout, this, &ControlDebounce::slotTimeout);
}

void ControlDebounce::setValue(double value) {
    if (mValue != value && mValue > 0.0) {
        mTimer.start();
    }
    mValue = value;
}

void ControlDebounce::slotTimeout() {
    emit onChangedDebounced(mValue);
}
