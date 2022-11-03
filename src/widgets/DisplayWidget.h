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
// Created by Vitalii Arkusha on 28.10.2022.
//

#ifndef PS_MANAGEMENT_DISPLAYWIDGET_H
#define PS_MANAGEMENT_DISPLAYWIDGET_H

#include <QLCDNumber>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>

class DisplayWidget : public QWidget {
Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent);

    void displayVoltage(double value);
    void displayCurrent(double value);
    void constantVoltage();
    void constantCurrent();

private:
    void setupUI();
    QLabel *createLabel(const QString &text, int fontSize);
    QLCDNumber *createLCD();

private:
    QLCDNumber* mDisplayVoltage;
    QLCDNumber* mDisplayCurrent;
    QLabel*     mLabelCV;
    QLabel*     mLabelCC;
};


#endif //PS_MANAGEMENT_DISPLAYWIDGET_H
