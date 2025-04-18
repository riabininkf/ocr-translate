#include "ocr/ParsedImage.h"

#include "ocr/Parser.h"
#include "translation/GoogleTranslator.h"

#include <QFuture>
#include <QGraphicsBlurEffect>
#include <QGraphicsScene>
#include <QLabel>
#include <QPainter>
#include <QRandomGenerator>

namespace ocr {
    ParsedImage::Block::Block(std::string text, const QFont& font, std::vector<QRect> lines, QImage& image)
        : text(std::move(text)), font(font), image(image), lines(std::move(lines)) {
    }

    std::string ParsedImage::Block::getText() const { return text; }

    void ParsedImage::Block::updateText(const std::string& newText) {
        text = newText;

        QPainter p(&image);
        p.setPen(Qt::black);
        p.setBrush(Qt::white);
        p.setFont(font);

        const QFontMetrics fontMetrics(font);
        const int spaceWidth = fontMetrics.horizontalAdvance(' ');

        QStringList words = QString::fromStdString(text).split(' ');

        int wordIdx = 0;
        for (auto& line : lines) {
            int width = 0;
            QStringList lineWords;

            for (; wordIdx < words.size(); ++wordIdx) {
                QString word = words[wordIdx];
                const int size = fontMetrics.horizontalAdvance(word) + spaceWidth;

                if (width + size > line.width()) {
                    break;
                }

                lineWords.emplace_back(word);
                width += size;
            }

            p.fillRect(line, lineColor);
            p.drawText(line, Qt::AlignJustify, lineWords.join(" "));
        }
    }

    ParsedImage::ParsedImage(const Language language, std::vector<Block> blocks, std::unique_ptr<QImage> image)
        : language(language), blocks(std::move(blocks)), image(std::move(image)) {
    }

    Language ParsedImage::getLanguage() const { return language; }

    const QImage& ParsedImage::getImage() const { return *image; }

    void ParsedImage::traverse(const std::function<void(Block&, int)>& func) {
        for (int i = 0; i < blocks.size(); ++i) {
            func(blocks[i], i);
        }
    }
}