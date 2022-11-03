//
// Created by Vitalii Arkusha on 02.11.2022.
//

#include "OutputSwitch.h"
#include <QDebug>
#include <QVBoxLayout>

const char* const BG_GREEN="rgb(0, 210, 0)";

OutputSwitch::OutputSwitch(QWidget *parent)
: QPushButton(parent) {
    mBackgroundColorDefault = palette().background().color().name();
    setupUI();
}

void OutputSwitch::setupUI() {
    setText(tr("OUTPUT OFF"));
    setMinimumSize(QSize(0, 80));
    setCheckable(true);
    setChecked(false);
    setStyleSheet(customStyle(mBackgroundColorDefault));
    connect(this, &QPushButton::clicked, this, &OutputSwitch::SetSwitchOn);
}

void OutputSwitch::SetSwitchOn(bool on) {
    if (on) {
        setChecked(true);
        setText(tr("OUTPUT ON"));
        setStyleSheet(customStyle(BG_GREEN));
    } else {
        setChecked(false);
        setText(tr("OUTPUT OFF"));
        setStyleSheet(customStyle(mBackgroundColorDefault));
    }
}

QString OutputSwitch::customStyle(const QString &color) {
    return QString("background-color: %1;border:none;").arg(color);
}