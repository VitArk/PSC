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
