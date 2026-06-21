#pragma once

void* reader_thread_func(void* arg);
void* average_thread_func(void* arg);
void* sender_thread_avg_func(void* arg);
void* sender_thread_raw_func(void* arg);