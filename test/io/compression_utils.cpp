#include "compression_utils.hpp"

#include <fstream>
std::vector<char> decompress_cmdline(
    const std::string& cmd,  // eg: "zlib -d %s -o %s"
    const std::filesystem::path& input_path) {
    const std::filesystem::path output_path =
        input_path.string() + ".decompressed";
    char full_cmd[1024];
    std::snprintf(full_cmd, 1024, cmd.c_str(), input_path.c_str(),
                  output_path.c_str());
    int result = std::system(full_cmd);
    if (result != 0) {
        throw std::runtime_error("Decompression command failed");
    }

    std::ifstream file(output_path, std::ios::binary);
    std::vector<char> decompressed_data;

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        decompressed_data.insert(decompressed_data.end(), buffer,
                                 buffer + file.gcount());
    }
    file.close();

    std::filesystem::remove(output_path);

    return decompressed_data;
}
