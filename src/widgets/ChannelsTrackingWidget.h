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
// Created by Vitalii Arkusha on 01.11.2022.
//

#ifndef PS_MANAGEMENT_CHANNELSTRACKINGWIDGET_H
#define PS_MANAGEMENT_CHANNELSTRACKINGWIDGET_H

#include <QWidget>
#include <QRadioButton>
#include <QGraphicsView>
#include <QLabel>

#include "Global.h"

class ChannelsTrackingWidget : public QWidget {
Q_OBJECT
public:
    explicit ChannelsTrackingWidget(QWidget *parent);
    inline Global::ChannelsTracking channelsTracking() { return mTracking; }

signals:
    void onSetChannelsTracking(Global::ChannelsTracking tracking);

public slots:
    void SetChannelsTracking(Global::ChannelsTracking tracking);

private:
    void setupUI();
    QRadioButton *createRadioButton(const QString &text);
    QGraphicsView *createGraphicsView();

    void showChannelsTrackingMode(Global::ChannelsTracking tracking);

    void loadSvg(const QString &resource);

private slots:
    void ChannelsTrackingChanged();

private:
    QRadioButton*            mButtonIndependent;
    QRadioButton*            mButtonSerial;
    QRadioButton*            mButtonParallel;
    QGraphicsView*           mGraphicsView;
    QLabel*                  mLabelHint;
    Global::ChannelsTracking mTracking = Global::Independent;
};


#endif //PS_MANAGEMENT_CHANNELSTRACKINGWIDGET_H
