#pragma once

#include "Language.h"

#include <string>

namespace translation {
    class Translator {
    public:
        virtual ~Translator() = default;

        [[nodiscard]] virtual std::string translate(Language from, Language to, const std::string& text) const = 0;
    };
}