cmake_minimum_required(VERSION 3.25)

# Public API surface for 0.1.x is governed by docs/adrs/0004-public-api-surface-freeze.md (semver aligned with this line).
project(PDFDocumentView VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Qt discovery: append ENV{QT_PREFIX} to CMAKE_PREFIX_PATH when set (non-destructive;
# does not replace a CMAKE_PREFIX_PATH passed on the command line). file(TO_CMAKE_PATH)
# normalizes native separators / path lists (notably on Windows).
if(DEFINED ENV{QT_PREFIX})
    string(STRIP "$ENV{QT_PREFIX}" _pdfdocumentview_qt_prefix_env)
    if(NOT _pdfdocumentview_qt_prefix_env STREQUAL "")
        file(TO_CMAKE_PATH "${_pdfdocumentview_qt_prefix_env}" _pdfdocumentview_qt_prefix)
        list(APPEND CMAKE_PREFIX_PATH "${_pdfdocumentview_qt_prefix}")
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)

option(PDFDOCUMENTVIEW_BUILD_EXAMPLES "Build integration demo (basic)" ON)
option(PDFDOCUMENTVIEW_BUILD_DOCS "Generate Doxygen HTML API reference (requires Doxygen)" OFF)
option(PDFDOCUMENTVIEW_INSTALL "Install the library and CMake package config" ON)

# Machine-readable render backend selection (see ADR-0001). PDFIUM links a
# concrete PDFium artifact when enabled; Poppler can be stub or real (optional).
set(PDFDOCUMENTVIEW_RENDER_BACKEND "PDFIUM" CACHE STRING
    "PDF render backend target: PDFIUM (default) or POPPLER. Allowed values: PDFIUM; POPPLER.")
set_property(CACHE PDFDOCUMENTVIEW_RENDER_BACKEND PROPERTY STRINGS PDFIUM POPPLER)
set(_pdfdocumentview_render_backends PDFIUM POPPLER)
if(NOT PDFDOCUMENTVIEW_RENDER_BACKEND IN_LIST _pdfdocumentview_render_backends)
    string(JOIN ", " _pdfdocumentview_render_backend_choices ${_pdfdocumentview_render_backends})
    message(FATAL_ERROR
        "PDFDOCUMENTVIEW_RENDER_BACKEND must be one of: ${_pdfdocumentview_render_backend_choices} (got '${PDFDOCUMENTVIEW_RENDER_BACKEND}')")
endif()
message(STATUS "PDFDocumentView: PDFDOCUMENTVIEW_RENDER_BACKEND=${PDFDOCUMENTVIEW_RENDER_BACKEND}")

# Install / CPack: omit shipped PDFium shared library from the install tree and from CPack
# archives when RUNTIME_WITHOUT_SHIPPED_PDFIUM (PDFIUM builds only; integrator supplies PDFium).
# POPPLER builds ignore this (no shipped PDFium component).
set(PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET "FULL" CACHE STRING
    "Install+CPack: FULL = all components including shipped PDFium (PDFIUM). RUNTIME_WITHOUT_SHIPPED_PDFIUM = omit installed libpdfium/pdfium.dll (PDFIUM only).")
set_property(CACHE PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET PROPERTY STRINGS FULL RUNTIME_WITHOUT_SHIPPED_PDFIUM)
set(_pdfdocumentview_pkg_component_choices FULL RUNTIME_WITHOUT_SHIPPED_PDFIUM)
if(NOT PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET IN_LIST _pdfdocumentview_pkg_component_choices)
    string(JOIN ", " _pdfdocumentview_pkg_joined ${_pdfdocumentview_pkg_component_choices})
    message(FATAL_ERROR
        "PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET must be one of: ${_pdfdocumentview_pkg_joined} "
        "(got '${PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET}')")
endif()
set(_pdfdocumentview_omit_shipped_pdfium OFF)
if(PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET STREQUAL "RUNTIME_WITHOUT_SHIPPED_PDFIUM"
        AND PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
    set(_pdfdocumentview_omit_shipped_pdfium ON)
endif()

# Minimum PDFium revision covered by the vendored legal snapshot. The
# canonical SHA lives in `third_party/pdfium/REVISION`; this module exposes it
# as a cache variable so maintainers can bump or override it without editing
# CMake. Newer upstream revisions remain allowed for external sources — this
# documents the bundled-notices floor, not a runtime equality check. See
# `cmake/PDFiumMinimumRevision.cmake` and `third_party/pdfium/README.md`.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/PDFiumMinimumRevision.cmake")
message(STATUS
    "PDFDocumentView: PDFium minimum (bundled notices): "
    "${PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA}")

if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/PdfiumPrebuilt.cmake")
endif()

set(PDFDOCUMENTVIEW_HAVE_POPPLER_QT6 FALSE)
if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "POPPLER")
    option(PDFDOCUMENTVIEW_POPPLER_REAL
        "Link Poppler-Qt6 for a real Poppler backend (GPL obligations on distributors). Requires poppler-qt6 (pkg-config)."
        OFF)
    if(PDFDOCUMENTVIEW_POPPLER_REAL)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(PdfDocPopplerQt6 REQUIRED IMPORTED_TARGET poppler-qt6)
        # pkgconf sometimes omits "system" -I flags from INTERFACE_INCLUDE_DIRECTORIES. Locate the
        # public headers explicitly so `#include <poppler/qt6/...>` works on Debian/Ubuntu multiarch.
        find_path(PDFDOCUMENTVIEW_POPPLER_QT6_INCLUDE_DIR
            NAMES poppler/qt6/poppler-document.h
            HINTS ${PdfDocPopplerQt6_INCLUDE_DIRS}
            PATHS
                /usr/include
                /usr/local/include
                /usr/include/x86_64-linux-gnu
                /usr/include/aarch64-linux-gnu)
        if(PDFDOCUMENTVIEW_POPPLER_QT6_INCLUDE_DIR)
            message(STATUS
                "PDFDocumentView: Poppler-Qt6 headers at ${PDFDOCUMENTVIEW_POPPLER_QT6_INCLUDE_DIR}")
        else()
            message(WARNING
                "PDFDocumentView: could not locate poppler/qt6/poppler-document.h; "
                "relying on PkgConfig::PdfDocPopplerQt6 include interface only.")
        endif()
        set(PDFDOCUMENTVIEW_HAVE_POPPLER_QT6 TRUE)
        message(STATUS "PDFDocumentView: Poppler-Qt6 enabled (PDFDOCUMENTVIEW_POPPLER_REAL=ON)")
    else()
        message(STATUS
            "PDFDocumentView: POPPLER backend uses the stub model/render path; "
            "set PDFDOCUMENTVIEW_POPPLER_REAL=ON and install poppler-qt6 dev files for a linked build.")
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)
if(PDFDOCUMENTVIEW_HAVE_POPPLER_QT6)
    find_package(Qt6 REQUIRED COMPONENTS Gui)
