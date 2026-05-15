# List the header so AUTOMOC runs for Q_OBJECT in the public header (static library).
set(_pdfdocumentview_sources
    create_default_pdf_document_model.cpp
    create_default_pdf_render_backend.cpp
    pdf_document_view_widget.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/pdf_document_view/pdf_document_view_widget.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/pdf_document_view/page_highlight.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/pdf_document_view/pdf_document_model.hpp"
)

if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
    list(APPEND _pdfdocumentview_sources
        pdfium_library.cpp
        pdfium_document_model.cpp
        pdfium_backend.cpp
        pdfium_tile_raster.cpp
        pdf_tile_render_worker.hpp
        pdf_tile_render_worker.cpp
    )
elseif(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "POPPLER")
    if(PDFDOCUMENTVIEW_HAVE_POPPLER_QT6)
        list(APPEND _pdfdocumentview_sources
            poppler_qt_document_model.cpp
            poppler_qt_backend.cpp
            pdf_tile_render_worker.hpp
            pdf_tile_render_worker.cpp
        )
    else()
        list(APPEND _pdfdocumentview_sources
            stub_pdf_document_model.cpp
            poppler_stub_backend.cpp
        )
    endif()
endif()

add_library(pdf_document_view SHARED ${_pdfdocumentview_sources})

target_include_directories(pdf_document_view
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if(WIN32)
    target_compile_definitions(pdf_document_view PRIVATE PDFDOCUMENTVIEW_LIBRARY)
endif()

target_link_libraries(pdf_document_view PUBLIC Qt6::Widgets)

if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
    target_compile_definitions(pdf_document_view PUBLIC
        PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM=1
        PDFDOCUMENTVIEW_ASYNC_TILE_WORKER=1)
    target_include_directories(pdf_document_view PRIVATE "${PDFIUM_INCLUDE_DIR}")
    target_link_libraries(pdf_document_view PRIVATE "${PDFIUM_LIBRARY}")
    if(NOT "${PDFIUM_RUNTIME_LIBRARY}" STREQUAL "")
        set(_pdfdocumentview_pdfium_runtime_file "${PDFIUM_RUNTIME_LIBRARY}")
    else()
        set(_pdfdocumentview_pdfium_runtime_file "${PDFIUM_LIBRARY}")
    endif()
    if(UNIX)
        set_target_properties(pdf_document_view PROPERTIES
            BUILD_RPATH "$ORIGIN"
            INSTALL_RPATH "$ORIGIN")
    endif()
    get_filename_component(_pdfdocumentview_pdfium_copy_name "${_pdfdocumentview_pdfium_runtime_file}" NAME)
    add_custom_command(
        TARGET pdf_document_view
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_pdfdocumentview_pdfium_runtime_file}"
            "$<TARGET_FILE_DIR:pdf_document_view>/${_pdfdocumentview_pdfium_copy_name}"
        VERBATIM
        COMMENT "Stage libpdfium next to pdf_document_view for runtime resolution")
    unset(_pdfdocumentview_pdfium_runtime_file)
    if(APPLE)
        add_custom_command(
            TARGET pdf_document_view
            POST_BUILD
            COMMAND install_name_tool -change "./libpdfium.dylib" "@loader_path/libpdfium.dylib"
                "$<TARGET_FILE:pdf_document_view>"
            VERBATIM
            COMMENT "Point pdf_document_view load of libpdfium to @loader_path")
    endif()
elseif(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "POPPLER")
    target_compile_definitions(pdf_document_view PUBLIC PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER=1)
    if(PDFDOCUMENTVIEW_HAVE_POPPLER_QT6)
        target_compile_definitions(pdf_document_view PUBLIC PDFDOCUMENTVIEW_ASYNC_TILE_WORKER=1)
        target_compile_definitions(pdf_document_view PRIVATE PDFDOCUMENTVIEW_POPPLER_REAL=1)
        target_link_libraries(pdf_document_view PRIVATE PkgConfig::PdfDocPopplerQt6 Qt6::Gui)
        if(PdfDocPopplerQt6_INCLUDE_DIRS)
            target_include_directories(pdf_document_view PRIVATE ${PdfDocPopplerQt6_INCLUDE_DIRS})
        endif()
        if(PDFDOCUMENTVIEW_POPPLER_QT6_INCLUDE_DIR)
            target_include_directories(pdf_document_view PRIVATE "${PDFDOCUMENTVIEW_POPPLER_QT6_INCLUDE_DIR}")
        endif()
    endif()
endif()

set_target_properties(pdf_document_view PROPERTIES EXPORT_NAME pdf_document_view)

if(WIN32)
    set_target_properties(pdf_document_view PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(MSVC)
    target_compile_options(pdf_document_view PRIVATE /W4 /permissive-)
else()
    target_compile_options(pdf_document_view PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(PDFDOCUMENTVIEW_INSTALL)
    install(
        TARGETS pdf_document_view
        EXPORT PDFDocumentViewTargets
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT PDFDocumentView_Runtime
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT PDFDocumentView_Runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT PDFDocumentView_Runtime
    )

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../include/pdf_document_view/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/pdf_document_view"
        COMPONENT PDFDocumentView_Development
        FILES_MATCHING PATTERN "*.hpp"
    )

    install(
        EXPORT PDFDocumentViewTargets
        NAMESPACE PDFDocumentView::
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/PDFDocumentView"
        FILE PDFDocumentViewTargets.cmake
        COMPONENT PDFDocumentView_Development
    )
endif()
