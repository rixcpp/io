#include <cassert>
#include <iostream>
#include <string>

#include "rix/io/util.hpp"
#include "rix/io/writer.hpp"
#include "rix/io/reader.hpp"

using namespace rix::io;

static void test_write_and_read_text()
{
    auto path = temp_file_path("rix_io_text");

    const std::string original = "Hello from rix-io 🎯";

    write_text(path, original);
    auto loaded = read_text(path);

    assert(loaded == original && "read_text() did not return the same content as written");

    std::error_code ec;
    std::filesystem::remove(path, ec);
}

static void test_try_read_text_nonexistent()
{
    std::filesystem::path path = "/this/path/hopefully/does/not/exist/rix_io_test.txt";
    auto result = try_read_text(path);
    assert(!result.has_value() && "try_read_text() should return nullopt for nonexistent file");
}

static void run_all_tests()
{
    test_write_and_read_text();
    test_try_read_text_nonexistent();
}

int main()
{
    try
    {
        run_all_tests();
        std::cout << "[rix-io] All tests passed.\n";
        return 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[rix-io] Test failed with exception: " << ex.what() << "\n";
        return 1;
    }
}
