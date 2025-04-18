#pragma once

#include "ocr/TesseractParser.h"

#include <QLabel>
#include <QMainWindow>
#include <tesseract/baseapi.h>

class Viewer final : public QMainWindow {
    Q_OBJECT

public:
    explicit Viewer(QWidget* parent = nullptr);

public slots:
    void onResultUpdated(const QPixmap& pixmap);

    void onResultUpdatedWithPoint(const QPoint& point, const QPixmap& pixmap);

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    void changeEvent(QEvent* event) override;

private:
    bool isDragging = false;
    std::unique_ptr<QLabel> content;
    QPoint position;
};