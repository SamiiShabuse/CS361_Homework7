#include "search_worker.h"
#include <fstream>
#include <iostream>

// Helper: search a single file for the target
static void searchFileForTarget(
    const std::filesystem::path& filePath,
    const std::string& target,
    channel<Match>* resultChan
) {
    // Implement file searching logic line by line
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

// Worker Thread
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

// Printer thread to display results
void printerThreadFunc(channel<Match>* resultChan) {
    try {
        while (true) {
            Match m = resultChan->receive();
            
            std::cout << "----------" << std::endl;
            std::cout << "Thread " << m.thread_id << " found a match.\n";
            std::cout << "File: \"" << m.file_path.string() << "\"\n";
            std::cout << "Line " << m.line_number << ": " << m.line_content << "\n";
            std::cout << "----------\n";
        }
    } catch (...) {
        // Assume exception means channel is closed
    }
}