//
// Created by Vitalii Arkusha on 30.09.2022.
//

#ifndef PSC_SETTINGS_H
#define PSC_SETTINGS_H

#include <QSettings>
#include <QString>

class Settings : public QObject {
public:
    explicit Settings(QObject *parent = nullptr);

    int serialPortBaudRate() const;
    void setSerialPortBaudRate(int baudRate);

    QString serialPortName() const;
    void setSerialPortName(QString name);

private:
    QSettings mSettings;

    inline void setValue(const QString &key, const QVariant &value) {
        mSettings.setValue(key, value);
        mSettings.sync();
    }
};


#endif //PSC_SETTINGS_H
