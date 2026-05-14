/**
 * @file pdfium_library.cpp
 * @brief Process-wide PDFium `FPDF_InitLibrary` / `FPDF_DestroyLibrary` reference counting.
 */

#include "pdfium_library.hpp"

#include <atomic>
#include <mutex>

extern "C" {
#include "fpdfview.h"
}

namespace pdf_document_view::pdfium_library {
namespace {

std::mutex g_mutex;
std::atomic<int> g_users{0};

} // namespace

void ensureLibrary() {
    std::lock_guard lock(g_mutex);
    if (g_users.fetch_add(1) == 0) {
        FPDF_InitLibrary();
    }
}

void releaseLibrary() {
    std::lock_guard lock(g_mutex);
    if (g_users.fetch_sub(1) == 1) {
        FPDF_DestroyLibrary();
    }
}

} // namespace pdf_document_view::pdfium_library
