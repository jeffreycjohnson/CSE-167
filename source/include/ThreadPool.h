#ifndef INCLUDE_THREADPOOL_H
#define INCLUDE_THREADPOOL_H

#include "ForwardDecs.h"
#include <functional>
#include <set>
#include <list>
#include <thread>
#include <mutex>

class ThreadPool
{
public:
    class Job
    {
    public:
        friend ThreadPool;

        int affinity = -1;
        int priority = 0;

        void queue();
        void addDependency(Job* other);
        Job(std::function<void()> func, ThreadPool* pool);

    private:
        ThreadPool* const pool;
        std::function<void()> func;
        std::set<Job*> dependencies;
        std::set<Job*> dependents;
    };

    explicit ThreadPool(size_t threadCount = 0);
    ~ThreadPool();

    const size_t threadCount;
    bool shutdown = false;

    Job* createJob(const std::function<void()>& func);
    bool completed(Job* job);
    void wait(Job* job);

private:
    static bool jobComparator(Job* first, Job* second);
    std::set<Job*, bool(*)(Job*, Job*)> readyQueue;
    std::list<std::thread> threads;
    std::mutex jobLock;

    static void runThread(ThreadPool* pool, size_t id);
};

#endif