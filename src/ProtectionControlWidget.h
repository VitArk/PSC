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

#ifndef PS_MANAGEMENT_PROTECTIONCONTROLWIDGET_H
#define PS_MANAGEMENT_PROTECTIONCONTROLWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "Global.h"

class ProtectionControlWidget : public QWidget {
Q_OBJECT
public:
    explicit ProtectionControlWidget(QWidget *parent);

signals:
    void onProtectionChanged(Global::OutputProtection protection);

public slots:
    void SetActiveProtection(Global::OutputProtection protection);

private:
    void setupUI();
    QPushButton* createButton(const QString &text);

private slots:
    void OnButtonClick();

private:
    QPushButton* mButtonOVP;
    QPushButton* mButtonOCP;
};


#endif //PS_MANAGEMENT_PROTECTIONCONTROLWIDGET_H
