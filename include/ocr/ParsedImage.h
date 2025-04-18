#pragma once

#include "Language.h"

#include <QImage>
#include <qfont.h>

namespace ocr {
    static constexpr auto defaultLineColor = QColor(211, 211, 211);

    class ParsedImage {
    public:
        class Block;

        explicit ParsedImage(Language language, std::vector<Block> blocks, std::unique_ptr<QImage> image);

        [[nodiscard]] Language getLanguage() const;

        [[nodiscard]] const QImage& getImage() const;

        void traverse(const std::function<void(Block&, int)>& func);

        class Block {
        public:
            explicit Block(std::string text, const QFont& font, std::vector<QRect> lines, QImage& image);

            void updateText(const std::string& newText);

            [[nodiscard]] std::string getText() const;

        private:
            std::string text;

            QFont font;
            QColor lineColor = defaultLineColor;
            QImage& image;
            std::vector<QRect> lines;
        };

    private:
        const Language language;

        std::vector<Block> blocks;
        std::unique_ptr<QImage> image;
    };
}