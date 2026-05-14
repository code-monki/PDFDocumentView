# Optional CPack wiring (included when PDFDOCUMENTVIEW_ENABLE_CPACK=ON).
# Does not change default `cmake --install` behavior unless you invoke `cpack`.

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "PDFDocumentView")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
endif()

# Baseline generator; add NSIS/DragNDrop/etc. in your product repo if needed.
set(CPACK_GENERATOR "TGZ")

# Component names must match install(... COMPONENT ...) in root CMakeLists.txt / src/CMakeLists.txt.
# PDFDocumentView_PdfiumRuntime is PDFIUM-only (shipped libpdfium / pdfium.dll).
if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM" AND _pdfdocumentview_omit_shipped_pdfium)
    set(CPACK_COMPONENTS_ALL
        "PDFDocumentView_Runtime;PDFDocumentView_Development;PDFDocumentView_Documentation")
endif()

# Hermetic TGZ: selecting Runtime pulls the shipped PDFium shared library unless the integrator
# opted out with PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=RUNTIME_WITHOUT_SHIPPED_PDFIUM (PdfiumRuntime
# omitted from CPACK_COMPONENTS_ALL and from `cmake --install` rules).
if(PDFDOCUMENTVIEW_RENDER_BACKEND STREQUAL "PDFIUM" AND NOT _pdfdocumentview_omit_shipped_pdfium)
    set(CPACK_COMPONENT_PDFDOCUMENTVIEW_RUNTIME_DEPENDS_ON PDFDocumentView_PdfiumRuntime)
endif()

include(CPack)
