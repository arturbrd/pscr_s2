#include <iostream>
#include <thread>
#include <mutex>

void task1() {
    while (true) {
        std::cout << "Task 1\n";
    }
}

int main() {
    printf("Hello world!\n");

    std::thread t1(task1);

    t1.join();
    return 0;
}