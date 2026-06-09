#pragma once

#include <pthread.h>
#include <mqueue.h>
#include <string>
#include <unordered_map>
#include "weather.hpp"


const std::string SERVER_ADDRESS = "tcp://localhost:1883";
const std::string CLIENT_ID = "test_sub";
const std::string TOPIC = "test/topic";
const int READER_THREADS_COUNT = 12;

extern mqd_t mqtt_reader_queue;
extern mqd_t ready_map_queue;
extern pthread_mutex_t weather_data_mutex;

extern std::unordered_map<long, WeatherMap> weather_data;