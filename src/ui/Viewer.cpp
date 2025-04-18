#include "ui/Viewer.h"

#include <QApplication>
#include <QFileDialog>
#include <QFuture>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <iostream>

Viewer::Viewer(QWidget* parent) : QMainWindow(parent), content(std::make_unique<QLabel>(this)) {
    setCentralWidget(content.get());
    content->setAlignment(Qt::AlignCenter);

    setWindowFlags(Qt::FramelessWindowHint);

    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_TranslucentBackground);
}

void Viewer::onResultUpdatedWithPoint(const QPoint& point, const QPixmap& pixmap) {
    position = point;
    move(point);
    onResultUpdated(pixmap);
}

void Viewer::onResultUpdated(const QPixmap& pixmap) {
    content->setPixmap(pixmap);
    content->resize(pixmap.width(), pixmap.height());
    centralWidget()->resize(pixmap.width(), pixmap.height());
    show();
}

void Viewer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        position = event->globalPosition().toPoint();
    }

    QMainWindow::mousePressEvent(event);
}

void Viewer::mouseMoveEvent(QMouseEvent* event) {
    if (isDragging) {
        const QPoint currentPosition = event->globalPosition().toPoint();
        const QPoint diff = currentPosition - position;
        move(pos() + diff);
        position = currentPosition;
    }

    QMainWindow::mouseMoveEvent(event);
}

void Viewer::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }

    QMainWindow::mouseReleaseEvent(event);
}

void Viewer::changeEvent(QEvent* event) {
    if (event->type() == QEvent::ActivationChange && !isActiveWindow()) {
        close();
    }

    QMainWindow::changeEvent(event);
}

void Viewer::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }

    QMainWindow::keyPressEvent(event);
}