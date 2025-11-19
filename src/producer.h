#ifndef PRODUCER_H
#define PRODUCER_H

#include <string>
#include <filesystem>
#include "channel.h"

/**
 * @brief Check if the file has a valid extension
 * 
 * @param p The file path
 * 
 * @return true if valid, false otherwise
 * 
 * @details Valid extensions are: .cc, .c, .cpp, .h, .hpp, .pl, .sh, .py, .txt and
 * it checks to see if the file's extension is in the predefined set. If it is not,
 * the file is considered invalid.
 */
static bool hasValidExtension(const std::filesystem::path& p);

/**
 * @brief Producer thread function to traverse directories and send file paths
 * 
 * @param rootDir The root directory to start traversal
 * @param fileChan The channel to send file paths through
 * 
 * @return void
 * 
 * @details This function recursively traverses the directory tree starting from
 * the specified root directory. It checks each file's extension against a set of
 * valid extensions. If a file has a valid extension, its path is sent through the
 * provided channel. Once the traversal is complete, the channel is closed to signal
 * that no more files will be sent.
 */
void producerThreadFunc(
    const std::filesystem::path& rootDir,
    channel<std::filesystem::path>* fileChan
);

#endif // PRODUCER_H