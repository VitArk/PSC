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

#ifndef PS_MANAGEMENT_DIALWIDGET_H
#define PS_MANAGEMENT_DIALWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QDial>
#include <QDoubleSpinBox>
#include <QBasicTimer>

class DialWidget : public QGroupBox {
    Q_OBJECT
public:
    explicit DialWidget(const QString &title, QWidget *parent);
    void setPrecision(double precision);

    void setSuffix(const QString &suffix);
    void setLimits(double min, double max, double precision);
    void setValue(double value);
    double value() const;

signals:
    void onSetValue(double value);

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void setupUI();
    QDial* createDial();
    QDoubleSpinBox* createSpinBox();

    inline int toInteger(double value) const;
    inline double toDouble(int value) const;

private slots:
    void ValueChanged();

private:
    QDial*           mDial;
    QDoubleSpinBox*  mSpinBox;

    double           mPrecision = 0.01;
    QBasicTimer      mTimer;

    double           mExternalValue = 0.0;
};


#endif //PS_MANAGEMENT_DIALWIDGET_H
