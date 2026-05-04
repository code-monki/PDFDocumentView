# List the header so AUTOMOC runs for Q_OBJECT in the public header (static library).
add_library(pdf_document_view
    pdf_document_view_widget.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/pdf_document_view/pdf_document_view_widget.hpp"
)

target_include_directories(pdf_document_view
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

target_link_libraries(pdf_document_view PUBLIC Qt6::Widgets)

if(MSVC)
    target_compile_options(pdf_document_view PRIVATE /W4 /permissive-)
else()
    target_compile_options(pdf_document_view PRIVATE -Wall -Wextra -Wpedantic)
endif()
