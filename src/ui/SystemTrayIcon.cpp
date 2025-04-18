#include "ui/SystemTrayIcon.h"
#include <QStyle>

SystemTrayIcon::SystemTrayIcon(QObject *parent) : QSystemTrayIcon(parent) {
    setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    show();

    // todo
    connect(this, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger ||
                reason == QSystemTrayIcon::DoubleClick) {
                emit leftClicked();
            }
        });
}
