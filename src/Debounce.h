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
// Created by Vitalii Arkusha on 30.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_CONTROLDEBOUNCE_H
#define POWER_SUPPLY_CONTROLLER_CONTROLDEBOUNCE_H

#include <QObject>
#include <QTimer>

class Debounce : public QObject {
Q_OBJECT
public:
    explicit Debounce(QObject *parent = nullptr);
    void setValue(double value);

signals:
    void onChangedDebounced(double value);

private:
    QTimer mTimer;
    double mValue = 0.0;

private slots:
    void Timeout();
};


#endif //POWER_SUPPLY_CONTROLLER_CONTROLDEBOUNCE_H
