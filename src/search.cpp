#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <filesystem>
#include "channel.h"
#include "producer.h"
#include "search_worker.h"

/**
 * @brief Main function to set up and run the multi-threaded search application
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * 
 * @return int Exit status
 * 
 * @details This function parses command-line arguments to determine the target
 * string and root directory for the search. It sets up channels for file paths
 * and search results, starts the producer thread, a pool of worker threads, and
 * a printer thread. It waits for all threads to complete before cleaning up
 * and exiting.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: bin/search <target> [directory]" << std::endl;
        return 1;
    }

    std::string target = argv[1];
    std::filesystem::path rootDir = (argc >= 3) ? argv[2] : std::filesystem::current_path();

    // Determine number of threads in pool 
    unsigned int hw = std::thread::hardware_concurrency();
    if (hw < 2) hw = 2; // Minimum 2 threads based on assignment

    // Print header
    std::cout << "----- Search Started -----" << std::endl;
    std::cout << "Target Folder: " << rootDir.string() << std::endl;
    std::cout << "Target Text: " << target << std::endl;
    std::cout << "Using a Pool of " << hw << " threads to search." << std::endl;

    // Create channels
    channel<std::filesystem::path>* fileChan = makeChannel<std::filesystem::path>(/*buffer size*/ 64);
    channel<Match>* resultChan = makeChannel<Match>(/*buffer size*/ 64);

    // Start producer thread
    std::thread producerThread(
        producerThreadFunc,
        rootDir,
        fileChan    
    );

    // Start worker thread pool
    std::vector<std::thread> workers;
    workers.reserve(hw);
    for (unsigned int i = 0; i < hw; ++i) {
        workers.emplace_back(
            workerThreadFunc,
            fileChan,
            resultChan,
            std::ref(target)
        
        );
    }

    // Start printer thread
    std::thread printerThread(
        printerThreadFunc,
        resultChan
    );

    // JOin producer thread
    producerThread.join();

    // Join worker threads
    for (auto& worker : workers) {
        worker.join();
    }

    // CLose reuslt channel so printer exits
    resultChan->close();

    // Join printer thread
    printerThread.join();

    // Cleanup channels
    delete fileChan;
    delete resultChan;

    // Print completion footer
    std::cout << "----- Search Complete -----" << std::endl;
    return 0;
}