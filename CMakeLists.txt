cmake_minimum_required(VERSION 3.25.1)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(translator)

#find_package(PkgConfig REQUIRED)

set(
        HEADERS
        include/ocr/ParsedImage.h
        include/ocr/Parser.h
        include/ocr/TesseractParser.h
        include/translation/GoogleTranslator.h
        include/translation/Translator.h
        include/ui/CaptureFrame.h
        include/ui/Viewer.h
        include/ui/SystemTrayIcon.h
        include/controllers/ScreenCaptureController.h
)

set(SOURCES
        src/main.cpp
        src/ocr/TesseractParser.cpp
        src/ocr/ParsedImage.cpp
        src/ui/CaptureFrame.cpp
        src/ui/Viewer.cpp
        src/ui/SystemTrayIcon.cpp
        src/controllers/ScreenCaptureController.cpp
        src/translation/GoogleTranslator.cpp
        include/ocr/Language.h
        include/translation/Language.h
)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(Qt6 COMPONENTS Widgets Core DBus Concurrent Gui REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Widgets
        Qt6::Core
        Qt6::DBus
        Qt6::Concurrent
        Qt6::Gui
)

find_package(cpr REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

find_package(nlohmann_json 3.11.3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

find_package(QHotkey CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE qhotkey)

find_package(Leptonica REQUIRED leptonica)
target_link_libraries(${PROJECT_NAME} PRIVATE leptonica)

pkg_search_module(TESSERACT REQUIRED tesseract)
target_link_directories(${PROJECT_NAME} PRIVATE ${TESSERACT_LIBRARY_DIRS})
#target_compile_options(${PROJECT_NAME} PUBLIC ${TESSERACT_CFLAGS_OTHER})
target_link_libraries(${PROJECT_NAME} PRIVATE tesseract)
