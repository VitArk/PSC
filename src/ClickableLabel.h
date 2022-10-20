//
// Created by Vitalii Arkusha on 20.10.2022.
//

#ifndef PS_MANAGEMENT_CLICKABLELABEL_H
#define PS_MANAGEMENT_CLICKABLELABEL_H

#include <QLabel>
#include <QMoveEvent>
#include <QBasicTimer>

class ClickableLabel : public QLabel {
Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel() = default;

signals:
    void onClick();
    void onDoubleClick();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void timerEvent(QTimerEvent *) override;
private:
    QBasicTimer mTimer;
};


#endif //PS_MANAGEMENT_CLICKABLELABEL_H
