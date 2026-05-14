# PdfiumPrebuilt.cmake
#
# Resolves PDFium headers + shared library for PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM.
#
# Resolution order:
#   1) Explicit cache paths: PDFIUM_INCLUDE_DIR + PDFIUM_LIBRARY (highest priority).
#   2) PDFIUM_ROOT: expects include/fpdfview.h and lib/libpdfium.{dylib,so} or bin/pdfium.dll.
#   3) When PDFDOCUMENTVIEW_FETCH_PDFIUM=ON: download + extract a pinned bblanchon/pdfium-binaries
#      tarball into the build tree (checksum-verified). Supported hosts: macOS, Linux (glibc
#      x64/arm64 by default), Windows x64/x86/arm64. For Linux musl, set
#      PDFDOCUMENTVIEW_PDFIUM_LINUX_ABI=MUSL or supply PDFIUM_ROOT / PDFIUM_* manually.
#      If CMake mis-detects the host CPU (rare cross scenarios), set cache variable
#      PDFDOCUMENTVIEW_CMAKE_ARCH (e.g. arm64, x86_64, aarch64, AMD64, ARM64) to force
#      which bblanchon asset is selected.
#
# Fetch defaults ON on macOS, Linux (glibc), and Windows; set PDFDOCUMENTVIEW_FETCH_PDFIUM=OFF
# and supply PDFIUM_* for air-gapped or policy-restricted environments.

