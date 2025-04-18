vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO Skycoder42/QHotkey
        REF 1.5.0
        SHA512 "c57135864e976c0881bca320f39ff98ec6973cb226902fe051b844ad29d106593bdf5524335b8f83a62cecac362af95b61964bc5fa6a47abcb7adf668bcc14c1"
)

vcpkg_configure_cmake(SOURCE_PATH ${SOURCE_PATH} PREFER_NINJA OPTIONS -DQT_DEFAULT_MAJOR_VERSION=6 -DQHOTKEY_EXAMPLES=ON)

vcpkg_install_cmake()
vcpkg_copy_pdbs()
