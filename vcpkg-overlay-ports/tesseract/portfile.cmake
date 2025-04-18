vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO tesseract-ocr/tesseract
        REF 5.5.0
        SHA512 "206e7da2d28a6271217ff384b482aa45a50beee0c53327aa4fd3da7082dce83386c8b7600194cbc30282134013b6182a1bed9d128ed6378f2957d0b8d1770b2d"
)

vcpkg_configure_cmake(
        SOURCE_PATH ${SOURCE_PATH}
        PREFER_NINJA
        OPTIONS
        -DBUILD_TRAINING_TOOLS=OFF
        -DCMAKE_BUILD_TYPE=Release
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()
