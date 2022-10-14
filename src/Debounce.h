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
    void slotTimeout();
};


#endif //POWER_SUPPLY_CONTROLLER_CONTROLDEBOUNCE_H
