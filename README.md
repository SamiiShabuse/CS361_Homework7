# Your name and drexel ID

- Samii Shabuse
- sus24@drexel.edu

# Instructions to run my code

# Build and run:
1. From the project root run `make` to build the binary `bin/search`.
2. Run the program as `./bin/search <target> [directory]` where `<target>` is the search string and `[directory]` is optional (defaults to the current directory). Example:

```bash
make
./bin/search "Hi" /path/to/search
```
# Short Essay Questions

## Question 1: What data structures did you use/build? Why?
This project uses small, focused data structures such as a `Match` struct to hold match metadata, such as thread id, file path, line number, and line text, `std::queue` inside the buffered channel implementation by Professor Boady to store pending items, and `std::vector` to hold worker `std::thread` objects. The channels themselves are templated wrappers around these queues for unbuffered channels and coordinate access through mutexes and condition variables. These were chosen because queues naturally model FIFO task buffers and the `Match` struct packages the information producers/passers need to communicate results between threads.

## Question 2: How does the thread pool get it’s tasks?
The thread pool receives tasks from a producer thread via a `channel<std::filesystem::path>`. The producer recursively walks the root directory and sends each valid file path into the file channel, and each worker thread repeatedly calls `receive()` on that channel to obtain a file to process. This decouples file discovery from file processing and allows multiple worker threads to pull tasks concurrently from the same safe queue-like channel.

## Question 3: How does the thread pool know that it is finished all tasks
The thread pool know that is done by coodinating closing file channel. Once the producer finishes directory traversal it calls `fileChan->close()`, which causes worker threads' `receive()` calls to stop producing new tasks, they catch the channel-closed condition and exit their loop. The main thread joins the producer and all workers, then closes the result channel so the printer thread can exit. The joining guarantees the program knows when all work has completed.

## Question 4: How did you use channels for thread safety?
How I used channels for thread safety by using the channel.h given by Profesor Boady. The `bufferedChannel` a `std::queue` (`buffer`) is protected by `buffMut` with two condition variables: `send()` blocks when the queue size reaches `maxSize`, and `receive()` blocks when the queue is empty. Then `close()` sets `open = false` and notifies waiters. With the help of `isClosed()` returns true only when the channel is closed and the buffer is empty, ensuring receivers don't get partial data. For `unbufferedChannel` a handshake agreement is implemented using a placeholder `tempVal` plus `senderReady`/`receiverReady` flags so senders and receivers synchronize directly. By passing ownership of data through these channels, the design avoids data races and centralizes locking logic following the assignment requirements.

## Question 5: Think about your previous projects. What method programming with threads do you like best (lock/conditional variable/semaphore/channels)? Why?

After this assignment, I prefer using channels because they separate synchronization concerns from business logic. Channels hide locks and condition variables behind a simple send/receive interface, make reasoning about ownership and data flow a lot easier, and reduce common concurrency bugs like data races compared to ad-hoc shared-memory locking. In addition,they also scale well for producer/consumer and pipeline patterns, and they make it straightforward to add or remove worker threads without changing the core data-sharing logic. I definitely would prefer to take this approach depending on the type of problem I have to solve. 
