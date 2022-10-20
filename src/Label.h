//
// Created by Vitalii Arkusha on 20.10.2022.
//

#ifndef PS_MANAGEMENT_LABEL_H
#define PS_MANAGEMENT_LABEL_H

#include <QLabel>
#include <QMoveEvent>
#include <QBasicTimer>

class Label : public QLabel {
Q_OBJECT
public:
    explicit Label(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~Label() = default;

signals:
    void onClick();
    void onDoubleClick();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void timerEvent(QTimerEvent *) override;
private:
    QBasicTimer mTimer;
};


#endif //PS_MANAGEMENT_LABEL_H
