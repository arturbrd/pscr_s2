#pragma once

#include <pthread.h>
#include <mqueue.h>
#include <string>

const std::string SERVER_ADDRESS = "tcp://localhost:1883";
const std::string CLIENT_ID = "test_sub";
const std::string TOPIC = "test/topic";
const int READER_THREADS_COUNT = 12;

extern mqd_t mqtt_reader_queue;
extern pthread_mutex_t mqtt_buff_mutex;