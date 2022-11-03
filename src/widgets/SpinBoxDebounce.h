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

// Created by Vitalii Arkusha on 27.10.2022.
//

#ifndef PS_MANAGEMENT_SPINBOXDEBOUNCE_H
#define PS_MANAGEMENT_SPINBOXDEBOUNCE_H

#include <QDoubleSpinBox>
#include <QBasicTimer>

class SpinBoxDebounce : public QDoubleSpinBox {
Q_OBJECT
public:
    explicit SpinBoxDebounce(QWidget *parent);
    void setValue(double value);

signals:
    void onSetValue(double value);

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    void ValueChanged();

private:
    QBasicTimer mTimer;
    double      mExternalValue = 0.0;
};



#endif //PS_MANAGEMENT_SPINBOXDEBOUNCE_H
