#include "main.hpp"
#include <iostream>

void* reader_thread_func(void* arg) {
    std::cout << "Hello from reader thread\n";
    char* buffer = new char[4096];
    while (true) {
        ssize_t bytes_received = mq_receive(mqtt_reader_queue, buffer, 4096, NULL);
        if (bytes_received == -1) {
            std::cerr << "Error: Couldn't read from a queue" << std::endl;
            break;
        } else {
            std::cout << buffer << std::endl;
        }
    }

    delete[] buffer;
    return nullptr;
}

void* average_thread_func(void* arg) {
    std::cout << "Hello from average thread\n";

    while (true) {

    }

    return nullptr;
}

void* sender_thread_func(void* arg) {
    std::cout << "Hello from sender thread\n";

    while (true) {

    }

    return nullptr;
}