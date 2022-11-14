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

//
// Created by Vitalii Arkusha on 02.11.2022.
//

#include "OutputSwitch.h"
#include <QDebug>
#include <QVBoxLayout>

const char* const BG_GREEN="rgb(0, 210, 0)";

OutputSwitch::OutputSwitch(QWidget *parent)
: QPushButton(parent) {
    mBackgroundColorDefault = palette().window().color().name();
    setupUI();
}

void OutputSwitch::setupUI() {
    setText(tr("OUTPUT OFF"));
    setMinimumSize(QSize(0, 80));
    setCheckable(true);
    setChecked(false);
    applyStyle(customStyle(mBackgroundColorDefault));
    connect(this, &QPushButton::clicked, this, &OutputSwitch::SetSwitchOn);
}

void OutputSwitch::SetSwitchOn(bool on) {
    if (on) {
        setChecked(true);
        setText(tr("OUTPUT ON"));
        applyStyle(customStyle(BG_GREEN));
    } else {
        setChecked(false);
        setText(tr("OUTPUT OFF"));
        applyStyle(customStyle(mBackgroundColorDefault));
    }
}

QString OutputSwitch::customStyle(const QString &color) {
   // return QString("background-color: %1;border:1px;").arg(color);
    return QString("background-color: %1; "
                   "border-style: outset;"
                   "border-width: 1px;"
                   "border-color: beige;").arg(color);
}

void OutputSwitch::applyStyle(const QString &style)
{
#ifndef Q_OS_WIN
    setStyleSheet(style);
#endif
}
