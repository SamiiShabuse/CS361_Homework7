/**
 * @file src/search_worker.cpp
 * 
 * @brief Implementation of worker and printer thread functions for searching files
 *        for a target string and printing the results.
 * 
 * @author Samii Shabuse <sus24@drexel.edu>
 * @date November 20, 2025
 * 
 * @section Overview
 * 
 * This file contains the implementation of the worker thread function that
 * processes file paths received from a channel, searches each file for a
 * specified target string, and sends any matches to a result channel. It also
 * includes the implementation of the printer thread function that receives
 * Match objects from the result channel and prints their details to the console.
 */


#include "search_worker.h"
#include <fstream>
#include <iostream>

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
) {
    // file searching logic line by line
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return; // Could not open file due to permissions or other issues
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (line.find(target) != std::string::npos) {
            Match m;
            m.thread_id = std::this_thread::get_id();
            m.file_path = filePath;
            m.line_number = lineNumber;
            m.line_content = line;
            resultChan->send(m);
        }
    }
}

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
) {
    while (true) {
        try {
            auto filePath = fileChan->receive();
            searchFileForTarget(filePath, target, resultChan);
        } catch (...) {
            // Assume exception means channel is closed
            break;
        }
    }
}

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
void printerThreadFunc(channel<Match>* resultChan) {
    try {
        while (true) {
            Match m = resultChan->receive();
            
            std::cout << "----------" << std::endl;
            std::cout << "Thread " << m.thread_id << " found a match." << std::endl;
            std::cout << "File: \"" << m.file_path.string() << "\"" << std::endl;
            std::cout << "Line " << m.line_number << ": " << m.line_content << std::endl;
            std::cout << "----------" << std::endl;
        }
    } catch (...) {
        // Assume exception means channel is closed
    }
}