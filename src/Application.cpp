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

    QTimer::singleShot(1000, this, [=](){
        w->enableMemoryKey(M2);
    });
    QTimer::singleShot(2000, this, [=](){
        w->enableMemoryKey(M4);
    });
    QTimer::singleShot(3000, this, [=](){
        w->enableMemoryKey(M5);
    });
}

void Application::slotOnMemoryKey(TMemoryKey key) {
    qDebug() << "slotOnMemoryKey" << key;
}
