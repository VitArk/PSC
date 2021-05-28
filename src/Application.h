//
// Created by Vitalii Arkusha on 12.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_APPLICATION_H
#define POWER_SUPPLY_CONTROLLER_APPLICATION_H

#include <QObject>
#include <QApplication>
#include "Commons.h"

#if defined(App)
#undef App
#endif
#define App (Application::i())

class Application : public QApplication {
    Q_DISABLE_COPY(Application)

Q_OBJECT

public:
    explicit Application(int &argc, char **argv, int = ApplicationFlags);
    ~Application() override = default;

    static Application *i() { return inst; }

private:
    static Application *inst;

private slots:
    void slotAppRun();

    static void slotOnMemoryKey(TMemoryKey key);
};


#endif //POWER_SUPPLY_CONTROLLER_APPLICATION_H
