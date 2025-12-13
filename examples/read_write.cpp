#include <iostream>
#include <string>

#include "rix/io/util.hpp"
#include "rix/io/writer.hpp"
#include "rix/io/reader.hpp"

using namespace rix::io;

static int run_example()
{
    auto path = temp_file_path("rix_io_example");

    std::cout << "[rix-io] Example file: " << path << "\n";

    std::string content =
        "Rix IO example\n"
        "--------------\n"
        "This file was created by rix::io.\n";

    write_text(path, content);

    auto loaded = read_text(path);

    std::cout << "[rix-io] Loaded content:\n";
    std::cout << "------------------------\n";
    std::cout << loaded << "\n";

    // cleanup best-effort
    std::error_code ec;
    std::filesystem::remove(path, ec);

    return 0;
}

int main()
{
    try
    {
        return run_example();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[rix-io] Example failed: " << ex.what() << "\n";
        return 1;
    }
}
