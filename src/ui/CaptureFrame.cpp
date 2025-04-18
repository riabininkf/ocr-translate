#include "ui/CaptureFrame.h"

#include <iostream>

CaptureFrame::CaptureFrame(QScreen* screen, QWidget* parent): QWidget(parent), screen(screen) {
    updateSize();
    setWindowOpacity(0.1);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setMouseTracking(true);

    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::CrossCursor);
}

void CaptureFrame::onScreenUpdated() {
    if (const auto newScreen = qobject_cast<QScreen*>(sender())) {
        screen = newScreen;
        updateSize();
    }
}

void CaptureFrame::updateSize() {
    setFixedWidth(screen->size().width());
    setFixedHeight(screen->size().height());
}

void CaptureFrame::onScreenCaptureStarted() {
    show();
    originalPixmap = screen->grabWindow(0);
}

void CaptureFrame::mousePressEvent(QMouseEvent* event) {
    selector.setTopLeft(event->pos());

    isSelecting = true;
    grabMouse();
    QApplication::setOverrideCursor(Qt::CrossCursor);
}

void CaptureFrame::mouseMoveEvent(QMouseEvent* event) {
    if (isSelecting) {
        selector.setBottomRight(event->pos());
        update();
    }
}

void CaptureFrame::mouseReleaseEvent(QMouseEvent* event) {
    if (isSelecting) {
        selector.setBottomRight(event->pos());

        isSelecting = false;
        releaseMouse();
        QApplication::restoreOverrideCursor();
        hide();
        saveCapturedRegion();
    }
}

void CaptureFrame::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, originalPixmap);

    if (isSelecting) {
        painter.setPen(QPen(Qt::gray, 4));
        painter.setBrush(QBrush(Qt::gray, Qt::Dense1Pattern));
        painter.drawRect(selector.normalized());
    }
}

void CaptureFrame::saveCapturedRegion() {
    const QRect rect(selector.topLeft() * screen->devicePixelRatio(),
                     selector.bottomRight() * screen->devicePixelRatio());

    const QRect normalizedRect = rect.normalized();

    emit screenCaptured(selector.topLeft(),originalPixmap.copy(normalizedRect));
}