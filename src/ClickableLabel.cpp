//
// Created by Vitalii Arkusha on 20.10.2022.
//

#include "ClickableLabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
        : QLabel(parent) {
}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    if (mTimer.isActive()) {
        mTimer.stop();
        emit onDoubleClick();
    }
    else {
        mTimer.start(300, this);
    }
}

void ClickableLabel::timerEvent(QTimerEvent *) {
    mTimer.stop();
    emit onClick();
}




