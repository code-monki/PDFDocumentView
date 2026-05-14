add_executable(pdf_document_view_basic
    main.cpp
)

target_link_libraries(pdf_document_view_basic PRIVATE PDFDocumentView::pdf_document_view Qt6::Widgets)

# Absolute default PDF at repo root when present at configure time (file is typically .gitignored).
set(_pdfdocumentview_example_default_pdf "${CMAKE_SOURCE_DIR}/gnu-c-language-manual.pdf")
if(EXISTS "${_pdfdocumentview_example_default_pdf}")
    file(REAL_PATH "${_pdfdocumentview_example_default_pdf}" _pdfdocumentview_example_default_pdf_abs)
    file(TO_CMAKE_PATH "${_pdfdocumentview_example_default_pdf_abs}" _pdfdocumentview_example_default_pdf_cmake)
    # C++11 raw string avoids fragile -D escaping for paths with spaces or backslashes (delimiter must not appear in path).
    target_compile_definitions(pdf_document_view_basic PRIVATE
        "PDFDOCUMENTVIEW_EXAMPLE_DEFAULT_PDF=R\"(${_pdfdocumentview_example_default_pdf_cmake})\"")
endif()

# Historical: this demo intentionally does not use Qt Pdf. PdfDocumentViewWidget is the only PDF
# surface so the demo exercises the library's public API, not a parallel raster path.

set_target_properties(pdf_document_view_basic PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# Installed macOS .app: RPATH must include both (1) our GNUInstallDirs lib/ (libpdf_document_view,
# libpdfium) and (2) Qt's lib/ (QtWidgets.framework, etc.). Qt is not copied into the prefix by
# default — resolve from Qt6_DIR (…/lib/cmake/Qt6 → …/lib).
if(APPLE AND PDFDOCUMENTVIEW_INSTALL)
    file(REAL_PATH "${Qt6_DIR}/../.." _pdfdocumentview_qt6_libdir)
endif()

# PDFIUM: find libpdf_document_view (and libpdfium staged beside it) via RPATH — no
# install_name_tool or copying libpdfium into the .app for the default GNUInstallDirs
# install layout (bundle under CMAKE_INSTALL_BINDIR, shared libs under CMAKE_INSTALL_LIBDIR).
if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
    set(_pdfdocumentview_basic_pdfium_build_rpath "$<TARGET_FILE_DIR:pdf_document_view>")
    if(APPLE)
        if(PDFDOCUMENTVIEW_INSTALL)
            file(RELATIVE_PATH _pdfdocumentview_basic_install_lib_relpath
                "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/pdf_document_view_basic.app/Contents/MacOS"
                "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
            set_target_properties(pdf_document_view_basic PROPERTIES
                BUILD_RPATH "${_pdfdocumentview_basic_pdfium_build_rpath}"
                # Second entry: Qt frameworks live under the Qt prefix lib/, not CMAKE_INSTALL_LIBDIR.
                INSTALL_RPATH
                    "@loader_path/${_pdfdocumentview_basic_install_lib_relpath};${_pdfdocumentview_qt6_libdir}")
        else()
            set_target_properties(pdf_document_view_basic PROPERTIES
                BUILD_RPATH "${_pdfdocumentview_basic_pdfium_build_rpath}")
        endif()
    elseif(UNIX)
        if(PDFDOCUMENTVIEW_INSTALL)
            file(REAL_PATH "${Qt6_DIR}/../.." _pdfdocumentview_qt6_libdir_unix)
            file(RELATIVE_PATH _pdfdocumentview_basic_install_lib_relpath
                "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
                "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
            set_target_properties(pdf_document_view_basic PROPERTIES
                BUILD_RPATH "${_pdfdocumentview_basic_pdfium_build_rpath}"
                INSTALL_RPATH
                    "$ORIGIN/${_pdfdocumentview_basic_install_lib_relpath};${_pdfdocumentview_qt6_libdir_unix}")
        else()
            set_target_properties(pdf_document_view_basic PROPERTIES
                BUILD_RPATH "${_pdfdocumentview_basic_pdfium_build_rpath}")
        endif()
    endif()
elseif(APPLE AND PDFDOCUMENTVIEW_INSTALL)
    # Poppler (or other) backend: still link Qt Widgets; same Qt RPATH gap without PDFIUM block.
    file(RELATIVE_PATH _pdfdocumentview_basic_install_lib_relpath
        "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/pdf_document_view_basic.app/Contents/MacOS"
        "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    set_target_properties(pdf_document_view_basic PROPERTIES
        INSTALL_RPATH
            "@loader_path/${_pdfdocumentview_basic_install_lib_relpath};${_pdfdocumentview_qt6_libdir}")
endif()

if(PDFDOCUMENTVIEW_INSTALL)
    install(TARGETS pdf_document_view_basic
        BUNDLE DESTINATION "${CMAKE_INSTALL_BINDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT PDFDocumentView_Development)
endif()