endif()

add_subdirectory(src)

add_library(PDFDocumentView::pdf_document_view ALIAS pdf_document_view)

if(PDFDOCUMENTVIEW_BUILD_DOCS)
    find_package(Doxygen)
    if(Doxygen_FOUND)
        set(PDFDOCUMENTVIEW_DOXYGEN_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/include")
        set(PDFDOCUMENTVIEW_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/api-html")
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen/Doxyfile.in"
            "${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/Doxyfile"
            @ONLY
        )
        add_custom_target(docs
            COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen"
            COMMENT "Doxygen HTML -> ${PDFDOCUMENTVIEW_DOXYGEN_OUTPUT_DIR}/html/index.html"
            VERBATIM
        )
    else()
        message(WARNING
            "PDFDOCUMENTVIEW_BUILD_DOCS is ON but Doxygen was not found. "
            "Install the doxygen package, or run `doxygen` from docs/doxygen using the checked-in Doxyfile (see README).")
    endif()
endif()

if(PDFDOCUMENTVIEW_BUILD_EXAMPLES)
    add_subdirectory(examples/basic)
endif()

if(PDFDOCUMENTVIEW_INSTALL)
    include(CMakePackageConfigHelpers)

    set(_pdfdocumentview_cmake_install_dir "${CMAKE_INSTALL_DATAROOTDIR}/cmake/PDFDocumentView")
    set(PDFDOCUMENTVIEW_RENDER_BACKEND_EXPORT "${PDFDOCUMENTVIEW_RENDER_BACKEND}")

    # Installed PDFDocumentViewConfig.cmake: optional PDFDocumentView::PDFium IMPORTED target,
    # PDFDocumentView::Pdfium ALIAS, + INTERFACE_LINK_LIBRARIES propagation (see cmake/PDFDocumentViewConfig.cmake.in).
    # When shipped PDFium is omitted from the install prefix, do not emit an imported PDFium target
    # pointing at a path we never installed (integrator supplies PDFium).
    if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM" AND NOT _pdfdocumentview_omit_shipped_pdfium)
        set(PDFDOCUMENTVIEW_CONFIG_HAVE_PDFIUM_RUNTIME ON)
        get_filename_component(PDFDOCUMENTVIEW_CONFIG_PDFIUM_RUNTIME_FILENAME "${PDFIUM_LIBRARY}" NAME)
        if(WIN32 AND PDFIUM_LIBRARY MATCHES "\\.[Dd][Ll][Ll]$")
            set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPORTED_LOCATION_SUBPATH
                "${CMAKE_INSTALL_BINDIR}/${PDFDOCUMENTVIEW_CONFIG_PDFIUM_RUNTIME_FILENAME}")
            get_filename_component(_pdfdocview_pdfium_dll_dir "${PDFIUM_LIBRARY}" DIRECTORY)
            get_filename_component(_pdfdocview_pdfium_prebuilt_root "${_pdfdocview_pdfium_dll_dir}" DIRECTORY)
            get_filename_component(_pdfdocview_pdfium_dll_we "${PDFIUM_LIBRARY}" NAME_WE)
            set(_pdfdocview_pdfium_import_lib "${_pdfdocview_pdfium_prebuilt_root}/lib/${_pdfdocview_pdfium_dll_we}.lib")
            if(EXISTS "${_pdfdocview_pdfium_import_lib}")
                set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPLIB_SUBPATH "${CMAKE_INSTALL_LIBDIR}/${_pdfdocview_pdfium_dll_we}.lib")
                set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_HAVE_IMPLIB ON)
            else()
                set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_HAVE_IMPLIB OFF)
                set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPLIB_SUBPATH "")
            endif()
            unset(_pdfdocview_pdfium_dll_dir)
            unset(_pdfdocview_pdfium_prebuilt_root)
            unset(_pdfdocview_pdfium_dll_we)
            unset(_pdfdocview_pdfium_import_lib)
        else()
            set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPORTED_LOCATION_SUBPATH
                "${CMAKE_INSTALL_LIBDIR}/${PDFDOCUMENTVIEW_CONFIG_PDFIUM_RUNTIME_FILENAME}")
            set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_HAVE_IMPLIB OFF)
            set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPLIB_SUBPATH "")
        endif()
    elseif(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM" AND _pdfdocumentview_omit_shipped_pdfium)
        set(PDFDOCUMENTVIEW_CONFIG_HAVE_PDFIUM_RUNTIME OFF)
        set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPORTED_LOCATION_SUBPATH "")
        set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPLIB_SUBPATH "")
        set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_HAVE_IMPLIB OFF)
    else()
        set(PDFDOCUMENTVIEW_CONFIG_HAVE_PDFIUM_RUNTIME OFF)
        set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPORTED_LOCATION_SUBPATH "")
        set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_IMPLIB_SUBPATH "")
        set(PDFDOCUMENTVIEW_CONFIG_PDFIUM_HAVE_IMPLIB OFF)
    endif()

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PDFDocumentViewConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/PDFDocumentViewConfig.cmake"
        INSTALL_DESTINATION "${_pdfdocumentview_cmake_install_dir}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/PDFDocumentViewConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/PDFDocumentViewConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/PDFDocumentViewConfigVersion.cmake"
        DESTINATION "${_pdfdocumentview_cmake_install_dir}"
        COMPONENT PDFDocumentView_Development
    )

    install(
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/third-party-notices.template.md"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}/pdfdocumentview"
        COMPONENT PDFDocumentView_Documentation
    )

    # Vendored upstream PDFium legal texts + linked prebuilt shared library (PDFIUM only).
    if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
        # Windows prebuilts ship the DLL under `bin/`; consumers expect runtime DLLs in `bin`
        # and the MSVC import library under `lib/`. Unix keeps the shared object in `lib/`.
        if(NOT _pdfdocumentview_omit_shipped_pdfium)
            if(WIN32 AND PDFIUM_LIBRARY MATCHES "\\.[Dd][Ll][Ll]$")
                install(
                    FILES "${PDFIUM_LIBRARY}"
                    DESTINATION "${CMAKE_INSTALL_BINDIR}"
                    COMPONENT PDFDocumentView_PdfiumRuntime
                )
                get_filename_component(_pdfium_dll_dir "${PDFIUM_LIBRARY}" DIRECTORY)
                get_filename_component(_pdfium_prebuilt_root "${_pdfium_dll_dir}" DIRECTORY)
                get_filename_component(_pdfium_dll_we "${PDFIUM_LIBRARY}" NAME_WE)
                set(_pdfium_import_lib "${_pdfium_prebuilt_root}/lib/${_pdfium_dll_we}.lib")
                if(EXISTS "${_pdfium_import_lib}")
                    install(
                        FILES "${_pdfium_import_lib}"
                        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
                        COMPONENT PDFDocumentView_Development
                    )
                endif()
                unset(_pdfium_dll_dir)
                unset(_pdfium_prebuilt_root)
                unset(_pdfium_dll_we)
                unset(_pdfium_import_lib)
            else()
                install(
                    FILES "${PDFIUM_LIBRARY}"
                    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
                    COMPONENT PDFDocumentView_PdfiumRuntime
                )
            endif()
        endif()
        install(
            FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pdfium/REVISION"
                "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pdfium/README.md"
                "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pdfium/LICENSE"
                "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pdfium/AUTHORS"
            DESTINATION "${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium"
            COMPONENT PDFDocumentView_Documentation
        )
        if(DEFINED PDFIUM_PREBUILT_LICENSE_FILE AND PDFIUM_PREBUILT_LICENSE_FILE)
            install(
                FILES "${PDFIUM_PREBUILT_LICENSE_FILE}"
                DESTINATION "${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium-prebuilt"
                RENAME "LICENSE.pdfium-binary-drop"
                COMPONENT PDFDocumentView_Documentation
            )
        endif()
        install(
            FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/notices/pdfium-prebuilt-NOTICES.md"
            DESTINATION "${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium-prebuilt"
            RENAME "NOTICES.pdfium-prebuilt.md"
            COMPONENT PDFDocumentView_Documentation
        )
        # bblanchon tarballs include a licenses/ tree (ICU, zlib, …). Install it when the
        # resolved prebuilt layout root is known and the directory exists at configure time.
        set(_pdfdocumentview_prebuilt_licenses_dir "")
        if(DEFINED PDFIUM_FETCH_EXTRACT_ROOT AND PDFIUM_FETCH_EXTRACT_ROOT)
            set(_pdfdocumentview_prebuilt_licenses_dir "${PDFIUM_FETCH_EXTRACT_ROOT}/licenses")
        elseif(PDFIUM_ROOT)
            set(_pdfdocumentview_prebuilt_licenses_dir "${PDFIUM_ROOT}/licenses")
        endif()
        if(_pdfdocumentview_prebuilt_licenses_dir AND IS_DIRECTORY "${_pdfdocumentview_prebuilt_licenses_dir}")
            install(
                DIRECTORY "${_pdfdocumentview_prebuilt_licenses_dir}/"
                DESTINATION "${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium-prebuilt/licenses"
                COMPONENT PDFDocumentView_Documentation
            )
        endif()
        unset(_pdfdocumentview_prebuilt_licenses_dir)
    endif()
