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
// Created by Vitalii Arkusha on 28.10.2022.
//

#include "DisplayWidget.h"
#include <QFormLayout>

const int FONT_SIZE_UNITS = 24;
const int FONT_SIZE_MODE = 13;

const char* const STYLE_BG_RED = "background-color: rgb(210, 0, 0)";
const char* const STYLE_BG_GREEN = "background-color: rgb(0, 210, 0)";
const char* const STYLE_BG_NONE = "";

DisplayWidget::DisplayWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void DisplayWidget::setupUI(){
    auto layout = new QFormLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    mDisplayVoltage = createLCD();
    displayVoltage(30.00);
    layout->setWidget(0, QFormLayout::LabelRole, mDisplayVoltage);

    mDisplayCurrent = createLCD();
    displayCurrent(1.100);
    layout->setWidget(1, QFormLayout::LabelRole, mDisplayCurrent);

    auto layoutV = new QVBoxLayout();
    layoutV->setContentsMargins(-1, 10, -1, 10);
    mLabelCV = createLabel(tr("c.v."), FONT_SIZE_MODE);
    layoutV->addWidget(mLabelCV);
    layoutV->addWidget(createLabel(tr("V"), FONT_SIZE_UNITS));
    layout->setLayout(0, QFormLayout::FieldRole, layoutV);

    auto layoutA = new QVBoxLayout();
    layoutA->setContentsMargins(-1, 10, -1, 10);
    mLabelCC = createLabel(tr("c.c."), FONT_SIZE_MODE);
    layoutA->addWidget(mLabelCC);
    layoutA->addWidget(createLabel(tr("A"), FONT_SIZE_UNITS), 0, Qt::AlignBottom);
    layout->setLayout(1, QFormLayout::FieldRole, layoutA);
}

QLabel *DisplayWidget::createLabel(const QString &text, int fontSize) {
    auto label = new QLabel(text, this);
    QFont font;
    font.setPointSize(fontSize);
    label->setFont(font);

    return label;
}

QLCDNumber *DisplayWidget::createLCD() {
    auto lcd = new QLCDNumber(this);
    lcd->setMinimumSize(QSize(175, 75));
    lcd->setMaximumSize(QSize(175, 75));
    lcd->setFrameShape(QFrame::StyledPanel);
    lcd->setLineWidth(1);
    lcd->setSmallDecimalPoint(true);
    lcd->setDigitCount(5);
    lcd->setMode(QLCDNumber::Dec);
    lcd->setSegmentStyle(QLCDNumber::Flat);

    return lcd;
}

void DisplayWidget::displayCurrent(double value) {
    mDisplayCurrent->display(QString::asprintf("%05.03f", value));
}

void DisplayWidget::displayVoltage(double value) {
    mDisplayVoltage->display(QString::asprintf("%05.02f", value));
}

void DisplayWidget::constantCurrent() {
    mLabelCC->setStyleSheet(STYLE_BG_RED);
    mLabelCV->setStyleSheet(STYLE_BG_NONE);
}

void DisplayWidget::constantVoltage() {
    mLabelCV->setStyleSheet(STYLE_BG_GREEN);
    mLabelCC->setStyleSheet(STYLE_BG_NONE);
}
