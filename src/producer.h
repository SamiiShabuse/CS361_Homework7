#ifndef PRODUCER_H
#define PRODUCER_H

#include <string>
#include <filesystem>
#include "channel.h"

// Producer thread entry point
void producerThreadFunc(
    const std::filesystem::path& rootDir,
    channel<std::filesystem::path>* fileChan
);

#endif // PRODUCER_H