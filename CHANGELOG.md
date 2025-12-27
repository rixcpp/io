# Changelog

All notable changes to **rix-io** will be documented in this file.
This project follows **Semantic Versioning** (SemVer).
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

---

## [v1.0.0] — 2025-01-XX

🎉 **First stable release of rix-io**

This release marks the **stabilization of the public IO API** for the Rix ecosystem.
All core abstractions, naming conventions, and behaviors are now considered stable.

---

### ✨ Added

- `rix::io::File`
  - Unified file abstraction for text and binary I/O
  - RAII-based resource management
  - Explicit `FileMode` and `FileType`
- `rix::io::Buffer`
  - Byte-oriented buffer abstraction
  - Construction from text or raw bytes
  - Safe conversion back to `std::string`
- Reader utilities
  - `read_text`
  - `read_binary`
  - `try_read_text`
  - `try_read_binary`
- Writer utilities
  - `write_file_text`
  - `write_file_binary`
  - Explicit `WriteMode` (`truncate`, `append`)
- Filesystem utilities
  - `path_exists`
  - `file_size`
  - `temp_file_path`
- Comprehensive examples:
  - Text read/write
  - Binary read/write
  - Append mode
  - File copy
  - Buffer roundtrip
  - Missing-file handling
  - Existence & size checks
- Initial test suite covering:
  - Text write/read
  - Safe read on missing files

---

### 🔁 Changed

- Introduced **strict naming conventions** to avoid collisions with the C++ standard library
  - e.g. `exists` → `path_exists`
- Standardized API structure across headers:
  - `file.hpp`
  - `reader.hpp`
  - `writer.hpp`
  - `buffer.hpp`
  - `util.hpp`
- Improved CMake configuration for:
  - Header-only mode
  - Optional static build
  - Examples and tests toggles

---

### ⚠️ Deprecated

- Deprecated legacy helper names (still available for transition):
  - `exists`
  - `write_text`
  - `write_binary`

> Deprecated APIs will remain available during the **1.x** series and may be removed in **v2.0.0**.

---

### 🧱 Stability Guarantee

- All public APIs introduced in **v1.0.0** are considered **stable**
- No breaking changes will be introduced without a **major version bump**

---

### 🚀 Notes

- This release establishes the **quality and design baseline** for all future Rix modules
- `rix-io` serves as the reference implementation for:
  - API clarity
  - Naming discipline
  - Example-driven documentation

---

## [0.0.1] - 2025-12-13

### Added

-

### Changed

-

### Removed

-

## [0.1.0] - 2025-12-05

### Added

-

### Changed

-

### Removed

-

## [v0.1.0] - 2025-12-05

### Added

- Initial project scaffolding for the `vixcpp/websocket` module.
- CMake build system:
  - STATIC vs header-only build depending on `src/` contents.
  - Integration with `vix::core` and optional JSON backend.
  - Support for sanitizers via `VIX_ENABLE_SANITIZERS`.
- Basic repository structure:
  - `include/vix/websocket/` for public headers.
  - `src/` for implementation files.
- Release workflow:
  - `Makefile` with `release`, `commit`, `push`, `merge`, and `tag` targets.
  - `changelog` target wired to `scripts/update_changelog.sh`.
