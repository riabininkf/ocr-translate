#pragma once

#include "translation/GoogleTranslator.h"
#include "ui/CaptureFrame.h"
#include "ui/SystemTrayIcon.h"

class ScreenCaptureController final : public QObject {
    Q_OBJECT

public:
    explicit ScreenCaptureController(const ocr::Parser& parser, const translation::Translator& translator);

public slots:
    void onScreenCaptured(const QPoint& point, const QPixmap& pixmap);

    void onOutputLanguageChanged(translation::Language language);

signals:
    void resultUpdated(const QPixmap& pixmap);

    void resultUpdatedWithPoint(const QPoint& point, const QPixmap& pixmap);

private:
    ocr::Language inputLanguage = ocr::Language::Default;
    translation::Language outputLanguage = translation::Language::Russian;

    const ocr::Parser& parser;
    const translation::Translator& translator;
};