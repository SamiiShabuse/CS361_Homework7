#include "producer.h"
#include <iostream>
#include <filesystem>
#include <set> 

namespace fs = std::filesystem;

static bool hasValidExtension(const fs::path& p) {
    static const std::set<std::string> validExtensions = {
        ".cc", ".c", ".cpp", ".h", ".hpp", ".pl", ".sh", ".py", ".txt"
    };
    auto ext = p.extension().string();
    return validExtensions.find(ext) != validExtensions.end();
}

void producerThreadFunc(
    const std::filesystem::path& rootDir,
    channel<std::filesystem::path>* fileChan
) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(rootDir)) {
            if (entry.is_regular_file() && hasValidExtension(entry.path())) {
                fileChan->send(entry.path());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during directory traversal: " << e.what() << std::endl;
    }
    fileChan->close();
}