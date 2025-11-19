#ifndef SEARCH_WORKER_H
#define SEARCH_WORKER_H

#include <string>
#include <filesystem>
#include <thread>
#include "channel.h"

/**
 * @struct Match
 * 
 * @brief Structure to hold information about a search match
 * 
 * @var thread_id The ID of the thread that found the match
 * @var file_path The path of the file where the match was found
 * @var line_number The line number of the match in the file
 * @var line_content The content of the line containing the match
 * 
 * @details This structure contains the thread ID that found the match,
 * the file path where the match was found, the line number of the match,
 * and the content of the line containing the match.
 */
struct Match {
    std::thread::id thread_id;
    std::filesystem::path file_path;
    int line_number;
    std::string line_content;
};

/**
 * @brief Search a file for the target string and send matches to the result channel
 * 
 * @param filePath The path of the file to search
 * @param target The target string to search for
 * @param resultChan The channel to send Match objects through
 * 
 * @return void
 * 
 * @details This function opens the specified file and reads it line by line.
 * For each line that contains the target string, it creates a Match object
 * and sends it through the provided result channel.
 */
static void searchFileForTarget(
    const std::filesystem::path& filePath,
    const std::string& target,
    channel<Match>* resultChan
);

/**
 * @brief Worker thread to process files from the file channel and search for the target string
 * 
 * @param fileChan The channel to receive file paths from
 * @param resultChan The channel to send Match objects through
 * @param target The target string to search for
 * 
 * @return void
 * 
 * @details This function continuously receives file paths from the file channel,
 * searches each file for the target string using the searchFileForTarget helper,
 * and sends any matches to the result channel. It stops when the file channel is closed.
 */
void workerThreadFunc(
    channel<std::filesystem::path>* fileChan,
    channel<Match>* resultChan,
    const std::string& target
);

/**
 * @brief Printer thread to receive Match objects from the result channel and print them
 * 
 * @param resultChan The channel to receive Match objects from
 * 
 * @return void
 * 
 * @details This function continuously receives Match objects from the result channel
 * and prints their details to the console. It stops when the result channel is closed.
 */
void printerThreadFunc(
    channel<Match>* resultChan
);

#endif // SEARCH_WORKER_H