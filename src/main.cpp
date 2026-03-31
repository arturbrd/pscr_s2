#include <iostream>
#include <pthread.h>
#include <mutex>
#include <mqtt/async_client.h>
#include <mqueue.h>
#include <cerrno>
#include <cstring>
#include "main.hpp"
#include "threads.hpp"

mqd_t mqtt_reader_queue;
pthread_mutex_t mqtt_buff_mutex;

class MqttCallback : public virtual mqtt::callback {
    public:
        void message_arrived(mqtt::const_message_ptr msg) override {
            std::string msg_string = msg->to_string();
            std::cout << "Topic: " << msg->get_topic() << std::endl;
            std::cout << "Message: " << msg_string << std::endl;

            if (mq_send(mqtt_reader_queue, msg_string.c_str(), msg_string.size() + 1, 0) == -1) {
                std::cerr << "Error: Couldn't send msg to queue" << std::endl;
            }
        }
};

int main() {
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    mqtt::connect_options connOpts;
    connOpts.set_mqtt_version(MQTTVERSION_5);

    MqttCallback mqtt_callback;
    client.set_callback(mqtt_callback);

    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 4096;

    mqtt_reader_queue = mq_open("/mqtt_reader_queue", O_CREAT | O_RDWR, 0666, &attr);
    if (mqtt_reader_queue == (mqd_t)-1) {
        std::cerr << "Error: Couldn't open queue" << std::endl
                    << strerror(errno) << std::endl;
        return 1;
    }

    if (pthread_mutex_init(&mqtt_buff_mutex, NULL) != 0) {
        std::cerr << "Error: Couldn't initialize mutex" << std::endl;
    }

    try {
        client.connect(connOpts)->wait();
        client.subscribe("test/topic", 1);

        client.subscribe("pscr/c12/0", 1);
        client.subscribe("pscr/c12/1", 1);
        client.subscribe("pscr/c12/2", 1);
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

    pthread_t sender_thread;
    pthread_create(&sender_thread, nullptr, sender_thread_func, nullptr);
    

    for (int i = 0; i < 12; i++) {
        pthread_join(reader_threads[i], nullptr);
    }
    pthread_join(average_thread, nullptr);
    pthread_join(sender_thread, nullptr);


    mq_close(mqtt_reader_queue);
    mq_unlink("/mqtt_reader_queue");

    return 0;
}