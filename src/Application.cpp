//
// Created by Vitalii Arkusha on 12.05.2021.
//

#include "Application.h"
#include <QTimer>
#include <QDebug>

#include "MainWindow.h"

Application *Application::inst = nullptr;

Application::Application(int &argc, char **argv, int) : QApplication(argc, argv) {
    Q_ASSERT(!inst); // only one instance should be for app.
    inst = this;

    QApplication::setApplicationName("PSC");
    QApplication::setApplicationVersion("1.0");

    QTimer::singleShot(0, this, SLOT(slotAppRun()));

}

void Application::slotAppRun() {
    auto w = new MainWindow();
    connect(w, &MainWindow::onMemoryKey, this, &Application::slotOnMemoryKey);
    w->show();

    connect(w, &MainWindow::onVoltageChanged, this, &Application::slotVoltageChanged);
    connect(w, &MainWindow::onCurrentChanged, this, &Application::slotCurrentChanged);


    QTimer::singleShot(1000, this, [=](){
        w->enableMemoryKey(M2);
        w->slotUpdateOutputStatus(ConstantCurrent, ConstantVoltage, false);
    });
    QTimer::singleShot(2000, this, [=](){
        w->enableMemoryKey(M4);
        w->slotUpdateOutputStatus(ConstantVoltage, ConstantVoltage, false);
    });
    QTimer::singleShot(3000, this, [=](){
        w->enableMemoryKey(M5);
        w->slotUpdateOutputStatus(ConstantVoltage, ConstantCurrent, false);
    });
}

void Application::slotOnMemoryKey(TMemoryKey key) {
    qDebug() << "slotOnMemoryKey" << key;
}

void Application::slotVoltageChanged(TChannel channel, double value) {
    qDebug() << "slotVoltageChanged" << channel << value;
}

void Application::slotCurrentChanged(TChannel channel, double value) {
    qDebug() << "slotCurrentChanged" << channel << value;
}
