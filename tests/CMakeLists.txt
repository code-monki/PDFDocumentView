if(NOT BUILD_TESTING)
    return()
endif()

add_executable(pdf_document_model_smoke pdf_document_model_smoke.cpp)
target_link_libraries(pdf_document_model_smoke PRIVATE Qt6::Core pdf_document_view)

add_executable(pdf_widget_async_tile_capability_smoke pdf_widget_async_tile_capability_smoke.cpp)
target_link_libraries(pdf_widget_async_tile_capability_smoke PRIVATE Qt6::Widgets pdf_document_view)

if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM" AND UNIX)
    set_target_properties(pdf_document_model_smoke PROPERTIES
        BUILD_RPATH "$<TARGET_FILE_DIR:pdf_document_view>")
    set_target_properties(pdf_widget_async_tile_capability_smoke PROPERTIES
        BUILD_RPATH "$<TARGET_FILE_DIR:pdf_document_view>")
elseif(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "POPPLER" AND UNIX)
    set_target_properties(pdf_widget_async_tile_capability_smoke PROPERTIES
        BUILD_RPATH "$<TARGET_FILE_DIR:pdf_document_view>")
endif()

add_test(NAME pdf_document_model_smoke COMMAND pdf_document_model_smoke)
add_test(NAME pdf_widget_async_tile_capability_smoke COMMAND pdf_widget_async_tile_capability_smoke)

if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM")
    set(_pdf_fixture_pdf "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/minimal-one-page.pdf")
    add_executable(pdf_document_fixture_smoke pdf_document_fixture_smoke.cpp)
    target_link_libraries(pdf_document_fixture_smoke PRIVATE Qt6::Core pdf_document_view)
    target_compile_definitions(
        pdf_document_fixture_smoke
        PRIVATE "FIXTURE_PDF_PATH=\"${_pdf_fixture_pdf}\""
    )

    if(UNIX)
        set_target_properties(pdf_document_fixture_smoke PROPERTIES
            BUILD_RPATH "$<TARGET_FILE_DIR:pdf_document_view>")
    endif()

    add_test(NAME pdf_document_fixture_smoke COMMAND pdf_document_fixture_smoke)

    add_executable(pdf_document_view_reveal_smoke pdf_document_view_reveal_smoke.cpp)
    target_link_libraries(pdf_document_view_reveal_smoke PRIVATE Qt6::Widgets pdf_document_view)
    target_compile_definitions(
        pdf_document_view_reveal_smoke
        PRIVATE "FIXTURE_PDF_PATH=\"${_pdf_fixture_pdf}\""
    )

    if(UNIX)
        set_target_properties(pdf_document_view_reveal_smoke PROPERTIES
            BUILD_RPATH "$<TARGET_FILE_DIR:pdf_document_view>")
    endif()

    add_test(NAME pdf_document_view_reveal_smoke COMMAND pdf_document_view_reveal_smoke)
endif()
