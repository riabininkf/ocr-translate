#pragma once


#include <QImage>
#include <QObject>

namespace ocr {
    class Parser : public QObject {
        Q_OBJECT

    public:
        [[nodiscard]] virtual ParsedImage parse(Language lang, QImage image) const = 0;

    public slots:
        virtual void onScreenUpdated() = 0;
    };
}