if(NOT PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
    return()
endif()

if(APPLE)
    option(PDFDOCUMENTVIEW_FETCH_PDFIUM
        "Download and extract bblanchon/pdfium-binaries for this host (default ON on macOS)"
        ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    option(PDFDOCUMENTVIEW_FETCH_PDFIUM
        "Download and extract bblanchon/pdfium-binaries for this host (default ON on Linux glibc)"
        ON)
elseif(WIN32)
    option(PDFDOCUMENTVIEW_FETCH_PDFIUM
        "Download and extract bblanchon/pdfium-binaries for this host (default ON on Windows; "
        "set OFF and supply PDFIUM_ROOT or PDFIUM_INCLUDE_DIR+PDFIUM_LIBRARY for air-gapped builds)"
        ON)
else()
    option(PDFDOCUMENTVIEW_FETCH_PDFIUM
        "Download and extract bblanchon/pdfium-binaries for this host"
        OFF)
endif()

set(PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG "chromium/7834" CACHE STRING
    "bblanchon/pdfium-binaries release tag (e.g. chromium/7834). Used when fetch is enabled.")

set(PDFDOCUMENTVIEW_PDFIUM_LINUX_ABI "GLIBC" CACHE STRING
    "Linux prebuilt ABI when using fetch: GLIBC (default) or MUSL (Alpine-style).")
set_property(CACHE PDFDOCUMENTVIEW_PDFIUM_LINUX_ABI PROPERTY STRINGS GLIBC MUSL)

set(PDFDOCUMENTVIEW_CMAKE_ARCH "" CACHE STRING
    "If non-empty, override CPU detection for PDFium prebuilt tarball selection only "
    "(e.g. arm64, aarch64, x86_64, AMD64, ARM64). Leave empty for CMAKE_*_SYSTEM_PROCESSOR.")

# SHA-256 of GitHub release assets (see release JSON digests for the tag above).
set(_pdfdocumentview_pdfium_mac_arm64_sha256
    "2b733774416de02482281c0abc7589b08dc908896ecef2bfc31a85c5b5ffd572")
set(_pdfdocumentview_pdfium_mac_x64_sha256
    "fcfed5eaf8fe9a761577d626dff651227600a52fc5f933c461447564361bb036")
set(_pdfdocumentview_pdfium_linux_x64_sha256
    "e10b18234af3e988b3021547786e574b8905a24511067f14773f29c9cac12365")
set(_pdfdocumentview_pdfium_linux_arm64_sha256
    "5381c1e7436dc6811ba86f4444fdcaccadd90fdb2a06f12ee81bfba96689ee36")
set(_pdfdocumentview_pdfium_linux_musl_x64_sha256
    "b6c5c8f0ff24fc09bf19f3572620294938bd4a35efd97630bec1669984a407c3")
set(_pdfdocumentview_pdfium_linux_musl_arm64_sha256
    "1737a6f0d26f16ec46bb82ad4a31cfaf92ba7709686121306be4d0245f867d20")
set(_pdfdocumentview_pdfium_win_x64_sha256
    "0abfacf8aacc919f98eff2c3efa2927c3dc9faf07e31f22558a1f1cf93809612")
set(_pdfdocumentview_pdfium_win_x86_sha256
    "3d07f10c3fafedfce7bd1cf74effff878bed79d844cdf9399e9aca1ed58a3af6")
set(_pdfdocumentview_pdfium_win_arm64_sha256
    "6a60c3dc5ca7ac1afe53c83e07fe3f6747b48e30723fa4917bdf3ea59436e5a6")

set(PDFIUM_INCLUDE_DIR "" CACHE PATH "Directory containing PDFium public headers (fpdfview.h).")
set(PDFIUM_LIBRARY "" CACHE FILEPATH "Path to libpdfium shared library.")
set(PDFIUM_ROOT "" CACHE PATH
    "Root of an extracted pdfium-binaries layout (include/ + lib/). Overrides fetch when set.")

function(_pdfdocumentview_apply_pdfium_root root_dir)
    if(NOT EXISTS "${root_dir}/include/fpdfview.h")
        message(FATAL_ERROR "PDFDocumentView: PDFIUM_ROOT='${root_dir}' has no include/fpdfview.h")
    endif()
    if(APPLE AND EXISTS "${root_dir}/lib/libpdfium.dylib")
        set(_lib "${root_dir}/lib/libpdfium.dylib")
    elseif(UNIX AND NOT APPLE AND EXISTS "${root_dir}/lib/libpdfium.so")
        set(_lib "${root_dir}/lib/libpdfium.so")
    elseif(WIN32 AND EXISTS "${root_dir}/bin/pdfium.dll")
        set(_lib "${root_dir}/bin/pdfium.dll")
    else()
        message(FATAL_ERROR
            "PDFDocumentView: PDFIUM_ROOT='${root_dir}' — expected libpdfium under lib/ (or pdfium.dll under bin/ on Windows).")
    endif()
    set(PDFIUM_INCLUDE_DIR "${root_dir}/include" CACHE PATH "PDFium headers directory" FORCE)
    set(PDFIUM_LIBRARY "${_lib}" CACHE FILEPATH "PDFium shared library" FORCE)
    if(EXISTS "${root_dir}/LICENSE")
        set(PDFIUM_PREBUILT_LICENSE_FILE "${root_dir}/LICENSE" CACHE FILEPATH
            "LICENSE from the PDFium binary drop (e.g. bblanchon/pdfium-binaries)" FORCE)
    else()
        unset(PDFIUM_PREBUILT_LICENSE_FILE CACHE)
    endif()
endfunction()

function(_pdfdocumentview_download_extract_pdfium asset_file sha256_hex)
    string(REPLACE "/" "%2F" _pdfdocumentview_pdfium_tag_url "${PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG}")
    set(_pdfdocumentview_pdfium_url
        "https://github.com/bblanchon/pdfium-binaries/releases/download/${_pdfdocumentview_pdfium_tag_url}/${asset_file}")

    set(_pdfdocumentview_pdfium_stamp "${CMAKE_BINARY_DIR}/CMakeFiles/pdfium-prebuilt-${PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG}-${asset_file}.stamp")
    set(_pdfdocumentview_pdfium_tgz "${CMAKE_BINARY_DIR}/_deps/pdfium/${asset_file}")
    set(_pdfdocumentview_pdfium_extract "${CMAKE_BINARY_DIR}/_deps/pdfium/extracted/${PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG}/${asset_file}")

    if(NOT EXISTS "${_pdfdocumentview_pdfium_stamp}")
        message(STATUS "PDFDocumentView: downloading PDFium prebuilt (${asset_file}) …")
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/_deps/pdfium")
        file(DOWNLOAD
            "${_pdfdocumentview_pdfium_url}"
            "${_pdfdocumentview_pdfium_tgz}"
            EXPECTED_HASH "SHA256=${sha256_hex}"
            TLS_VERIFY ON)
        file(REMOVE_RECURSE "${_pdfdocumentview_pdfium_extract}")
        file(MAKE_DIRECTORY "${_pdfdocumentview_pdfium_extract}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E tar xzf "${_pdfdocumentview_pdfium_tgz}"
            WORKING_DIRECTORY "${_pdfdocumentview_pdfium_extract}"
            RESULT_VARIABLE _pdfdocumentview_tar_rc)
        if(NOT _pdfdocumentview_tar_rc EQUAL 0)
            message(FATAL_ERROR "PDFDocumentView: extracting '${_pdfdocumentview_pdfium_tgz}' failed (${_pdfdocumentview_tar_rc})")
        endif()
        file(WRITE "${_pdfdocumentview_pdfium_stamp}" "extracted\n")
    endif()

    _pdfdocumentview_apply_pdfium_root("${_pdfdocumentview_pdfium_extract}")
    set(PDFIUM_FETCH_EXTRACT_ROOT "${_pdfdocumentview_pdfium_extract}" CACHE INTERNAL "Extracted PDFium prebuilt root" FORCE)
    message(STATUS "PDFDocumentView: PDFium prebuilt ${asset_file} @ ${PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG}")
endfunction()

# User supplied explicit include + lib
if(PDFIUM_INCLUDE_DIR AND PDFIUM_LIBRARY)
    if(NOT EXISTS "${PDFIUM_INCLUDE_DIR}/fpdfview.h")
        message(FATAL_ERROR "PDFDocumentView: PDFIUM_INCLUDE_DIR must contain fpdfview.h")
    endif()
    if(NOT EXISTS "${PDFIUM_LIBRARY}")
        message(FATAL_ERROR "PDFDocumentView: PDFIUM_LIBRARY points to a missing file: '${PDFIUM_LIBRARY}'")
    endif()
    unset(PDFIUM_PREBUILT_LICENSE_FILE CACHE)
    message(STATUS "PDFDocumentView: PDFium from cache (PDFIUM_INCLUDE_DIR / PDFIUM_LIBRARY)")
elseif(PDFIUM_ROOT)
    _pdfdocumentview_apply_pdfium_root("${PDFIUM_ROOT}")
    message(STATUS "PDFDocumentView: PDFium from PDFIUM_ROOT=${PDFIUM_ROOT}")
elseif(PDFDOCUMENTVIEW_FETCH_PDFIUM AND APPLE)
    if(PDFDOCUMENTVIEW_CMAKE_ARCH)
        set(_cpu "${PDFDOCUMENTVIEW_CMAKE_ARCH}")
    else()
        set(_cpu "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    endif()
    if(_cpu MATCHES "arm64|aarch64")
        set(_asset "pdfium-mac-arm64.tgz")
        set(_sha "${_pdfdocumentview_pdfium_mac_arm64_sha256}")
    elseif(_cpu MATCHES "x86_64|AMD64|i.86|i686|x64")
        set(_asset "pdfium-mac-x64.tgz")
        set(_sha "${_pdfdocumentview_pdfium_mac_x64_sha256}")
    else()
        message(FATAL_ERROR
            "PDFDocumentView: unsupported macOS CPU for pinned PDFium fetch ('${_cpu}'). "
            "Set PDFIUM_ROOT / PDFIUM_* or PDFDOCUMENTVIEW_CMAKE_ARCH to arm64|aarch64 or x86_64.")
    endif()
    _pdfdocumentview_download_extract_pdfium("${_asset}" "${_sha}")
elseif(PDFDOCUMENTVIEW_FETCH_PDFIUM AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(PDFDOCUMENTVIEW_CMAKE_ARCH)
        set(_cpu "${PDFDOCUMENTVIEW_CMAKE_ARCH}")
    else()
        set(_cpu "${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    if(PDFDOCUMENTVIEW_PDFIUM_LINUX_ABI STREQUAL "MUSL")
        if(_cpu MATCHES "aarch64|arm64")
            set(_asset "pdfium-linux-musl-arm64.tgz")
            set(_sha "${_pdfdocumentview_pdfium_linux_musl_arm64_sha256}")
        else()
            set(_asset "pdfium-linux-musl-x64.tgz")
            set(_sha "${_pdfdocumentview_pdfium_linux_musl_x64_sha256}")
        endif()
    else()
        if(_cpu MATCHES "aarch64|arm64")
            set(_asset "pdfium-linux-arm64.tgz")
            set(_sha "${_pdfdocumentview_pdfium_linux_arm64_sha256}")
        elseif(_cpu MATCHES "armv7|armv7l|arm")
            message(FATAL_ERROR
                "PDFDocumentView: fetch for Linux ARMv7 is not pinned in-tree; set PDFIUM_ROOT / PDFIUM_* "
                "or extend cmake/PdfiumPrebuilt.cmake with the matching bblanchon asset + SHA256.")
        else()
            set(_asset "pdfium-linux-x64.tgz")
            set(_sha "${_pdfdocumentview_pdfium_linux_x64_sha256}")
        endif()
    endif()
    _pdfdocumentview_download_extract_pdfium("${_asset}" "${_sha}")
elseif(PDFDOCUMENTVIEW_FETCH_PDFIUM AND WIN32)
    if(PDFDOCUMENTVIEW_CMAKE_ARCH)
        set(_cpu "${PDFDOCUMENTVIEW_CMAKE_ARCH}")
    else()
        set(_cpu "${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    if(_cpu MATCHES "ARM64|aarch64|arm64")
        set(_asset "pdfium-win-arm64.tgz")
        set(_sha "${_pdfdocumentview_pdfium_win_arm64_sha256}")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(_asset "pdfium-win-x86.tgz")
        set(_sha "${_pdfdocumentview_pdfium_win_x86_sha256}")
    else()
        set(_asset "pdfium-win-x64.tgz")
        set(_sha "${_pdfdocumentview_pdfium_win_x64_sha256}")
    endif()
    _pdfdocumentview_download_extract_pdfium("${_asset}" "${_sha}")
else()
    message(STATUS
        "PDFDocumentView: PDFium fetch is disabled or not used on this host; "
        "set PDFIUM_ROOT or PDFIUM_INCLUDE_DIR+PDFIUM_LIBRARY, or enable PDFDOCUMENTVIEW_FETCH_PDFIUM=ON "
        "when a bblanchon asset exists (see README and docs/off-mac-pdfium-build.md).")
    message(FATAL_ERROR
        "PDFDocumentView: PDFIUM backend requires PDFium. "
        "On macOS/Linux/Windows with fetch ON (default on those hosts), CMake downloads a pinned "
        "prebuilt (SHA-256 verified); otherwise set PDFIUM_ROOT or PDFIUM_INCLUDE_DIR+PDFIUM_LIBRARY, "
        "or set PDFDOCUMENTVIEW_FETCH_PDFIUM=OFF for air-gapped builds. "
        "See README \"PDFium prebuilt\" and docs/off-mac-pdfium-build.md.")
endif()

add_library(PDFDocumentView::PDFium SHARED IMPORTED GLOBAL)
set_target_properties(PDFDocumentView::PDFium PROPERTIES
    IMPORTED_LOCATION "${PDFIUM_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${PDFIUM_INCLUDE_DIR}")
message(STATUS
    "PDFDocumentView: optional imported target PDFDocumentView::PDFium defined "
    "(integrators may link it explicitly; the library links the resolved PDFium shared library privately).")

unset(_pdfdocumentview_pdfium_mac_arm64_sha256)
unset(_pdfdocumentview_pdfium_mac_x64_sha256)
unset(_pdfdocumentview_pdfium_linux_x64_sha256)
unset(_pdfdocumentview_pdfium_linux_arm64_sha256)
unset(_pdfdocumentview_pdfium_linux_musl_x64_sha256)
unset(_pdfdocumentview_pdfium_linux_musl_arm64_sha256)
unset(_pdfdocumentview_pdfium_win_x64_sha256)
unset(_pdfdocumentview_pdfium_win_x86_sha256)
unset(_pdfdocumentview_pdfium_win_arm64_sha256)
