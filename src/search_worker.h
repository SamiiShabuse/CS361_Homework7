#ifndef SEARCH_WORKER_H
#define SEARCH_WORKER_H

#include <string>
#include <filesystem>
#include <thread>
#include "channel.h"

struct Match {
    std::thread::id thread_id;
    std::filesystem::path file_path;
    int line_number;
    std::string line_content;
};

// Worker thread entry point
void workerThreadFunc(
    channel<std::filesystem::path>* fileChan,
    channel<Match>* resultChan,
    const std::string& target
)

// Printer thread entry point
void printerThreadFunc(
    channel<Match>* resultChan
);

#endif // SEARCH_WORKER_H