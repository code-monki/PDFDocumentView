add_executable(pdf_document_view_basic
    main.cpp
)

target_link_libraries(pdf_document_view_basic PRIVATE PDFDocumentView::pdf_document_view Qt6::Widgets)

# Committed synthetic fixture (placeholder text only). Path is relative to this list file so the
# example stays valid when the project is add_subdirectory'd into a larger tree.
set(_pdfdocumentview_basic_demo_pdf "${CMAKE_CURRENT_LIST_DIR}/demo-synthetic-3page.pdf")
if(NOT EXISTS "${_pdfdocumentview_basic_demo_pdf}")
    message(FATAL_ERROR
        "Missing synthetic demo PDF: ${_pdfdocumentview_basic_demo_pdf} "
        "(expected a small committed fixture under examples/basic/)")
endif()

add_custom_command(
    TARGET pdf_document_view_basic
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${_pdfdocumentview_basic_demo_pdf}"
        "$<TARGET_FILE_DIR:pdf_document_view_basic>/demo-synthetic-3page.pdf"
    VERBATIM
    COMMENT "Stage synthetic demo PDF next to pdf_document_view_basic")

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
    if(APPLE)
        install(FILES "${_pdfdocumentview_basic_demo_pdf}"
            DESTINATION "$<TARGET_BUNDLE_CONTENT_DIR:pdf_document_view_basic>/MacOS"
            COMPONENT PDFDocumentView_Development)
    else()
        install(FILES "${_pdfdocumentview_basic_demo_pdf}"
            DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT PDFDocumentView_Development)
    endif()
endif()
