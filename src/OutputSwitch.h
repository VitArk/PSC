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
    QString customStyle(const QString &color);
private:
    QString mBackgroundColorDefault;
};


#endif //PS_MANAGEMENT_OUTPUTSWITCH_H
