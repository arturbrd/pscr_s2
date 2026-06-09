#include "main.hpp"
#include <iostream>
#include "threads.hpp"
#include "weather.hpp"

using json = nlohmann::json;

void* reader_thread_func(void* arg) {
    std::cout << "Hello from reader thread\n";
    char* buffer = new char[4096];
    while (true) {
        ssize_t bytes_received = mq_receive(mqtt_reader_queue, buffer, 4096, NULL);
        
        if (bytes_received == -1) {
            std::cerr << "Error: Couldn't read from a queue" << std::endl;
            break;
        } else {
            // std::cout << "Reader_thread: " << buffer << std::endl;
            json j = json::parse(buffer);
            Data d = j.get<Data>();
            std::cout << d.timestamp << std::endl;

            pthread_mutex_lock(&weather_data_mutex);
            if (weather_data.count(d.timestamp)) {
                weather_data[d.timestamp].records.insert(weather_data[d.timestamp].records.end(), d.records.begin(), d.records.end());
                std::cout << "Records count: " << weather_data[d.timestamp].records.size() << std::endl;
                if (weather_data[d.timestamp].records.size() == 125) {
                    std::cout << "WeatherMap full" << std::endl;
                    mq_send(ready_map_queue, (char*)&d.timestamp, sizeof(long), 0);
                }
            } else {
                WeatherMap map = {d.timestamp, d.records};
                weather_data[d.timestamp] = map;
                std::cout << "Created new WeatherMap" << std::endl;
            }
            pthread_mutex_unlock(&weather_data_mutex);
        }
    }

    delete[] buffer;
    return nullptr;
}

void* average_thread_func(void* arg) {
    std::cout << "Hello from average thread\n";
    char* buffer = new char[sizeof(long)];

    while (true) {
        ssize_t bytes_received = mq_receive(ready_map_queue, buffer, sizeof(long), NULL);
        long index;
        std::memcpy(&index, buffer, sizeof(long));
        
        if (bytes_received == -1) {
            std::cerr << "Error: Couldn't read from a queue" << std::endl;
            break;
        } else {
            pthread_mutex_lock(&weather_data_mutex);
            if (weather_data.count(index)) {
                WeatherMap map = weather_data[index];
                double avg = 0;
                for (auto& r : map.records) avg += r.temp_c;
                avg /= map.records.size();
                std::cout << "Timestamp: " << index << "Average temperature: " << avg << std::endl;
            } else {
                std::cerr << "For some reason, there are no such records" << std::endl;
            }
            pthread_mutex_unlock(&weather_data_mutex);
        


            
        }
    }

    return nullptr;
}

void* sender_thread_func(void* arg) {
    std::cout << "Hello from sender thread\n";

    while (true) {

    }

    return nullptr;
}