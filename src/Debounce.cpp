//
// Created by Vitalii Arkusha on 30.05.2021.
//

#include "Debounce.h"

const int debounceTimeoutMs = 250;

Debounce::Debounce(QObject *parent) :
        QObject(parent) {
    mTimer.setInterval(debounceTimeoutMs);
    mTimer.setSingleShot(true);
    connect(&mTimer, &QTimer::timeout, this, &Debounce::slotTimeout);
}

void Debounce::setValue(double value) {
    if (mValue != value && mValue > 0.0) {
        mTimer.start();
    }
    mValue = value;
}

void Debounce::slotTimeout() {
    emit onChangedDebounced(mValue);
}
