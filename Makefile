-include .env

APP_NAME = translator
VCPKG_OVERLAY_PORTS = ./vcpkg-overlay-ports
TESSDATA_FOLDER = ./tessdata
BUILD_DIR=./.build

.PHONY: build-osx-arm64 .build build-check-vars

build-osx-arm64:
	@platform=arm64-osx; \
	output_dir="${BUILD_DIR}/macos-arm64"; \
	$(MAKE) .build VCPKG_TARGET_TRIPLET=$$platform OUTPUT_DIR=$$output_dir; \
	mkdir -p $$output_dir/${APP_NAME}.app/Contents/Resources/tessdata; \
	cp -r ${TESSDATA_FOLDER}/* $$output_dir/${APP_NAME}.app/Contents/Resources/tessdata


.build: build-check-vars
	@cmake \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} \
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
		-DCMAKE_TOOLCHAIN_FILE=${VCPKG_TOOLCHAIN_FILE} \
		-DVCPKG_OVERLAY_PORTS=${VCPKG_OVERLAY_PORTS} \
		-DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET} \
		-G ${BUILD_SYSTEM_GENERATOR} \
		-S ./ \
		-B ${OUTPUT_DIR}

build-check-vars:
	@if [ -z "${CMAKE_MAKE_PROGRAM}" ]; then echo "Missing CMAKE_MAKE_PROGRAM"; exit 1; fi
	@if [ -z "${CMAKE_C_COMPILER}" ]; then echo "Missing CMAKE_C_COMPILER"; exit 1; fi
	@if [ -z "${CMAKE_CXX_COMPILER}" ]; then echo "Missing CMAKE_CXX_COMPILER"; exit 1; fi
	@if [ -z "${VCPKG_TOOLCHAIN_FILE}" ]; then echo "Missing VCPKG_TOOLCHAIN_FILE"; exit 1; fi
	@if [ -z "${VCPKG_OVERLAY_PORTS}" ]; then echo "Missing VCPKG_OVERLAY_PORTS"; exit 1; fi
	@if [ -z "${VCPKG_TARGET_TRIPLET}" ]; then echo "Missing VCPKG_TARGET_TRIPLET"; exit 1; fi
	@if [ -z "${BUILD_SYSTEM_GENERATOR}" ]; then echo "Missing BUILD_SYSTEM_GENERATOR"; exit 1; fi
	@if [ -z "${OUTPUT_DIR}" ]; then echo "Missing OUTPUT_DIR"; exit 1; fi

