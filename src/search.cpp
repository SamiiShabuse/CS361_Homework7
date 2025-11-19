#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <filesystem>
#include "channel.h"
#include "producer.h"
#include "search_worker.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: bin/search <target> [directory]" << std::endl;
        return 1;
    }

    std::string target = argv[1];
    std::filesystem::path rootDir = (argc >= 3) ? argv[2] : std::filesystem::current_path();

    // Determine number of threads in pool 
    unsigned int hw = std::thread::hardware_concurrency();
    if (hw < 2) hw = 2; // Minimum 2 threads based on assignments

    // Print starter header
    std::cout << "----- Search Started -----\n";
    std::cout << "Target Folder: " << rootDir.string() << "\n";
    std::cout << "Target Text: " << target << "\n";
    std::cout << "Using a Pool of " << hw << " threads to search.\n";

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
    std::cout << "----- Search Complete -----\n";
    return 0;
}