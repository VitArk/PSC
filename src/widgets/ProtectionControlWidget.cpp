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
// Created by Vitalii Arkusha on 01.11.2022.
//

#include "ProtectionControlWidget.h"
#include <QHBoxLayout>

ProtectionControlWidget::ProtectionControlWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void ProtectionControlWidget::setupUI() {
    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(0, 0, 0, 0);

    mButtonOVP = createButton(tr("OVP"));
    layout()->addWidget(mButtonOVP);

    mButtonOCP = createButton(tr("OCP"));
    layout()->addWidget(mButtonOCP);
}

QPushButton *ProtectionControlWidget::createButton(const QString &text) {
    auto btn = new QPushButton(text, this);
    btn->setCheckable(true);
    btn->setChecked(false);
    connect(btn, &QPushButton::clicked, this, &ProtectionControlWidget::OnButtonClick);

    return btn;
}

void ProtectionControlWidget::SetActiveProtection(Global::OutputProtection protection) {
    mButtonOCP->setChecked(protection == Global::OverCurrentProtectionOnly || protection == Global::OutputProtectionAllEnabled);
    mButtonOVP->setChecked(protection == Global::OverVoltageProtectionOnly || protection == Global::OutputProtectionAllEnabled);
}

void ProtectionControlWidget::OnButtonClick() {
    if (mButtonOCP->isChecked() && mButtonOVP->isChecked()) {
        return emit onProtectionChanged(Global::OutputProtectionAllEnabled);
    }

    if (mButtonOCP->isChecked()) {
        return emit onProtectionChanged(Global::OverCurrentProtectionOnly);
    }

    if (mButtonOVP->isChecked()) {
        return emit onProtectionChanged(Global::OverVoltageProtectionOnly);
    }

    emit onProtectionChanged(Global::OutputProtectionAllDisabled);
}
