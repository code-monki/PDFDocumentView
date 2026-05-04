add_executable(pdf_document_view_basic
    main.cpp
)

target_link_libraries(pdf_document_view_basic PRIVATE pdf_document_view Qt6::Widgets)

set_target_properties(pdf_document_view_basic PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
