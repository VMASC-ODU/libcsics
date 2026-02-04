#pragma once
#include <filesystem>
#include <vector>

std::vector<char> decompress_cmdline(
    const std::string& cmd, // eg: "zlib -d %s -o %s"
    const std::filesystem::path& input_path
);
