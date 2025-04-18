#pragma once

#include <QApplication>
#include <QSystemTrayIcon>

class SystemTrayIcon final : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit SystemTrayIcon(QObject* parent = nullptr);

signals:
    void leftClicked();
};