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
// Created by Vitalii Arkusha on 03.11.2022.
//

#include "StatusBar.h"

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent) {

    setupUI();
}

void StatusBar::setupUI() {
    mDeviceInfo = new ClickableLabel(this);
    connect(mDeviceInfo, &ClickableLabel::onDoubleClick, this, &StatusBar::onDeviceInfoDoubleClick);
    addPermanentWidget(mDeviceInfo, 100);

    mLockStatus = new QLabel(this);
    addPermanentWidget(mLockStatus, 120);

    mDebugInfo = new QLabel(this);
    addPermanentWidget(mDebugInfo, 80);

    mConnectionStatus = new QLabel(this);
    addPermanentWidget(mConnectionStatus);
}

void StatusBar::setText(const QString &text, Target target) {
    switch (target) {
        case Target::DeviceInfo:
            mDeviceInfo->setText(text);
            break;
        case Target::LockStatus:
            mLockStatus->setText(text);
            break;
        case Target::ConnectionStatus:
            mConnectionStatus->setText(text);
            break;
        case Target::DebugInfo:
            mDebugInfo->setText(text);
            break;
    }
}