cmake_minimum_required(VERSION 3.25)
project(PDFDocumentViewConsumerSmoke LANGUAGES CXX)

# Qt + PDFDocumentView: set CMAKE_PREFIX_PATH (and/or Qt6_DIR) to your Qt kit and
# to the prefix where PDFDocumentView was installed. ENV{QT_PREFIX} is not read
# here—only PDFDocumentView's top-level CMake appends it when building that tree.

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PDFDocumentView CONFIG REQUIRED)

add_executable(consumer_smoke main.cpp)
# Exported name is PDFDocumentView::pdf_document_view (see src/CMakeLists.txt EXPORT_NAME).
# PDFIUM installs: PDFDocumentViewConfig.cmake defines PDFDocumentView::PDFium (ALIAS Pdfium)
# and propagates $<LINK_ONLY:PDFDocumentView::PDFium> on the main target — linking only the
# library suffices here.
target_link_libraries(consumer_smoke PRIVATE PDFDocumentView::pdf_document_view Qt6::Widgets)
