# ====================================================================
# Rix — IO Module
# ====================================================================
# Purpose:
#   Build configuration for the rix-io module.
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

# Friendly warnings (can be tuned later if needed)
set(RIX_IO_GLOBAL_CXX_FLAGS "-Wall -Wextra -Wshadow")
set(CMAKE_CXX_FLAGS_RELEASE "${RIX_IO_GLOBAL_CXX_FLAGS} -O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "${RIX_IO_GLOBAL_CXX_FLAGS} -g")

# ------------------------ Sources discovery --------------------------
# If any .cpp exists under src/, we switch to STATIC build mode.
file(GLOB_RECURSE RIX_IO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# ------------------------ Helper: Sanitizers (optional) --------------
# This is opt-in and controlled by a top-level option if needed.
option(RIX_IO_ENABLE_SANITIZERS "Enable address/UB sanitizers for rix-io" OFF)

function(rix_io_apply_sanitizers tgt scope)
  if (RIX_IO_ENABLE_SANITIZERS AND TARGET ${tgt})
    message(STATUS "[rix-io] enabling sanitizers on ${tgt} (${scope})")
    if (${scope} STREQUAL "PRIVATE")
      target_compile_options(${tgt} PRIVATE
        -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined)
      target_link_options(${tgt} PRIVATE
        -fsanitize=address,undefined)
    else()
      target_compile_options(${tgt} INTERFACE
        -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined)
      target_link_options(${tgt} INTERFACE
        -fsanitize=address,undefined)
    endif()
  endif()
endfunction()

# ============================== STATIC ===============================
if (RIX_IO_SOURCES)
  message(STATUS "[rix-io] Building STATIC library with detected sources.")

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
  message(STATUS "[rix-io] Building HEADER-ONLY library (no sources).")

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
option(RIX_IO_BUILD_TESTS "Build rix-io tests" ON)
option(RIX_IO_BUILD_EXAMPLES "Build rix-io examples" ON)

# ----------------------------- Tests --------------------------------
if (RIX_IO_BUILD_TESTS)
  enable_testing()

  add_executable(rix_io_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/file_test.cpp
  )

  target_link_libraries(rix_io_tests PRIVATE rix_io)

  # Include dirs arrivent via rix_io (PUBLIC / INTERFACE), donc pas besoin
  # d'en rajouter ici.

  # Optionnel : activer les sanitizers aussi pour les tests
  rix_io_apply_sanitizers(rix_io_tests PRIVATE)

  add_test(NAME rix_io_file_test COMMAND rix_io_tests)
endif()

# ---------------------------- Examples -------------------------------
if (RIX_IO_BUILD_EXAMPLES)
  add_executable(rix_io_read_write
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/read_write.cpp
  )

  target_link_libraries(rix_io_read_write PRIVATE rix_io)

  # Optionnel : sanitizers sur l'exemple aussi
  rix_io_apply_sanitizers(rix_io_read_write PRIVATE)
endif()


# ------------------------ Headers install ----------------------------
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

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
