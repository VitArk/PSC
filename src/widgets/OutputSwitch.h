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

#ifndef PS_MANAGEMENT_OUTPUTSWITCH_H
#define PS_MANAGEMENT_OUTPUTSWITCH_H

#include <QWidget>
#include <QPushButton>
#include <QPalette>

class OutputSwitch : public QPushButton {
Q_OBJECT
public:
    OutputSwitch(QWidget *parent);

public slots:
    void SetSwitchOn(bool on);

private:
    void setupUI();
    static QString customStyle(const QString &color);
    void applyStyle(const QString &style);
private:
    QString mBackgroundColorDefault;
};


#endif //PS_MANAGEMENT_OUTPUTSWITCH_H
