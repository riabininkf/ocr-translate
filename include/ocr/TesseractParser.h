#pragma once

#include "ParsedImage.h"
#include "Parser.h"

#include <QObject>
#include <QScreen>
#include <tesseract/baseapi.h>

namespace ocr {
    class TesseractParser final : public Parser {
    public:
        explicit TesseractParser(int ratio, const std::string& tessdataPath);

        /**  @throws std::runtime_error */
        [[nodiscard]] ParsedImage parse(Language lang, QImage image) const override;

    public slots:
        void onScreenUpdated() override;

    private:
        struct ApiDeleter;

        int ratio;
        std::map<Language, std::unique_ptr<tesseract::TessBaseAPI, ApiDeleter> > pool;

        struct ApiDeleter {
            void operator()(tesseract::TessBaseAPI* api) const {
                if (api) {
                    api->End();
                    delete api;
                }
            }
        };

        struct ApiClearGuard {
            tesseract::TessBaseAPI* api;

            explicit ApiClearGuard(tesseract::TessBaseAPI* api) : api(api) {
            }

            ~ApiClearGuard() {
                if (api) {
                    api->Clear();
                }
            }
        };
    };
}