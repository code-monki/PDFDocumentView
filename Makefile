# Top-level orchestration for PDFDocumentView (library + optional demo).
# Requires Qt 6 for library and examples — pass QT_PREFIX on configure.

SHELL := /bin/sh

CMAKE ?= cmake
CTEST ?= ctest
CXX ?= c++

BUILD_DIR ?= build
BUILD_TYPE ?= Debug
CTEST_OUTPUT_ON_FAILURE ?= 1

# Qt 6 installation prefix (CMAKE_PREFIX_PATH). Required for find_package(Qt6 …).
QT_PREFIX ?=

.DEFAULT_GOAL := help

.PHONY: help
help:
	@printf '%s\n' 'PDFDocumentView orchestration targets:'
	@printf '%s\n' ''
	@printf '  %-16s %s\n' 'help' 'Show this target list.'
	@printf '  %-16s %s\n' 'tool-check' 'Verify cmake, ctest, and C++ compiler.'
	@printf '  %-16s %s\n' 'all' 'Configure and build library + demo (needs QT_PREFIX).'
	@printf '  %-16s %s\n' 'configure' 'Run CMake into $(BUILD_DIR).'
	@printf '  %-16s %s\n' 'build' 'Build default targets.'
	@printf '  %-16s %s\n' 'demo-run' 'Launch pdf_document_view_basic example (after build).'
	@printf '  %-16s %s\n' 'test' 'Run CTest (when tests exist).'
	@printf '  %-16s %s\n' 'check' 'tool-check, configure, build, test.'
	@printf '  %-16s %s\n' 'clean' 'Remove $(BUILD_DIR).'
	@printf '%s\n' ''
	@printf '%s\n' 'Variables:'
	@printf '  %-16s %s\n' 'BUILD_DIR' 'CMake build directory (default: build)'
	@printf '  %-16s %s\n' 'BUILD_TYPE' 'CMake build type (default: Debug)'
	@printf '  %-16s %s\n' 'QT_PREFIX' 'Qt 6 prefix for CMAKE_PREFIX_PATH (required)'

.PHONY: tool-check
tool-check:
	@command -v "$(CMAKE)" >/dev/null 2>&1 || { printf '%s\n' 'Missing: cmake'; exit 1; }
	@command -v "$(CTEST)" >/dev/null 2>&1 || { printf '%s\n' 'Missing: ctest'; exit 1; }
	@command -v "$(CXX)" >/dev/null 2>&1 || { printf '%s\n' "Missing: C++ compiler ($$CXX)"; exit 1; }
	@"$(CMAKE)" --version | sed -n '1p'
	@"$(CTEST)" --version | sed -n '1p'
	@"$(CXX)" --version | sed -n '1p'

.PHONY: all
all: build

# Require QT_PREFIX for Qt-based configure
.PHONY: _require_qt
_require_qt:
	@test -n "$(strip $(QT_PREFIX))" || { \
		printf '%s\n' 'QT_PREFIX is required (e.g. export QT_PREFIX=$$HOME/Qt/6.9.3/macos).'; \
		exit 1; \
	}

.PHONY: configure
configure: tool-check _require_qt
	@"$(CMAKE)" -S . -B "$(BUILD_DIR)" \
		-DCMAKE_BUILD_TYPE="$(BUILD_TYPE)" \
		-DCMAKE_PREFIX_PATH="$(QT_PREFIX)"

.PHONY: build
build: configure
	@"$(CMAKE)" --build "$(BUILD_DIR)"

.PHONY: test
test: build
	@cd "$(BUILD_DIR)" && "$(CTEST)" --output-on-failure -C "$(BUILD_TYPE)"

.PHONY: check
check: build test

.PHONY: demo-run
demo-run: build
	@case "$$(uname -s)" in \
	Darwin) \
	  _app="$(BUILD_DIR)/examples/basic/pdf_document_view_basic.app"; \
	  if [ ! -d "$$_app" ]; then _app="$(BUILD_DIR)/examples/basic/$(BUILD_TYPE)/pdf_document_view_basic.app"; fi; \
	  if [ ! -d "$$_app" ]; then \
	    printf '%s\n' 'Demo app not found. Build with: make all QT_PREFIX=…'; exit 1; \
	  fi; \
	  printf '%s\n' "Launching $$(pwd)/$$_app"; \
	  open "$$_app" ;; \
	Linux|*) \
	  _bin="$(BUILD_DIR)/examples/basic/pdf_document_view_basic"; \
	  if [ ! -x "$$_bin" ]; then _bin="$(BUILD_DIR)/examples/basic/$(BUILD_TYPE)/pdf_document_view_basic"; fi; \
	  if [ ! -x "$$_bin" ]; then \
	    printf '%s\n' 'Demo binary not found. Build with: make all QT_PREFIX=…'; exit 1; \
	  fi; \
	  "$$_bin" ;; \
	esac

.PHONY: clean
clean:
	rm -rf "$(BUILD_DIR)"
