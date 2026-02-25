#include <cassert>
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <rix/io/buffer.hpp>
#include <rix/io/file.hpp>
#include <rix/io/reader.hpp>
#include <rix/io/util.hpp>
#include <rix/io/writer.hpp>

namespace fs = std::filesystem;

static void test_buffer_text_roundtrip()
{
  rix::io::Buffer b;
  b.assign(std::string_view("hello"));
  assert(b.size() == 5);
  assert(!b.empty());
  assert(b.to_string() == "hello");

  b.append(std::string_view(" world"));
  assert(b.to_string() == "hello world");

  b.clear();
  assert(b.empty());
  assert(b.size() == 0);
}

static void test_buffer_bytes_and_pod()
{
  rix::io::Buffer b;
  std::vector<std::byte> bytes;
  bytes.push_back(static_cast<std::byte>(0x01));
  bytes.push_back(static_cast<std::byte>(0x02));
  b.assign(std::span<const std::byte>(bytes.data(), bytes.size()));
  assert(b.size() == 2);
  assert(b[0] == static_cast<std::byte>(0x01));
  assert(b[1] == static_cast<std::byte>(0x02));

  const int v = 1234567;
  b.clear();
  b.append_pod(v);
  assert(b.size() == sizeof(int));

  const int out = b.read_pod<int>(0);
  assert(out == v);

  const int v2 = 42;
  b.write_pod<int>(0, v2);
  const int out2 = b.read_pod<int>(0);
  assert(out2 == v2);

  bool threw = false;
  try
  {
    (void)b.read_pod<int>(1);
  }
  catch (...)
  {
    threw = true;
  }
  assert(threw);
}

static void test_util_temp_path()
{
  const fs::path p1 = rix::io::temp_path("rix_io");
  const fs::path p2 = rix::io::temp_path("rix_io");
  assert(p1 != p2);
  assert(p1.filename().string().find("rix_io") != std::string::npos);
  assert(p2.filename().string().find("rix_io") != std::string::npos);
}

static void test_write_read_text_helpers()
{
  const fs::path p = rix::io::temp_path("rix_io_text");

  rix::io::write_file_text(p, "abc");
  assert(rix::io::path_exists(p));
  assert(rix::io::read_file_text(p) == "abc");

  rix::io::write_file_text(p, "def", rix::io::WriteMode::append);
  assert(rix::io::read_file_text(p) == "abcdef");

  fs::remove(p);
  assert(!rix::io::path_exists(p));
}

static void test_write_read_binary_helpers()
{
  const fs::path p = rix::io::temp_path("rix_io_bin");

  std::vector<std::byte> data;
  data.push_back(static_cast<std::byte>(0x10));
  data.push_back(static_cast<std::byte>(0x20));
  data.push_back(static_cast<std::byte>(0x30));

  rix::io::write_file_binary(p, std::span<const std::byte>(data.data(), data.size()));
  assert(rix::io::path_exists(p));

  const auto out = rix::io::read_file_binary(p);
  assert(out.size() == data.size());
  assert(out[0] == data[0]);
  assert(out[1] == data[1]);
  assert(out[2] == data[2]);

  fs::remove(p);
  assert(!rix::io::path_exists(p));
}

static void test_file_mode_checks()
{
  const fs::path p = rix::io::temp_path("rix_io_mode");

  rix::io::write_file_text(p, "x");

  bool threw = false;
  try
  {
    rix::io::File f{p, rix::io::FileMode::write, rix::io::FileType::text};
    (void)f.read_all_text();
  }
  catch (...)
  {
    threw = true;
  }
  assert(threw);

  threw = false;
  try
  {
    rix::io::File f{p, rix::io::FileMode::read, rix::io::FileType::text};
    f.write(std::string_view("y"));
  }
  catch (...)
  {
    threw = true;
  }
  assert(threw);

  fs::remove(p);
}

static void test_try_read_helpers()
{
  const fs::path missing = rix::io::temp_path("rix_io_missing");

  const auto t = rix::io::try_read_file_text(missing);
  const auto b = rix::io::try_read_file_binary(missing);

  assert(!t.has_value());
  assert(!b.has_value());
}

int main()
{
  test_buffer_text_roundtrip();
  test_buffer_bytes_and_pod();
  test_util_temp_path();
  test_write_read_text_helpers();
  test_write_read_binary_helpers();
  test_file_mode_checks();
  test_try_read_helpers();
  return 0;
}
