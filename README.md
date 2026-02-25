# io

Minimal modern I/O utilities for C++20.

`io` provides deterministic filesystem helpers and a contiguous owning
byte buffer without hidden behavior.

Header-only by default. Zero external dependencies.

## Download

https://github.com/rixcpp/rix

(Part of the Rix modular utility library)

## Why io?

Basic I/O utilities are required in:

-   File manipulation
-   Temporary file handling
-   Binary serialization
-   Network payload construction
-   Buffer transformations
-   System utilities

This module provides:

-   Path existence checks
-   File size retrieval
-   Temporary path generation
-   Owning contiguous byte buffer
-   Text append helpers
-   Binary POD read/write

No implicit encoding.
No automatic endian conversion.
No hidden state.

Just minimal deterministic I/O tools.

## Installation

### Using Rix (umbrella)

``` cmake
find_package(rix REQUIRED)
```

Link your target:

``` cmake
target_link_libraries(my_app PRIVATE rix::io)
```

### Using rix::all

``` cmake
target_link_libraries(my_app PRIVATE rix::all)
```

### Manual

Clone the repository:

``` bash
git clone https://github.com/rixcpp/rix.git
```

Add the `include/` directory from `modules/io` to your project.

## Quick Examples

### Path Exists

``` cpp
#include <rix/io/util.hpp>
#include <iostream>

int main()
{
  if (rix::io::path_exists("data.txt"))
  {
    std::cout << "File exists\n";
  }
}
```

### File Size

``` cpp
#include <rix/io/util.hpp>
#include <iostream>

int main()
{
  try
  {
    auto size = rix::io::path_size("data.bin");
    std::cout << size << "\n";
  }
  catch (const std::filesystem::filesystem_error& e)
  {
    std::cerr << e.what() << "\n";
  }
}
```

### Temporary Path

``` cpp
#include <rix/io/util.hpp>
#include <iostream>

int main()
{
  auto path = rix::io::temp_path("session");
  std::cout << path << "\n";
}
```

### Basic Buffer Usage

``` cpp
#include <rix/io/buffer.hpp>
#include <iostream>

int main()
{
  rix::io::Buffer buf("hello");

  buf.append(" world");

  std::cout << buf.to_string() << "\n";
}
```

### Binary Read / Write

``` cpp
#include <rix/io/buffer.hpp>
#include <iostream>
#include <cstdint>

int main()
{
  rix::io::Buffer buf;

  std::uint32_t value = 42;
  buf.append_pod(value);

  auto read = buf.read_pod<std::uint32_t>(0);

  std::cout << read << "\n";
}
```

## API Overview

### rix::io::Buffer

-   `Buffer()`
-   `Buffer(size)`
-   `Buffer(string_view)`
-   `Buffer(span<const std::byte>)`
-   `size()`
-   `empty()`
-   `span()`
-   `to_string()`
-   `append(string_view)`
-   `append(span)`
-   `append_pod<T>(value)`
-   `read_pod<T>(offset)`
-   `write_pod<T>(offset, value)`

### rix::io utilities

-   `path_exists(path)`
-   `path_size(path)`
-   `temp_path(prefix = "rix")`

## Design Principles

-   Explicit over implicit
-   No automatic encoding
-   No hidden conversions
-   Deterministic behavior
-   Minimal surface area
-   Modern C++20 only

This module is intentionally small.

If you need:

-   Endian-aware serialization
-   Memory-mapped files
-   Async file I/O
-   Encoding validation
-   Buffered stream abstraction

Build those on top.

## Performance Notes

-   Buffer is backed by `std::vector<std::byte>`
-   POD operations use `std::memcpy`
-   No dynamic polymorphism
-   No virtual dispatch
-   No heap allocations beyond vector growth

Designed for clarity and predictable behavior.

## Tests

Run:

``` bash
cmake --preset dev-ninja
cmake --build --preset dev-ninja
ctest --preset dev-ninja
```

Tests verify:

-   Path helpers
-   Buffer append correctness
-   Binary read/write integrity
-   Boundary checks
-   Exception behavior

## License

MIT License
Copyright (c) Gaspard Kirira

