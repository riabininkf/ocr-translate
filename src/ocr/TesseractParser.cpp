#include "ocr/TesseractParser.h"

#include "ocr/ParsedImage.h"

#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>
#include <iostream>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <QCoreApplication>
#include <QDir>

namespace ocr {
    static std::unordered_map<Language, std::string> languageToCode = {
        {Language::English, "eng"},
        {Language::Default, "deu"},
        {Language::Russian, "rus"},
        {Language::Default, "eng+deu+rus"},
    };

    static std::unordered_map<char, char> replace = {
        {'\n', ' '},
        {'|', 'I'}
    };

    static std::optional<ParsedImage::Block> parseBlock(int ratio, QImage& image, tesseract::ResultIterator& iterator);

    TesseractParser::TesseractParser(const int ratio, const std::string& tessdataPath) : ratio(ratio) {
        for (const auto& [lang, code] : languageToCode) {
            auto api = std::unique_ptr<tesseract::TessBaseAPI, ApiDeleter>(new tesseract::TessBaseAPI(), ApiDeleter());

            if (api->Init(tessdataPath.c_str(), code.c_str())) {
                throw std::runtime_error("Could not initialize tesseract.");
            }

            pool[lang] = std::move(api);
        }
    }

    void TesseractParser::onScreenUpdated() {
        if (const auto screen = qobject_cast<QScreen*>(sender())) {
            ratio = static_cast<int>(screen->devicePixelRatio());
        }
    }

    ParsedImage TesseractParser::parse(Language lang, QImage image) const {
        const auto api = pool.at(lang).get();
        const ApiClearGuard apiClearGuard(api);

        const std::unique_ptr<PIX, void (*)(Pix*)> pixImage(
            pixCreate(image.width(), image.height(), image.depth()),
            [](Pix* pix) { pixDestroy(&pix); });

        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                pixSetPixel(pixImage.get(), x, y, image.pixel(x, y));
            }
        }

        auto editableImage = std::make_unique<QImage>(std::move(image));

        api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
        api->SetImage(pixImage.get());
        api->Recognize(nullptr);

        const std::unique_ptr<tesseract::ResultIterator> iterator(api->GetIterator());
        if (iterator == nullptr) {
            throw std::runtime_error("Could not get iterator");
        }

        std::vector<ParsedImage::Block> blocks;

        do {
            if (auto block = parseBlock(ratio, *editableImage, *iterator); block.has_value()) {
                blocks.emplace_back(block.value());
            }
        } while (iterator->Next(tesseract::RIL_TEXTLINE));

        //todo
        return ParsedImage(Language::Default, blocks, std::move(editableImage));
    }

    std::optional<ParsedImage::Block> parseBlock(const int ratio, QImage& image, tesseract::ResultIterator& iterator) {
        double fontSize = 0;

        std::vector<std::pair<int, int> > parsedLines;
        std::ostringstream oss;

        QRect blockRect(0, 0, 0, 0);
        blockRect.setLeft(INT_MAX);
        blockRect.setBottom(INT_MAX);

        do {
            int left, top, right, bottom;
            if (iterator.BoundingBox(tesseract::RIL_TEXTLINE, &left, &top, &right, &bottom)) {
                if (blockRect.top() == 0) {
                    blockRect.setTop(top);
                }

                blockRect.setLeft(std::min(blockRect.left(), left));
                blockRect.setRight(std::max(blockRect.right(), right));

                auto str = std::unique_ptr<char[]>(iterator.GetUTF8Text(tesseract::RIL_TEXTLINE));

                const double fs = static_cast<double>(bottom - top) / ratio;
                fontSize = fontSize == 0 ? fs : (fontSize + fs) / 2;

                parsedLines.emplace_back(top, bottom);
                oss << str.get();

                if (iterator.IsAtFinalElement(tesseract::RIL_BLOCK, tesseract::RIL_TEXTLINE)) {
                    blockRect.setBottom(bottom);
                    break;
                }
            }
        } while (iterator.Next(tesseract::RIL_TEXTLINE));

        if (parsedLines.empty()) {
            return std::nullopt;
        }

        std::vector<QRect> lines;
        lines.reserve(parsedLines.size());

        for (auto& line : parsedLines) {
            lines.emplace_back(
                blockRect.left() / ratio,
                std::get<0>(line) / ratio - 2,
                (blockRect.right() - blockRect.left()) / ratio,
                (std::get<1>(line) - std::get<0>(line)) / ratio + 2
                );
        }

        auto text = oss.str();
        std::transform(text.begin(), text.end(), text.begin(), [](const char c) {
            return replace.contains(c) ? replace.at(c) : c;
        });

        return ParsedImage::Block(text, QFont("Arial", static_cast<int>(fontSize)), lines, image);
    }
}