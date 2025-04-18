#pragma once

#include "Viewer.h"

#include <QApplication>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>

class CaptureFrame final : public QWidget {
    Q_OBJECT

public:
    explicit CaptureFrame(QScreen* screen, QWidget* parent = nullptr);

    ~CaptureFrame() override = default;

public slots:
    void onScreenCaptureStarted();

    void onScreenUpdated();

signals:
    void screenCaptured(const QPoint& position, const QPixmap& pixmap);

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

private:
    QScreen* screen;
    QRect selector;
    QPixmap originalPixmap;

    bool isSelecting = false;

    void saveCapturedRegion();

    void updateSize();
};