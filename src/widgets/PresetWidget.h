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
// Created by Vitalii Arkusha on 31.10.2022.
//

#ifndef PS_MANAGEMENT_PRESETWIDGET_H
#define PS_MANAGEMENT_PRESETWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QMap>
#include "Global.h"

class PresetWidget : public QWidget {
Q_OBJECT
public:
    explicit PresetWidget(QWidget *parent);

signals:
    void onPresetClicked(Global::MemoryKey keyNum);
    void onSaveClicked(Global::MemoryKey keyNum);

public slots:
    void SetActivePreset(Global::MemoryKey keyNum);

private:
    void setupUI();
    QPushButton* createButton(const QString &text);
    Global::MemoryKey checkedButtonKey();

private slots:
    void PresetClicked();
    void SaveClicked();

private:
    QMap<Global::MemoryKey, QPushButton*> mPresetButtons;
};


#endif //PS_MANAGEMENT_PRESETWIDGET_H
