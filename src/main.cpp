#include <iostream>
#include <pthread.h>
#include <mutex>
#include <mqueue.h>
#include <cerrno>
#include <cstring>
#include "main.hpp"
#include "threads.hpp"


mqd_t mqtt_reader_queue;
mqd_t ready_map_queue;
mqd_t mqtt_sender_queue_raw;
mqd_t mqtt_sender_queue_avg;
pthread_mutex_t weather_data_mutex;
std::unordered_map<long, WeatherMap> weather_data;

mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);


class MqttCallback : public virtual mqtt::callback {
    public:
        void message_arrived(mqtt::const_message_ptr msg) override {
            std::string msg_string = msg->to_string();
            // std::cout << "Topic: " << msg->get_topic() << std::endl;
            // std::cout << "Message: " << msg_string << std::endl;

            if (mq_send(mqtt_reader_queue, msg_string.c_str(), msg_string.size() + 1, 0) == -1) {
                std::cerr << "Error: Couldn't send msg to queue" << std::endl;
            }
        }
};

int main() {

    mqtt::connect_options connOpts;
    connOpts.set_mqtt_version(MQTTVERSION_5);

    MqttCallback mqtt_callback;
    client.set_callback(mqtt_callback);

    struct mq_attr attr_mqtt;
    attr_mqtt.mq_maxmsg = 20;
    attr_mqtt.mq_msgsize = 4096;

    mqtt_reader_queue = mq_open("/mqtt_reader_queue", O_CREAT | O_RDWR, 0666, &attr_mqtt);
    if (mqtt_reader_queue == (mqd_t)-1) {
        std::cerr << "Error: Couldn't open queue" << std::endl
                    << strerror(errno) << std::endl;
        return 1;
    }

    struct mq_attr attr_mqtt_sender;
    attr_mqtt_sender.mq_maxmsg = 20;
    attr_mqtt_sender.mq_msgsize = 4096;

    mqtt_sender_queue_raw = mq_open("/mqtt_sender_queue_raw", O_CREAT | O_RDWR, 0666, &attr_mqtt_sender);
    if (mqtt_sender_queue_raw == (mqd_t)-1) {
        std::cerr << "Error: Couldn't open queue" << std::endl
                    << strerror(errno) << std::endl;
        return 1;
    }

    struct mq_attr attr_mqtt_sender_avg;
    attr_mqtt_sender_avg.mq_maxmsg = 4;
    attr_mqtt_sender_avg.mq_msgsize = 1024;

    mqtt_sender_queue_avg = mq_open("/mqtt_sender_queue_avg", O_CREAT | O_RDWR, 0666, &attr_mqtt_sender_avg);
    if (mqtt_sender_queue_avg == (mqd_t)-1) {
        std::cerr << "Error: Couldn't open queue" << std::endl
                    << strerror(errno) << std::endl;
        return 1;
    }

    struct mq_attr attr_ready;
    attr_ready.mq_maxmsg = 10;
    attr_ready.mq_msgsize = sizeof(long);

    ready_map_queue = mq_open("/ready_map_queue", O_CREAT | O_RDWR, 0666, &attr_ready);
    if (ready_map_queue == (mqd_t)-1) {
        std::cerr << "Error: Couldn't open queue" << std::endl
                    << strerror(errno) << std::endl;
        return 1;
    }

    if (pthread_mutex_init(&weather_data_mutex, NULL) != 0) {
        std::cerr << "Error: Couldn't initialize mutex" << std::endl;
    }

    try {
        client.connect(connOpts)->wait();
        client.subscribe("test/topic", 1);

        client.subscribe("weather/c1/0", 1);
        client.subscribe("weather/c1/1", 1);
        client.subscribe("weather/c1/2", 1);
        client.subscribe("weather/c1/3", 1);
        client.subscribe("weather/c1/4", 1);
        client.subscribe("weather/c1/5", 1);
        client.subscribe("weather/c1/6", 1);
        client.subscribe("weather/c1/7", 1);
        client.subscribe("weather/c1/8", 1);
        client.subscribe("weather/c1/9", 1);
        std::cout << "Connected!" << std::endl;
    } catch (const mqtt::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    pthread_t reader_threads[READER_THREADS_COUNT];

    for (int i = 0; i < READER_THREADS_COUNT; i++) {
        pthread_create(&reader_threads[i], nullptr, reader_thread_func, nullptr);
    }

    pthread_t average_thread;
    pthread_create(&average_thread, nullptr, average_thread_func, nullptr);

    pthread_t sender_thread_avg;
    pthread_create(&sender_thread_avg, nullptr, sender_thread_avg_func, nullptr);

    pthread_t sender_thread_raw;
    pthread_create(&sender_thread_raw, nullptr, sender_thread_raw_func, nullptr);
    

    for (int i = 0; i < 12; i++) {
        pthread_join(reader_threads[i], nullptr);
    }
    pthread_join(average_thread, nullptr);
    pthread_join(sender_thread_avg, nullptr);
    pthread_join(sender_thread_raw, nullptr);


    mq_close(mqtt_reader_queue);
    mq_unlink("/mqtt_reader_queue");

    mq_close(ready_map_queue);
    mq_unlink("/ready_map_queue");

    mq_close(mqtt_sender_queue_avg);
    mq_unlink("/mqtt_sender_queue_avg");

    mq_close(mqtt_sender_queue_raw);
    mq_unlink("/mqtt_sender_queue_raw");

    return 0;
}