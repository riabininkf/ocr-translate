#pragma once

#include "Translator.h"

#include <string>

namespace translation {
    class GoogleTranslator final : public Translator {
    public:
        GoogleTranslator() = default;

        /**  @throws std::runtime_error */
        [[nodiscard]] std::string translate(Language from, Language to, const std::string& text) const override;
    };
}