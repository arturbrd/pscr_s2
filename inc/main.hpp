#pragma once

#include <pthread.h>
#include <mqueue.h>
#include <string>
#include <unordered_map>
#include "weather.hpp"
#include <mqtt/async_client.h>


const std::string SERVER_ADDRESS = "tcp://25.34.193.58:1883";
const std::string CLIENT_ID = "test_sub";
const std::string TOPIC = "weathert/c1";
const int READER_THREADS_COUNT = 12;

extern mqd_t mqtt_reader_queue;
extern mqd_t ready_map_queue;
extern mqd_t mqtt_sender_queue_raw;
extern mqd_t mqtt_sender_queue_avg;

extern pthread_mutex_t weather_data_mutex;

extern mqtt::async_client client;

extern std::unordered_map<long, WeatherMap> weather_data;