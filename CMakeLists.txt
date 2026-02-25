# ====================================================================
# Rix - IO Module
# ====================================================================
# Purpose:
#   Build configuration for the io module.
#
#   This module provides basic I/O utilities for Rix:
#     - simple file abstraction
#     - text/binary read & write helpers
#     - stream helpers
#
# Design:
#   - Header-only by default (INTERFACE library)
#   - If any .cpp files exist under src/, we switch to a STATIC lib
#
# Targets:
#   - rix_io      : The actual library target (STATIC or INTERFACE)
#   - rix::io     : Namespaced alias for consumers
#
# Installation:
#   - Installs headers under <prefix>/include/rix/...
#   - Contributes to the umbrella export-set `RixTargets`
# ====================================================================

cmake_minimum_required(VERSION 3.20)
project(rix_io VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

# ------------------------ Global settings ----------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(RIX_IO_GLOBAL_CXX_FLAGS "-Wall -Wextra -Wshadow")
set(CMAKE_CXX_FLAGS_RELEASE "${RIX_IO_GLOBAL_CXX_FLAGS} -O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "${RIX_IO_GLOBAL_CXX_FLAGS} -g")

# ------------------------ Sources discovery --------------------------
file(GLOB_RECURSE RIX_IO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# ------------------------ Helper: Sanitizers (optional) --------------
option(RIX_IO_ENABLE_SANITIZERS "Enable address/UB sanitizers for io" OFF)

function(rix_io_apply_sanitizers tgt scope)
  if (RIX_IO_ENABLE_SANITIZERS AND TARGET ${tgt})
    message(STATUS "[io] enabling sanitizers on ${tgt} (${scope})")

    if (${scope} STREQUAL "PRIVATE")
      target_compile_options(${tgt} PRIVATE
        -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
      )
      target_link_options(${tgt} PRIVATE
        -fsanitize=address,undefined
      )
    else()
      target_compile_options(${tgt} INTERFACE
        -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
      )
      target_link_options(${tgt} INTERFACE
        -fsanitize=address,undefined
      )
    endif()
  endif()
endfunction()

# ============================== STATIC ===============================
if (RIX_IO_SOURCES)
  message(STATUS "[io] Building STATIC library with detected sources.")

  add_library(rix_io STATIC ${RIX_IO_SOURCES})
  add_library(rix::io ALIAS rix_io)
  target_compile_features(rix_io PUBLIC cxx_std_20)

  target_include_directories(rix_io
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  # Sanitizers (opt-in)
  rix_io_apply_sanitizers(rix_io PRIVATE)

  set_target_properties(rix_io PROPERTIES
    OUTPUT_NAME rix_io
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME io
  )

  install(TARGETS rix_io
    EXPORT RixTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

# ============================ HEADER-ONLY ============================
else()
  message(STATUS "[io] Building HEADER-ONLY library (no sources).")

  add_library(rix_io INTERFACE)
  add_library(rix::io ALIAS rix_io)
  target_compile_features(rix_io INTERFACE cxx_std_20)

  target_include_directories(rix_io INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  # Propagate sanitizers to dependents if requested
  rix_io_apply_sanitizers(rix_io INTERFACE)

  install(TARGETS rix_io
    EXPORT RixTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

# ------------------------ Options local build ------------------------
option(RIX_IO_BUILD_TESTS "Build io tests" ON)
option(RIX_IO_BUILD_EXAMPLES "Build io examples" ON)

# ----------------------------- Tests --------------------------------
if (RIX_IO_BUILD_TESTS)
  enable_testing()

  add_executable(rix_io_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/file_test.cpp
  )
  target_link_libraries(rix_io_tests PRIVATE rix_io)
  rix_io_apply_sanitizers(rix_io_tests PRIVATE)

  add_test(NAME rix_io_file_test COMMAND rix_io_tests)
endif()

# ---------------------------- Examples -------------------------------
if (RIX_IO_BUILD_EXAMPLES)
  function(rix_io_add_example exe_name src_file)
    add_executable(${exe_name} ${src_file})
    target_link_libraries(${exe_name} PRIVATE rix_io)
    rix_io_apply_sanitizers(${exe_name} PRIVATE)
  endfunction()

  set(RIX_IO_EXAMPLES
    read_write
    binary_read_write
    buffer_text_roundtrip
    append_text
    try_read_missing
    util_exists_and_size
    file_copy
  )

  foreach(ex ${RIX_IO_EXAMPLES})
    rix_io_add_example(rix_io_${ex}
      ${CMAKE_CURRENT_SOURCE_DIR}/examples/${ex}.cpp
    )
  endforeach()
endif()

# ------------------------ Headers install ----------------------------
install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# ----------------------------- Summary -------------------------------
message(STATUS "------------------------------------------------------")
message(STATUS "rix::io configured (${PROJECT_VERSION})")
if (RIX_IO_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <rix/...>)")
message(STATUS "Sanitizers enabled: ${RIX_IO_ENABLE_SANITIZERS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