endif()

option(PDFDOCUMENTVIEW_ENABLE_CPACK
    "Enable CPack variables (e.g. TGZ with install-tree components). Default OFF; see README \"CPack\"."
    OFF)
if(PDFDOCUMENTVIEW_ENABLE_CPACK)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/PDFDocumentViewCPack.cmake")
    # Optional: regenerate build/THIRD_PARTY_NOTICES.pdfium.md from the resolved prebuilt tree.
    if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
        set(_pdfdocumentview_notices_pdfium_root "")
        if(DEFINED PDFIUM_FETCH_EXTRACT_ROOT AND PDFIUM_FETCH_EXTRACT_ROOT)
            set(_pdfdocumentview_notices_pdfium_root "${PDFIUM_FETCH_EXTRACT_ROOT}")
        elseif(PDFIUM_ROOT)
            set(_pdfdocumentview_notices_pdfium_root "${PDFIUM_ROOT}")
        endif()
        if(_pdfdocumentview_notices_pdfium_root
                AND IS_DIRECTORY "${_pdfdocumentview_notices_pdfium_root}/licenses")
            find_package(Python3 COMPONENTS Interpreter QUIET)
            if(Python3_Interpreter_FOUND)
                add_custom_target(aggregate-pdfium-third-party-notices
                    COMMAND "${Python3_EXECUTABLE}"
                        "${CMAKE_CURRENT_SOURCE_DIR}/tools/aggregate_pdfium_third_party_notices.py"
                        --pdfium-root "${_pdfdocumentview_notices_pdfium_root}"
                        --template "${CMAKE_CURRENT_SOURCE_DIR}/docs/about/THIRD_PARTY_NOTICES.template.md"
                        --out "${CMAKE_CURRENT_BINARY_DIR}/THIRD_PARTY_NOTICES.pdfium.md"
                        --include-root-license
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                    COMMENT "Aggregate PDFium licenses/ -> ${CMAKE_CURRENT_BINARY_DIR}/THIRD_PARTY_NOTICES.pdfium.md"
                    VERBATIM
                )
            else()
                message(STATUS
                    "PDFDocumentView: target aggregate-pdfium-third-party-notices skipped "
                    "(Python 3 interpreter not found; install Python 3 or run the script manually).")
            endif()
        else()
            message(STATUS
                "PDFDocumentView: aggregate-pdfium-third-party-notices not defined "
                "(no licenses/ under resolved PDFIUM_ROOT / fetch extract root).")
        endif()
        unset(_pdfdocumentview_notices_pdfium_root)
    endif()
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
