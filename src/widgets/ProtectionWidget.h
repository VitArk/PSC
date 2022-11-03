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

#ifndef PS_MANAGEMENT_PROTECTIONWIDGET_H
#define PS_MANAGEMENT_PROTECTIONWIDGET_H

#include <QGroupBox>

#include "SpinBoxDebounce.h"

class ProtectionWidget : public QGroupBox{
Q_OBJECT
public:
    explicit ProtectionWidget(QWidget *parent);

    void setLimitsOVP(double min, double max, double precision);
    void setLimitsOCP(double min, double max, double precision);

    void setValueOVP(double value);
    double valueOVP() const;

    void setValueOCP(double value);
    double valueOCP() const;

signals:
    void onSetOVP(double value);
    void onSetOCP(double value);

private:
    void setupInput();

private slots:
    void SetValueInternal(double value);

private:
    SpinBoxDebounce *mOVPSet;
    SpinBoxDebounce *mOCPSet;

    bool mBlockSignals = false;
};


#endif //PS_MANAGEMENT_PROTECTIONWIDGET_H
