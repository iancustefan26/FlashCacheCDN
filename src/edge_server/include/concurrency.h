#ifndef CONCURRENCY_H
#define CONCURRENCY_H
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

struct thread_info
{
    thread::id id;
    bool active;
    thread thread_obj;
};

class ThreadPool
{
    int size;
    int active;
    vector<thread_info> threads;
    static mutex accept_mtx; // Not initialized in the constructor because it is default constructable (when declared), static because all threads should have acces
public:
    ThreadPool(int size, void (*universal_work_function)(int), int main_socket);
    void join_all();
    void detach_all();
    void detach(int index);
    int get_size() const;
    int get_active() const;
    thread_info& operator[](int index);
    static void lock_mutex();
    static void unlock_mutex();
};

#endif