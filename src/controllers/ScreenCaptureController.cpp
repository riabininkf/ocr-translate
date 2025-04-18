#include "controllers/ScreenCaptureController.h"

#include "translation/GoogleTranslator.h"
#include "ui/CaptureFrame.h"

#include <QThreadPool>
#include <iostream>

static std::unordered_map<ocr::Language, translation::Language> languageMap = {
    {ocr::Language::English, translation::Language::English},
    {ocr::Language::Default, translation::Language::Default},
};

ScreenCaptureController::ScreenCaptureController(const ocr::Parser& parser, const translation::Translator& translator)
    : parser(parser), translator(translator) {
}

void ScreenCaptureController::onScreenCaptured(const QPoint& point, const QPixmap& pixmap) {
    emit resultUpdatedWithPoint(point, pixmap);

    //todo use QConcurrent
    QCoreApplication::processEvents();

    QImage image = pixmap.toImage();
    QPainter painter(&image);

    ocr::ParsedImage parsedImage = parser.parse(inputLanguage, image);

    std::vector<std::string> texts;
    parsedImage.traverse([&texts](const ocr::ParsedImage::Block& block, int) -> void {
        texts.emplace_back(block.getText());
    });

    std::vector<std::future<void> > tasks;
    tasks.reserve(texts.size());
    std::mutex mux;
    for (auto& text : texts) {
        tasks.emplace_back(std::async(std::launch::deferred | std::launch::async, [&,this] {
            const auto tr = translator.translate(translation::Language::Unknown, outputLanguage, text);
            // const auto tr = translator.translate(languageMap.at(parsedImage.getLanguage()), outputLanguage, text);
            std::lock_guard lock(mux);

            text = tr;
        }));
    }

    for (auto& task : tasks) {
        task.wait();
    }

    parsedImage.traverse([&](ocr::ParsedImage::Block& block, const int idx) {
        block.updateText(texts[idx]);
    });

    emit resultUpdated(QPixmap::fromImage(parsedImage.getImage()));
}

void ScreenCaptureController::onOutputLanguageChanged(const translation::Language language) {
    outputLanguage = language;
}