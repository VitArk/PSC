//
// Created by Vitalii Arkusha on 20.10.2022.
//

#include "Label.h"

Label::Label(QWidget* parent, Qt::WindowFlags f)
        : QLabel(parent) {
}

void Label::mousePressEvent(QMouseEvent* event) {
    if (mTimer.isActive()) {
        mTimer.stop();
        emit onDoubleClick();
    }
    else {
        mTimer.start(300, this);
    }
}

void Label::timerEvent(QTimerEvent *) {
    mTimer.stop();
    emit onClick();
}




