cmake_minimum_required(VERSION 3.25)

project(PDFDocumentView VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)

option(PDFDOCUMENTVIEW_BUILD_EXAMPLES "Build integration demo (basic)" ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(src)

if(PDFDOCUMENTVIEW_BUILD_EXAMPLES)
    add_subdirectory(examples/basic)
endif()

# Placeholder so `ctest` has at least one test until C++ unit tests are added.
include(CTest)
if(BUILD_TESTING)
    add_test(
        NAME placeholder
        COMMAND "${CMAKE_COMMAND}" -E echo "No C++ unit tests yet; CTest is wired for CI."
    )
endif()
