#include "concurrency.h"

mutex ThreadPool::accept_mtx;

ThreadPool::ThreadPool(int size, void (*universal_work_function)(int), int main_socket)
{
    this->size = size;
    threads.resize(size); // Use resize to initialize each thread_info object

    for (int i = 0; i < size; i++) {
        // Initialize thread properly
        threads[i].thread_obj = thread(universal_work_function, main_socket);
        threads[i].active = true;
        threads[i].id = threads[i].thread_obj.get_id();
    }
    this->active = size;
}


void ThreadPool::detach(int index)
{
    this -> threads[index].thread_obj.detach();
}

void ThreadPool::detach_all()
{
    for (thread_info& t : threads)
        t.thread_obj.detach();
}

int ThreadPool::get_size() const
{
    return size;
}

int ThreadPool::get_active() const
{
    return active;
}

thread_info& ThreadPool::operator[](int index)
{
    return threads[index];
}


void ThreadPool::join_all()
{
    for (thread_info& t : threads)
        if (t.thread_obj.joinable())
            t.thread_obj.join();
}

void ThreadPool::lock_mutex() {
    try {
        accept_mtx.lock();
    }
    catch (const std::exception& e) {
        std::cerr << "Mutex lock failed: " << e.what() << std::endl;
        throw; // rethrow
    }
}

void ThreadPool::unlock_mutex() {
    try {
        accept_mtx.unlock();
    }
    catch (const std::exception& e) {
        std::cerr << "Mutex unlock failed: " << e.what() << std::endl;
        throw; // rethrow
    }
}
